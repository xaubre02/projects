#!/usr/bin/env python3
"""
- Project: VYPa19 language compiler
- Author: Tomas Aubrecht (xaubre02), Jan Kotras (xkotra01)
- Date:   2019-10-25
- Desc:   compiler.
"""

from compiler import *
from compiler.code_creator import CodeCreator
from compiler.code_generator import CodeGenerator


class Function(Entity):
    """Class representing a function in a source program."""

    BUILT_IN_FUNCTION_NAMES = ['print', 'readInt', 'readString', 'length', 'subStr']

    def __init__(self, ctx=None, sym_table=None, name=None, rtype=None, params=None, def_class=None, line=None, column=None, built_in=False):
        """Initialize the function."""
        # only the context and symbol table have to be specified or name and return type have to be specified
        if (ctx is not None and sym_table is not None and (name is not None or rtype is not None or params is not None or line is not None or column is not None)) or \
           (ctx is None and (name is None or rtype is None)):
            raise InternalErrorException('invalid Function initialization arguments')

        # class where is the function(method) defined (None if global function)
        self._dclass = def_class
        self._rtype_cls = None
        self.sym_table = SymbolTable(sym_table)
        # built-in functions
        if built_in:
            super().__init__(EntityType.FUNCTION, name=name, line=line, column=column)
            self._rtype = rtype
            self._params = [] if params is None else params
        # use context to initialize the function
        else:
            super().__init__(EntityType.FUNCTION, ctx=ctx)
            # function return type
            self._rtype = VariableType.recognize(ctx)
            self._rtype_cls = VariableType.recognize_class(ctx)
            # function parameters
            self._params = []  # no parameters(void)
            param_list = ctx.param_list()
            if param_list is not None:  # void or at least one parameter is specified
                # void is specified
                params = param_list.VOID()
                # at least one parameter is specified
                if params is None:
                    for p in param_list.parameter():
                        self._params.append(Variable(p))

            # symbol table for this function
            self.sym_table = SymbolTable(sym_table)
            # create a code block for this function (list of statements)
            self._codeblock = ctx.codeblock().statement()

        # built-in function
        self._bin = built_in

        #conditions counter
        self.conditions_counter = 0
        self.iteration_counter = 0

        # method index
        self.method_index = 0


    @property
    def built_in(self) -> bool:
        """Return True if the function is built-in, False otherwise."""
        return self._bin

    @property
    def body(self) -> [Statement]:
        """Return the function body - list of statements."""
        return self._codeblock

    @property
    def class_context_name(self):
        return "{}::{}".format(self.def_class.name, self.name)

    def init(self) -> None:
        """Initialize the fucntion (second pass)."""
        # return type
        if self._rtype_cls is not None:
            if not self.sym_table.is_defined(self._rtype_cls):
                raise SemanticErrorException('return type \'{}\' is not defined'.format(self._rtype_cls), self.line, self.column)

        # store parameters
        i = -1
        for p in reversed(self.parameters):
            if p.data_type == VariableType.OBJECT and not self.sym_table.is_defined(p.object_class):
                raise SemanticErrorException('parameter type \'{}\' is not defined'.format(p.object_class), p.line, p.column)
            if isinstance(p, Variable):
                p.stack_index = i
                i -= 1
            self.sym_table.store(p)
        # replace a statementContext with a Statement objects
        for i, s in enumerate(self._codeblock):
            self._codeblock[i] = Statement(s, self, self.sym_table)

    def code(self):
        """Generate an assembly code."""
        code_creator = CodeCreator.code_creator()
        code_creator.comment("")
        code_creator.comment("")
        if not self.def_class:
            code_creator.comment("Function: {}".format(self.name))
            code_creator.comment("-------------------------------")
            code_creator.label(self.name)
        else:
            code_creator.comment("Method: {}".format(self.name))
            code_creator.comment("-------------------------------")
            code_creator.label(self.class_context_name)

        #PC is stored automaticky to stack by CALL function
        #set frame pointer
        code_creator.set("$FP", "$SP")

        if self._dclass:
            #store self pointer
            code_creator.comment("start self")
            code_creator.addi("$SP", "$SP", 1)
            code_creator.set("[$SP]", "$SELF")
            self_stack_index = len(self.parameters)+1
            code_creator.set("$SELF", '[$FP-{}]'.format(self_stack_index))
            code_creator.comment("end self")

        #handle statements
        if self.name in self.BUILT_IN_FUNCTION_NAMES:
            self._code_built_in_functions(code_creator)
        elif self.name in ['toString', 'getClass', 'Object'] and self._dclass and self._dclass.name == 'Object':
            # built in methods
            self._code_built_in_functions(code_creator)
        else:
            if self.sym_table.symbols:
                index = 1
                for name, symbol in self.sym_table.symbols.items():
                    if symbol in self.parameters:
                        continue
                    symbol.stack_index = index
                    index += 1
                count_local_variables = len(self.sym_table.symbols)
                code_creator.addi('$SP', '$SP', count_local_variables)
                for name, symbol in self.sym_table.symbols.items():
                    if symbol in self.parameters:
                        continue
                    if symbol.data_type == VariableType.INT:
                        code_creator.set('[$FP+{}]'.format(symbol.stack_index), 0)
                    if symbol.data_type == VariableType.STRING:
                        code_creator.set('[$FP+{}]'.format(symbol.stack_index), "\"\"")

            # clear return register
            if self.return_type == VariableType.INT:
                code_creator.set("$RETVAL", "\"\"")
            else:
                code_creator.set("$RETVAL", 0)

            #built in preceded call of parent constructor
            if self._dclass and self._dclass.parent and self._dclass.name == self.name:
                code_creator.addi("$SP", "$SP", 1)
                code_creator.set("[$SP]", "$FP")
                code_creator.addi("$SP", "$SP", 1)
                code_creator.call("[$SP]", '{}::{}'.format(self._dclass.parent.name, self._dclass.parent.name))
                code_creator.subi("$SP", "$SP", 1)
                code_creator.set("$FP", "[$SP]")
                code_creator.subi("$SP", "$SP", 1)

            for statement in self._codeblock:
                statement.code(code_creator)

        if self._dclass:
            #set back self pointer
            code_creator.set("$SELF", "[$FP+1]")

        # set stack pointer back
        code_creator.set("$SP", "$FP")
        #restor PC
        code_creator.return_i("[$FP]")

    def _code_built_in_functions(self, code_creator: CodeCreator):
        if self.name == 'print':
            # implemented dynamic - in  compiler/statement.py
            pass
        elif self.name == 'readInt':
            code_creator.readi("$RETVAL")
        elif self.name == 'readString':
            code_creator.reads("$RETVAL")
        elif self.name == 'length':
            self.parameters[0].stack_index = -1
            self.parameters[0].code(code_creator)
            code_creator.get_size("$RETVAL", '$ACC1')
        elif self.name == 'toString':
            code_creator.set('$RETVAL', self._dclass.cls_index)
            pass
        elif self.name == 'getClass':
            code_creator.set('$RETVAL', '\"Object\"')
        elif self.name == 'Object':
            code_creator.set("$RETVAL", "\"\"")
        elif self.name == 'subStr':
            code_creator.addi("$SP", "$SP", 2)
            #old string counter
            #new string counter
            #allocate
            code_creator.create("$ACC2", "[$FP-1]")     # string target
            code_creator.set("[$SP-1]", "[$FP-2]")      # index old string
            code_creator.set('[$SP]', 0)                # index new string
            # [$FP-2] - start index
            # [$FP-1] - len
            # [$FP-3] - source string
            code_creator.lti("$ACC3", "[$FP-2]", 0)
            code_creator.jumpnz('substr_return_empty_string', "$ACC3")
            code_creator.lti("$ACC3", "[$FP-1]", 0)
            code_creator.jumpnz('substr_return_empty_string', "$ACC3")
            code_creator.get_size("$ACC3", '[$FP-3]')
            code_creator.subi("$ACC3","$ACC3",1)
            code_creator.gti("$ACC3", "[$FP-1]", "$ACC3")
            code_creator.jumpnz('substr_return_empty_string', "$ACC3")
            code_creator.get_size("$ACC3", '[$FP-3]')
            code_creator.subi("$ACC3", "$ACC3", 1)
            code_creator.gti("$ACC3", "[$FP-2]", "$ACC3")
            code_creator.jumpnz('substr_return_empty_string', "$ACC3")
            #copy cycle
            code_creator.label('substr_cycle_condition_start')
            code_creator.subi("$ACC3", "[$FP-1]",1)
            code_creator.gti("$ACC3", "[$SP]", "$ACC3")
            code_creator.jumpnz('substr_return_part', "$ACC3")
            code_creator.get_word("$ACC3", "[$FP-3]", "[$SP-1]")
            code_creator.set_word("$ACC2", "[$SP]", "$ACC3")
            #change index
            code_creator.addi("$ACC3", "[$SP-1]", 1)
            code_creator.set("[$SP-1]","$ACC3")
            code_creator.addi("$ACC3", "[$SP]", 1)
            code_creator.set("[$SP]", "$ACC3")
            code_creator.jump('substr_cycle_condition_start')
            code_creator.label('substr_return_empty_string')
            code_creator.set("$ACC2", "\"\"")
            code_creator.label('substr_return_part')
            code_creator.set('$RETVAL', '$ACC2')

            code_creator.subi("$SP", "$SP", 2)


    def check_signature(self, ctx, sym_table, callee=None) -> None:
        """Check the arguments of a function call."""
        # called with arguments
        args = []
        for exp in ctx.expr():
            args.append(Expression(exp, sym_table, self if callee is None else callee))

        # print function has a variable number of arguments, but at least one has to be specified
        if self.name == 'print':
            if len(args) == 0:
                raise SemanticErrorException('at least one arguments has to be specified for the function \'print\'', ctx.start.line, ctx.start.column)
            for a in args:
                if a.value_type not in [VariableType.INT, VariableType.STRING]:
                    raise SemanticErrorException('only integers and strings can be printed', ctx.start.line, ctx.start.column, code=CompilerError.TYPE_ERROR)
        else:
            if len(args) != len(self.parameters):
                raise SemanticErrorException('invalid number of arguments of function \'{}\''.format(self.name), ctx.start.line, ctx.start.column)
            for a, p in zip(args, self.parameters):
                if a.value_type != p.data_type:
                    raise SemanticErrorException('invalid data type of an argument of function \'{}\''.format(self.name), ctx.start.line, ctx.start.column, code=CompilerError.TYPE_ERROR)

    def has_same_signature(self, func) -> bool:
        """Check whether this function has the same signature as the parameter func."""
        # check return type
        if self.return_type != func.return_type:
            return False
        # check number of parameters
        if len(self.parameters) != len(func.parameters):
            return False
        # check data type of parameters
        for x, y in zip(self.parameters, func.parameters):
            if x.data_type != y.data_type:
                return False

        return True

    @property
    def def_class(self) -> 'Class':
        """Return the class where the function is defined."""
        return self._dclass

    @property
    def return_type(self) -> VariableType:
        """Return the function return type."""
        return self._rtype

    @property
    def return_type_class(self) -> str:
        """Return the function return type class."""
        return self._rtype_cls

    @property
    def parameters(self) -> [Variable]:
        """Return the function parameters."""
        return self._params
