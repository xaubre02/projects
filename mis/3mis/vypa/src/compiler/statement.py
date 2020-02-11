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


class StatementType(Enum):
    """Enumeration of statement types."""
    # self-explanatory
    VARIABLE_DEFINITION = 0x00
    ASSIGNMENT = 0x01
    CONDITION = 0x02
    ITERATION = 0x03
    FUNCTION_CALL = 0x04
    RETURN_CALL = 0x05


class Statement:
    """Class representing a program statement."""

    def __init__(self, ctx, callee, sym_table):
        """Initialize the statement."""
        self._args = []    # function call arguments
        self._func = None  # called function
        self._obj = None   # object of the called method
        self._exp = None
        self._calle = callee

        # variable definiton
        vd_ctx = ctx.variable_definition()
        if vd_ctx is not None:
            self._type = StatementType.VARIABLE_DEFINITION
            vtype = VariableType.recognize(vd_ctx)       # variable type
            vcls = VariableType.recognize_class(vd_ctx)  # variable class
            # multiple variables declared at once
            self._variables = []
            for name in vd_ctx.ID():
                name = name.symbol
                if sym_table.is_defined(name.text):
                    raise SemanticErrorException('\'{}\' is already defined'.format(name.text), vd_ctx.start.line, vd_ctx.start.column)
                variable = Variable(name=name.text,
                                         dtype=vtype,
                                         vclass=vcls,
                                         line=name.line,
                                         column=name.column)
                sym_table.store(variable)
                self._variables.append(variable)

        # assignment
        elif ctx.assignment() is not None:
            self._type = StatementType.ASSIGNMENT
            p = Path(ctx.assignment().path(), sym_table, callee.def_class)
            exp = Expression(ctx.assignment().expr(), sym_table, callee)
            self._exp = exp
            self._target = p.target
            if p.value_type != exp.value_type:
                raise SemanticErrorException('cannot assign {} to {}'.format(p.value_type.value, exp.value_type.value), ctx.start.line, ctx.start.column, code=CompilerError.TYPE_ERROR)

        # condition
        elif ctx.conditional() is not None:
            self._type = StatementType.CONDITION
            exp = Expression(ctx.conditional().expr(), sym_table, callee)
            self._exp = exp
            # check the expression type
            if exp.value_type not in [VariableType.INT, VariableType.OBJECT]:
                raise SemanticErrorException('conditional expression must be of type int or an object', ctx.start.line, ctx.start.column, code=CompilerError.TYPE_ERROR)
            # evaluate the codeblocks
            self._true_statements = []
            self._false_statements = []
            if len(ctx.conditional().codeblock()) != 2:
                #almost imposilble (else if is not exist)
                raise Expression(ctx.conditional().expr(), sym_table, callee)

            for stmt in ctx.conditional().codeblock()[0].statement():
                s = Statement(stmt, callee, sym_table)
                self._true_statements.append(s)

            for stmt in ctx.conditional().codeblock()[1].statement():
                s = Statement(stmt, callee, sym_table)
                self._false_statements.append(s)


        # iteration
        elif ctx.iteration() is not None:
            self._type = StatementType.ITERATION
            exp = Expression(ctx.iteration().expr(), sym_table, callee)
            self._exp = exp
            # check the expression type
            if exp.value_type not in [VariableType.INT, VariableType.OBJECT]:
                raise SemanticErrorException('conditional expression must be of type int or an object', ctx.start.line, ctx.start.column, code=CompilerError.TYPE_ERROR)
            self._iterations_statements = []
            # evaluate the codeblock
            for stmt in ctx.iteration().codeblock().statement():
                # new scope
                s = Statement(stmt, callee, sym_table)
                self._iterations_statements.append(s)

        # function call
        elif ctx.function_call() is not None:
            self._type = StatementType.FUNCTION_CALL
            # function name
            func_name = ctx.function_call().ID().getText()
            # path to the function -> it's a method
            if ctx.function_call().path() is not None:
                self._obj = Path(ctx.function_call().path(), sym_table, callee.def_class).target
                if self._obj.type == EntityType.CLASS:
                    # get the class method
                    func = self._obj.get_member(func_name)
                else:
                    if self._obj.data_type != VariableType.OBJECT:
                        raise SemanticErrorException('\'{}\' is not an object'.format(self._obj.name), ctx.function_call().path().start.line, ctx.function_call().path().start.column)
                    # get the class method
                    # self._obj = sym_table.get(self._obj.object_class)
                    cls = sym_table.get(self._obj.object_class)
                    func = cls.get_member(func_name)
            # function call from an expression
            elif ctx.function_call().expr() is not None:
                exp = Expression(ctx.function_call().expr(), sym_table, callee)
                # expression has to be an object
                if exp.value_type != VariableType.OBJECT:
                    raise SemanticErrorException('expression does not return an object', ctx.function_call().expr().start.line, ctx.function_call().expr().start.column)

                self._obj = exp.target
                func = self._obj.get_member(func_name)
            else:
                # get the function
                if not sym_table.is_defined(func_name):
                    raise SemanticErrorException('function \'{}\' is not defined'.format(func_name), ctx.start.line, ctx.start.column)
                func = sym_table.get(func_name)
                # not a function
                if func.type != EntityType.FUNCTION:
                    raise SemanticErrorException('\'{}\' is not callable'.format(func.name), ctx.start.line, ctx.start.column)

            # check function signature
            args = ctx.function_call().arguments()
            func.check_signature(args, sym_table, callee)
            # save the function and arguments arguments
            self._func = func
            for exp in args.expr():
                self._args.append(Expression(exp, sym_table, callee))

        # return call
        elif ctx.return_call() is not None:
            self._type = StatementType.RETURN_CALL
            if ctx.return_call().expr() is None:
                rc_type = VariableType.VOID  # only return;
            else:
                exp = Expression(ctx.return_call().expr(), sym_table, callee)
                rc_type = exp.value_type  # return expr;
                self._ret_exp = exp
            self._rc_type = rc_type
            # return call type mismatch
            if rc_type != callee.return_type:
                raise SemanticErrorException('return type differs from the function return type', code=CompilerError.TYPE_ERROR)


    @property
    def type(self) -> StatementType:
        """Return the type of this statement."""
        return self._type

    def code(self, code_creator: CodeCreator) -> None:
        """Return the code for this statement."""
        if self.type == StatementType.VARIABLE_DEFINITION:
            # do nothing, stack is already allocated
            for variable in self._variables:
                val = 0
                if variable.data_type == VariableType.STRING:
                    val = "\"\""
                if not variable._class_type:
                    code_creator.set('[$FP+{}]'.format(variable.stack_index), val)
                else:
                    code_creator.get_word("$ACC2", "$VT", variable._class_type.cls_index)
                    code_creator.set_word('$ACC2', "$SELF", val)
        elif self.type == StatementType.ASSIGNMENT:
            self._exp.code(code_creator)
            if not self._target._class_type:
                code_creator.set('[$FP+{}]'.format(self._target.stack_index), '$ACC1')
            else:
                code_creator.get_word("$ACC2", "$VT", self._target._class_type.cls_index)
                code_creator.set_word('$ACC2', "$SELF", '$ACC1')
        elif self.type == StatementType.CONDITION:
            self._calle.conditions_counter +=1
            counter = self._calle.conditions_counter
            self._exp.code(code_creator)
            code_creator.jumpz('{}_{}_{}'.format(self._calle.name, counter, 'else'),'$ACC1')
            code_creator.label('{}_{}_{}'.format(self._calle.name, counter, 'if'))
            for stmt in self._true_statements:
                stmt.code(code_creator)
            code_creator.jump('{}_{}_{}'.format(self._calle.name, counter, 'endif'))
            code_creator.label('{}_{}_{}'.format(self._calle.name, counter, 'else'))
            for stmt in self._false_statements:
                stmt.code(code_creator)
            code_creator.label('{}_{}_{}'.format(self._calle.name, counter, 'endif'))
        elif self.type == StatementType.ITERATION:
            self._calle.iteration_counter += 1
            counter = self._calle.iteration_counter
            code_creator.label('{}_{}_{}'.format(self._calle.name, counter, 'while_condition'))
            self._exp.code(code_creator)
            code_creator.jumpz('{}_{}_{}'.format(self._calle.name, counter, 'while_end'), '$ACC1')
            code_creator.label('{}_{}_{}'.format(self._calle.name, counter, 'while_begin'))
            for stmt in self._iterations_statements:
                stmt.code(code_creator)
            code_creator.jump('{}_{}_{}'.format(self._calle.name, counter, 'while_condition'))
            code_creator.label('{}_{}_{}'.format(self._calle.name, counter, 'while_end'))
        elif self.type == StatementType.FUNCTION_CALL:
            if self._func is None:
                raise InternalErrorException('unknown function in function call')

            # catch built-in functions
            if self._func.name == 'print':
                for exp in self._args:
                    exp.code(code_creator)
                    if exp.value_type == VariableType.INT:
                        code_creator.writei("$ACC1")
                    else:
                        code_creator.writes("$ACC1")
                return

            #store FP
            code_creator.addi("$SP", "$SP", 1)
            code_creator.set("[$SP]", "$FP")
            if self._obj and isinstance(self._obj, Variable):
                #store self argument
                self._obj.code(code_creator)
                code_creator.addi("$SP", "$SP", 1)
                code_creator.set("[$SP]", "$ACC1")
            #set arguments
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

            #call function
            code_creator.addi("$SP", "$SP", 1)
            code_creator.call("[$SP]", "$ACC1")
            code_creator.subi("$SP", "$SP", 1)

            #clear arguments
            code_creator.subi("$SP", "$SP", len(self._args))
            #clear self
            if self._obj and isinstance(self._obj, Variable):
                code_creator.subi("$SP", "$SP", 1)

            #restore FP
            code_creator.set("$FP", "[$SP]")
            code_creator.subi("$SP", "$SP", 1)


        elif self.type == StatementType.RETURN_CALL:
            if self._rc_type != VariableType.VOID:
                self._ret_exp.code(code_creator)
                code_creator.set("$RETVAL", "$ACC1")
                code_creator.set("$SP", "$FP")
                code_creator.return_i("[$FP]")

        # should not happen
        else:
            raise InternalErrorException('unknown statement type')
