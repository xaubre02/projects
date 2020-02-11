#!/usr/bin/env python3
"""
- Project: VYPa19 language compiler
- Author: Tomas Aubrecht (xaubre02), Jan Kotras (xkotra01)
- Date:   2019-10-25
- Desc:   compiler.
"""

from enum import Enum
from compiler import *
from compiler.code_creator import CodeCreator


class ExpressionType(Enum):
    """Enumeration of expression types."""
    VALUE = 0x06
    NEW_OBJECT = 0x07
    CAST_INT = 0x08
    CAST_OBJECT = 0x09
    FUNCTION_CALL = 0x0A
    NEGATION = 0x0B
    MUL = 0x0C  # int * int
    DIV = 0x0D  # int / int
    ADD = 0x0E  # int + int
    CONCAT = 0x0F  # string + string
    SUB = 0x10  # int - int
    LT = 0x11  # less than
    LEQ = 0x12  # less than or qeual to
    GT = 0x13  # greater than
    GTEQ = 0x14  # greater than
    EQ = 0x15  # equal to
    NEQ = 0x16  # not nequal to
    AND = 0x17  # logical AND
    OR = 0x18  # logical O


class Expression:
    """Class representing an expression."""
    concat_counter = 0

    def __init__(self, ctx, scope, callee):
        """Initialize the expression."""
        # debug
        if isinstance(ctx, list):
            if len(ctx) != 1:
                InternalErrorException('expression initialization got multiple contexts')
            ctx = ctx[0]

        self._target = None  # target of a path expression
        self._val = None     # expression value
        self._func = None  # called function
        self._args = []  # function call arguments
        self._concat_cnt = Expression.concat_counter

        # expression is a value
        if ctx.value() is not None:
            self._type = ExpressionType.VALUE
            # expression is an integer
            if ctx.value().INTEGER() is not None:
                self._val_type = VariableType.INT
                self._val = int(ctx.value().INTEGER().symbol.text)
            # expression is a string
            elif ctx.value().STRING_LITERAL() is not None:
                self._val_type = VariableType.STRING
                self._val = ctx.value().STRING_LITERAL().symbol.text
            # expression is a path
            elif ctx.value().path() is not None:
                p = Path(ctx.value().path(), scope, callee.def_class)
                self._val_type = p.value_type
                self._target = p.target
                self._val = self.target
        # expression is a creation of an object
        elif ctx.NEW() is not None:
            self._type = ExpressionType.NEW_OBJECT
            self._val_type = VariableType.OBJECT
            name = ctx.ID().getText()
            # check if the given class is defined
            self._target = scope.get(name, EntityType.CLASS)
            self._val = self.target
        # casting
        elif ctx.data_type() is not None:
            # casting to this type
            self._val_type = VariableType.recognize(ctx)
            exp = Expression(ctx.expr(), scope, callee)
            # casting int to a string
            if self.value_type == VariableType.STRING and exp.value_type == VariableType.INT:
                self._type = ExpressionType.CAST_INT
                self._val = str(exp.value)
            elif self.value_type == VariableType.OBJECT and exp.value_type == VariableType.OBJECT:
                self._type = ExpressionType.CAST_OBJECT
                # class has to be defined
                c = VariableType.recognize_class(ctx)
                if not scope.is_defined(c):
                    raise SemanticErrorException('cannot cast: class {} is not defined'.format(c), ctx.start.line, ctx.start.column, code=CompilerError.TYPE_ERROR)
                elif scope.get(c).type != EntityType.CLASS:
                    raise SemanticErrorException('cannot cast: \'{}\' is not a data type'.format(c), ctx.start.line, ctx.start.column, code=CompilerError.TYPE_ERROR)
                self._val = scope.get(c, EntityType.CLASS)

            else:
                raise SemanticErrorException('cannot cast {} to {}'.format(exp.value_type.value, self.value_type.value), ctx.start.line, ctx.start.column, code=CompilerError.TYPE_ERROR)
        # function call
        elif ctx.arguments() is not None:
            self._type = ExpressionType.FUNCTION_CALL
            # function name
            func_name = ctx.ID().getText()
            # expr is specified(path to the function) -> it's a method
            if len(ctx.expr()) == 1:
                self._target = Expression(ctx.expr()[0], scope, callee).target
                if self.target.type == EntityType.CLASS:
                    # get the class method
                    func = self.target.get_member(func_name)
                else:
                    if self.target is None or self.target.data_type != VariableType.OBJECT:
                        raise SemanticErrorException('given expression does not specify an object', ctx.expr()[0].start.line, ctx.expr()[0].start.column)
                    # get the class method
                    func = scope.get(self.target.object_class).get_member(func_name)
            else:
                # get the function
                if not scope.is_defined(func_name):
                    raise SemanticErrorException('function \'{}\' is not defined'.format(func_name), ctx.start.line, ctx.start.column)
                func = scope.get(func_name)
                # not a function
                if func.type != EntityType.FUNCTION:
                    raise SemanticErrorException('\'{}\' is not callable'.format(func.name), ctx.start.line, ctx.start.column)

            # check function signature
            func.check_signature(ctx.arguments(), scope)
            # expression type equals to function return type
            self._val_type = func.return_type
            if func.return_type_class is not None:
                self._target = scope.get(func.return_type_class, EntityType.CLASS)
                self._val = self.target
            else:
                pass
            self._func = func
            args = ctx.arguments()
            for exp in args.expr():
                self._args.append(Expression(exp, scope, callee))
        # negation
        elif ctx.getChildCount() == 2 and ctx.getChild(0).getText() == '!':
            exp = Expression(ctx.getChild(1), scope, callee)
            self._op1 = exp
            self._type = ExpressionType.NEGATION
            self._val_type = exp.value_type
            if self.value_type != VariableType.INT:
                raise SemanticErrorException('only integers can be negated', ctx.start.line, ctx.start.column, code=CompilerError.TYPE_ERROR)
            self._target = exp.target
            self._val = 0 if exp.value else 1  # negation
        elif ctx.getChildCount() == 3:
            # parentheses -> skip (only used for precedence)
            if ctx.getChild(0).getText() == '(' and ctx.getChild(2).getText() == ')':
                exp = Expression(ctx.getChild(1), scope, callee)
                self._type = exp.type
                self._target = exp.target
                self._val_type = exp.value_type
                self._val = exp.value
                self._args = exp._args
                self._func = exp._func
                return

            operator = ctx.getChild(1).getText()              # operator
            op1 = Expression(ctx.getChild(0), scope, callee)  # operand #1
            op2 = Expression(ctx.getChild(2), scope, callee)  # operand #2
            self._op1 = op1
            self._op2 = op2
            # arithmetic operation
            if operator in ['*', '/', '+', '-']:
                if op1.value_type != op2.value_type:
                    raise SemanticErrorException('operand type mismatch: {} and {}'.format(op1.value_type.value, op2.value_type.value), ctx.start.line, ctx.start.column)
                if op1.value_type != VariableType.INT and operator != '+':
                    raise SemanticErrorException('arithmetic operation over non-integer value', ctx.start.line, ctx.start.column)
                # operation result data type
                self._val_type = op1.value_type
                if operator == '*':
                    self._type = ExpressionType.MUL
                elif operator == '/':
                    self._type = ExpressionType.DIV
                elif operator == '+':
                    # addition
                    if op1.value_type == VariableType.INT:
                        self._type = ExpressionType.ADD
                    # concatenation of strings
                    elif op1.value_type == VariableType.STRING:
                        self._type = ExpressionType.CONCAT
                        self._concat_cnt = Expression.concat_counter
                        Expression.concat_counter += 1
                    else:
                        raise SemanticErrorException('addition of two object is not possible', ctx.start.line, ctx.start.column)
                elif operator == '-':
                    self._type = ExpressionType.SUB
            # relation (P × P → {0,1}) and comparison (T × T → {0,1})
            elif operator in ['<', '<=', '>', '>=', '==', '!=']:
                self._val_type = VariableType.INT
                if op1.value_type != op2.value_type:
                    raise SemanticErrorException('operand type mismatch: {} and {}'.format(op1.value_type.value, op2.value_type.value), ctx.start.line, ctx.start.column)
                # relation: only integers and strings are allowed
                # comparison: integers, strings and objects are allowed
                if (operator in ['<', '<=', '>', '>='] and op1.value_type not in [VariableType.INT, VariableType.STRING]) or \
                   (operator in ['==', '!='] and op1.value_type not in [VariableType.INT, VariableType.STRING, VariableType.OBJECT]):
                    raise SemanticErrorException('invalid relation between {} and {}'.format(op1.value_type.value, op2.value_type.value), ctx.start.line, ctx.start.column)
                if operator == '<':
                    self._type = ExpressionType.LT
                elif operator == '<=':
                    self._type = ExpressionType.LEQ
                elif operator == '>':
                    self._type = ExpressionType.GT
                elif operator == '>=':
                    self._type = ExpressionType.GTEQ
                elif operator == '==':
                    self._type = ExpressionType.EQ
                elif operator == '!=':
                    self._type = ExpressionType.NEQ
            # logical operators (NC × NC → {0,1})
            elif operator in ['&&', '||']:
                self._val_type = VariableType.INT
                # only integers and objects are allowed
                if op1.value_type not in [VariableType.INT, VariableType.OBJECT] or op2.value_type not in [VariableType.INT, VariableType.OBJECT]:
                    raise SemanticErrorException('logical operation over {} and {}'.format(op1.value_type.value, op2.value_type.value), ctx.start.line, ctx.start.column)
                if operator == '&&':
                    self._type = ExpressionType.AND
                else:
                    self._type = ExpressionType.OR
            else:
                raise InternalErrorException('unprocessed expression type')

    @property
    def type(self) -> ExpressionType:
        """Return the type of this expression."""
        return self._type

    @property
    def value(self):
        """Return the value of this expression."""
        return self._val

    @property
    def value_type(self) -> VariableType:
        """Return the type of the value of this expression."""
        return self._val_type

    @property
    def target(self) -> Entity:
        """Return the entity specified by a path expression."""
        return self._target

    def code(self, code_creator: CodeCreator):
        # result will be on the top of stack
        if self.type == ExpressionType.VALUE:
            if isinstance(self.value, Variable):
                self.value.code(code_creator)
            else:
                code_creator.set("$ACC1", self.value)
        elif self.type == ExpressionType.NEW_OBJECT:
            self._val.code_new_obj(code_creator)
            pass
        elif self.type == ExpressionType.CAST_INT:
            pass
        elif self.type == ExpressionType.CAST_OBJECT:
            pass
        elif self.type == ExpressionType.FUNCTION_CALL:
            code_creator.comment("start")
            # store FP
            code_creator.addi("$SP", "$SP", 1)
            code_creator.set("[$SP]", "$FP")
            if self._target and isinstance(self._target, Variable):
                #store self argument
                self._target.code(code_creator)
                code_creator.addi("$SP", "$SP", 1)
                code_creator.set("[$SP]", "$ACC1")
            # set arguments
            for exp in self._args:
                exp.code(code_creator)
                code_creator.addi("$SP", "$SP", 1)
                code_creator.set("[$SP]", "$ACC1")

            # function name
            if self._func.def_class:
                code_creator.get_word("$ACC2", "$VT", self._func.def_class.cls_index)
                code_creator.get_word("$ACC1", "$ACC2", self._func.method_index)
            else:
                code_creator.set("$ACC1", '\"{}\"'.format(self._func.name))

            # call function
            code_creator.addi("$SP", "$SP", 1)
            code_creator.call("[$SP]", "$ACC1")
            code_creator.subi("$SP", "$SP", 1)

            # clear arguments
            code_creator.subi("$SP", "$SP", len(self._args))
            # clear self
            if self._target and isinstance(self._target, Variable):
                code_creator.subi("$SP", "$SP", 1)

            # restore FP
            code_creator.set("$FP", "[$SP]")
            code_creator.subi("$SP", "$SP", 1)
            code_creator.set("$ACC1", "$RETVAL")
            code_creator.comment("end")
        elif self.type == ExpressionType.NEGATION:
            self._op1.code(code_creator)
            code_creator.not_i("$ACC1", "$ACC1")
        elif self.type == ExpressionType.MUL:
            self._op1.code(code_creator)
            code_creator.addi("$SP", "$SP", 1)
            code_creator.set("[$SP]", "$ACC1")
            self._op2.code(code_creator)
            code_creator.muli("$ACC1", "[$SP]", "$ACC1")
            code_creator.subi("$SP", "$SP", 1)
        elif self.type == ExpressionType.DIV:
            self._op1.code(code_creator)
            code_creator.addi("$SP", "$SP", 1)
            code_creator.set("[$SP]", "$ACC1")
            self._op2.code(code_creator)
            code_creator.divi("$ACC1", "[$SP]", "$ACC1")
            code_creator.subi("$SP", "$SP", 1)
        elif self.type == ExpressionType.ADD:
            self._op1.code(code_creator)
            code_creator.addi("$SP", "$SP", 1)
            code_creator.set("[$SP]", "$ACC1")
            self._op2.code(code_creator)
            code_creator.addi("$ACC1","$ACC1","[$SP]")
            code_creator.subi("$SP", "$SP", 1)
        elif self.type == ExpressionType.CONCAT:
            code_creator.addi("$SP", "$SP", 3)
            self._op1.code(code_creator)
            code_creator.set("[$SP-2]", "$ACC1")    #op1
            self._op2.code(code_creator)
            code_creator.set("[$SP-1]", "$ACC1")      #op2
            #allocate
            code_creator.get_size('$ACC1', "[$SP-2]")
            code_creator.get_size('$ACC2', "[$SP-1]")
            code_creator.addi('$ACC1','$ACC1','$ACC2')
            code_creator.create("$RETVAL", "$ACC1")
            code_creator.copy('$RETVAL','[$SP-2]')
            code_creator.resize("$RETVAL", "$ACC1")
            code_creator.set("$ACC4", '[$SP-1]')
            code_creator.get_size('$ACC1', "[$SP-1]")
            code_creator.set("[$SP-1]", '$ACC1')
            code_creator.get_size('$ACC1', "[$SP-2]")
            code_creator.set("[$SP-2]", '$ACC1')
            code_creator.addi("$ACC1","[$SP-1]","[$SP-2]")
            code_creator.set("[$SP]", '$ACC1')
            # [$SP-2] - size of op1
            # [$SP-1] - size of op2
            # [$SP] - size of op2+op1
            # ACC4 - op2 value
            # ACC2 - iterator
            # copy cycle
            code_creator.set('$ACC2',0)
            code_creator.label('concat_cycle_condition_start_{}'.format(self._concat_cnt))
            code_creator.addi('$ACC1', '$ACC2', '[$SP-2]')
            code_creator.addi('$ACC1', '$ACC1', 1)
            code_creator.gti("$ACC1", "$ACC1", "[$SP]")
            code_creator.jumpnz('concat_return_part_{}'.format(self._concat_cnt), "$ACC1")
            code_creator.get_word("$ACC3", "$ACC4", "$ACC2")
            code_creator.addi('$ACC1', '$ACC2','[$SP-2]')
            code_creator.set_word("$RETVAL", "$ACC1", "$ACC3")
            code_creator.addi("$ACC2", "$ACC2", 1)
            code_creator.jump('concat_cycle_condition_start_{}'.format(self._concat_cnt))

            code_creator.label('concat_return_part_{}'.format(self._concat_cnt))
            code_creator.set('$ACC1', '$RETVAL')

            code_creator.subi("$SP", "$SP", 3)
        elif self.type == ExpressionType.SUB:
            self._op1.code(code_creator)
            code_creator.addi("$SP", "$SP", 1)
            code_creator.set("[$SP]", "$ACC1")
            code_creator.set("[$SP]", "$ACC1")
            self._op2.code(code_creator)
            code_creator.subi("$ACC1", "[$SP]", "$ACC1")
            code_creator.subi("$SP", "$SP", 1)
        elif self.type == ExpressionType.LT:
            self._op1.code(code_creator)
            code_creator.set("$ACC2", "$ACC1")
            self._op2.code(code_creator)
            code_creator.lti("$ACC1", "$ACC2", "$ACC1")
        elif self.type == ExpressionType.LEQ:
            self._op1.code(code_creator)
            code_creator.set("$ACC2", "$ACC1")
            self._op2.code(code_creator)
            code_creator.eqi("$ACC3", "$ACC2", "$ACC1")
            code_creator.lti("$ACC1", "$ACC2", "$ACC1")
            code_creator.or_i("$ACC1", "$ACC1", "$ACC3")
        elif self.type == ExpressionType.GT:
            self._op1.code(code_creator)
            code_creator.set("$ACC2", "$ACC1")
            self._op2.code(code_creator)
            code_creator.gti("$ACC1", "$ACC2", "$ACC1")
        elif self.type == ExpressionType.GTEQ:
            self._op1.code(code_creator)
            code_creator.set("$ACC2", "$ACC1")
            self._op2.code(code_creator)
            code_creator.eqi("$ACC3", "$ACC2", "$ACC1")
            code_creator.gti("$ACC1", "$ACC2", "$ACC1")
            code_creator.or_i("$ACC1", "$ACC1", "$ACC3")
        elif self.type == ExpressionType.EQ:
            self._op1.code(code_creator)
            code_creator.set("$ACC2", "$ACC1")
            self._op2.code(code_creator)
            if self._op1._val_type == VariableType.STRING or self._op2._val_type == VariableType.STRING:
                code_creator.eqs("$ACC1", "$ACC2", "$ACC1")
            else:
                code_creator.eqi("$ACC1", "$ACC2", "$ACC1")
        elif self.type == ExpressionType.NEQ:
            self._op1.code(code_creator)
            code_creator.set("$ACC2", "$ACC1")
            self._op2.code(code_creator)
            if self._op1._val_type == VariableType.STRING or self._op2._val_type == VariableType.STRING:
                code_creator.eqs("$ACC1", "$ACC2", "$ACC1")
            else:
                code_creator.eqi("$ACC1", "$ACC2", "$ACC1")
            code_creator.not_i("$ACC1","$ACC1")
        elif self.type == ExpressionType.AND:
            self._op1.code(code_creator)
            code_creator.set("$ACC2", "$ACC1")
            self._op2.code(code_creator)
            code_creator.addi("$ACC1", "$ACC2", "$ACC1")
        elif self.type == ExpressionType.OR:
            self._op1.code(code_creator)
            code_creator.set("$ACC2", "$ACC1")
            self._op2.code(code_creator)
            code_creator.or_i("$ACC1", "$ACC2", "$ACC1")



