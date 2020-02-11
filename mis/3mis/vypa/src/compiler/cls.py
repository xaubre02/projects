#!/usr/bin/env python3
"""
- Project: VYPa19 language compiler
- Author: Tomas Aubrecht (xaubre02), Jan Kotras (xkotra01)
- Date:   2019-10-25
- Desc:   Class module.
"""

from compiler import *
from compiler.code_creator import CodeCreator


class Class(Entity):
    """Class representing a VYPa19 language class."""

    def __init__(self, ctx=None, sym_table=None, name=None, methods=None, built_in=False):
        """Initialize the class."""
        # only the context and symbol table have to be specified or name, methods and symbol table
        if (ctx is not None and sym_table is not None and (name is not None or methods is not None)) or \
           (ctx is None and (sym_table is None or name is None or methods is None)):
            raise InternalErrorException('invalid Class initialization arguments')

        self.chunk_id = 0
        self.cls_index = 0
        self.method_index_counter = 0
        self.attributes_index_counter = 0
        # built-in class 'Object'
        if built_in:
            super().__init__(EntityType.CLASS, name=name)
            self._methods = [] if methods is None else methods
            self._my_methods = []
            self._vars = []
            self._parent = None
            self.sym_table = sym_table
            self._initialized = True
            for method in self._methods:
                method.method_index = self.method_index_counter
                self.method_index_counter += 1
        # use context to initialize the class
        else:
            super().__init__(EntityType.CLASS, ctx=ctx)
            self._my_methods = []
            self._methods = []
            self._vars = []
            # create a body for this class (list of statements)
            self._body = ctx.class_body()
            # parent class
            self._parent = ctx.ID()[1].symbol
            self.sym_table = sym_table
            self._initialized = False

        self._bin = built_in

    @property
    def built_in(self) -> bool:
        """Return True if the class is built-in, False otherwise."""
        return self._bin

    @property
    def initialized(self) -> bool:
        """Return True if this class has been initialized, False otherwise."""
        return self._initialized

    @property
    def members(self) -> SymbolTable:
        """Return the symbol table of this class: class methods and variables."""
        return self.sym_table

    # @property
    # def methods(self) -> [Function]:
    #     """Return the list of methods of this class."""
    #     return self._methods

    @property
    def all_methods(self) -> [Function]:
        """Return the list of methods of this class and its ancestors."""
        # methods of the parent
        pm = [] if self.parent is None else self.parent.all_methods

        # methods of this class
        methods = self._methods + self._my_methods
        methods_names = [elem.name for elem in self._methods + self._my_methods]
        to_append_methods = []
        # methods from ancestors
        for m in pm:
            # this method is not defined in this class
            if m.name not in methods_names:
                to_append_methods.append(m)


        return to_append_methods + methods

    @property
    def variables(self) -> [Variable]:
        """Return the list of variables of this class."""
        return self._vars

    @property
    def parent(self) -> 'Class':
        """Return the parent class."""
        return self._parent

    def init(self) -> None:
        """Initialize the class (second pass)."""
        # already initialized
        if self.initialized:
            return

        # parent class
        if not self.sym_table.is_defined(self._parent.text):
            raise SemanticErrorException('parent class \'{}\' is not defined'.format(self._parent.text), self._parent.line, self._parent.column)
        self._parent = self.sym_table.get(self._parent.text)
        # set the initialization flag
        self._initialized = True  # has to be here to catch the cyclic initialization
        # initialize the parent
        if not self.parent.initialized:
            self.parent.init()

        # cyclic definiton of the class hiearchy
        if self.parent.is_subclass_of(self.name):
            raise SemanticErrorException('cyclic class definiton: \'{}\' and \'{}\''.format(self.name, self.parent.name), self.line, self.column)

        # symbol table for this class
        self.sym_table = SymbolTable(self.parent.members)

        var_def = self._body.variable_definition()  # variables definitions
        fun_def = self._body.function_definition()  # functions definitions

        if var_def is not None:
            for vd in var_def:
                vtype = VariableType.recognize(vd)       # variable type
                vcls = VariableType.recognize_class(vd)  # variable class
                # multiple variables declared at once
                for name in vd.ID():
                    name = name.symbol
                    var = Variable(name=name.text,
                                   dtype=vtype,
                                   vclass=vcls,
                                   line=name.line,
                                   column=name.column,
                                   class_owner=self)
                    self._vars.append(var)
                    self.sym_table.store(var)

        if fun_def is not None:
            to_init = []
            # declare methods
            for fd in fun_def:
                fun = Function(ctx=fd, sym_table=self.sym_table, def_class=self)
                self._my_methods.append(fun)
                self.sym_table.store(fun)
                to_init.append(fun)

            # initialize methods
            for m in to_init:
                m.init()


        #indexate attributtes and methods
        for method in self.all_methods:
            method.method_index = self.method_index_counter
            self.method_index_counter += 1
        for attribute in self._vars:
            attribute.attribute_index = self.attributes_index_counter
            self.attributes_index_counter += 1

    def is_subclass_of(self, name) -> bool:
        """Return True if this class is a subclass of the 'name' class."""
        return self.parent.name == name if self.parent is not None else False

    def get_member(self, name) -> Entity:
        """Get a class member by a name."""
        if self.sym_table.is_defined(name):
            return self.sym_table.get(name)
        else:
            raise SemanticErrorException('class \'{}\' has no member \'{}\''.format(self.name, name))

    def code(self):
        """Generate an assembly code."""
        code_creator = CodeCreator.code_creator()
        code_creator.comment("")
        code_creator.comment("")
        code_creator.comment("Class: {}".format(self.name))
        code_creator.comment("-------------------------------")
        if self.name == "Object":
            for method in self._methods:
                method.code()
        else:
            for method in self._my_methods:
                method.code()
            if self.name not in [i.name for i in self._my_methods]:
                code_creator.comment("Method: {}::{}".format(self.name,self.name))
                code_creator.comment("-------------------------------")
                code_creator.label('{}::{}'.format(self.name,self.name))
                code_creator.set("$FP", "$SP")
                if self.parent:
                    code_creator.addi("$SP", "$SP", 1)
                    code_creator.set("[$SP]", "$FP")
                    code_creator.addi("$SP", "$SP", 1)
                    code_creator.call("[$SP]", '{}::{}'.format(self.parent.name, self.parent.name))
                    code_creator.subi("$SP", "$SP", 1)
                    code_creator.set("$FP", "[$SP]")
                    code_creator.subi("$SP", "$SP", 1)
                code_creator.set("$SP", "$FP")
                code_creator.return_i("[$FP]")

    def code_virtual_table(self, code_creator: CodeCreator):
        code_creator.create("$ACC3", len(self.all_methods))
        for method in self.all_methods:
            code_creator.set_word("$ACC3", method.method_index, "\"" + method.class_context_name + "\"")
        code_creator.set_word("$VT", self.cls_index, "$ACC3")

    def code_new_obj(self, code_creator: CodeCreator):
        code_creator.create("$ACC1", self.attributes_index_counter)
        code_creator.get_word("$ACC2", "$VT", self.cls_index)
        code_creator.get_size("$ACC3", "$ACC2")
        code_creator.addi("$ACC3", "$ACC3", 1)
        code_creator.resize("$ACC2", "$ACC3")
        code_creator.subi("$ACC3", "$ACC3", 1)
        code_creator.set_word("$ACC2", "$ACC3", "$ACC1")
        # store FP
        code_creator.addi("$SP", "$SP", 1)
        code_creator.set("[$SP]", "$FP")
        # set pointer
        code_creator.addi("$SP", "$SP", 1)
        code_creator.set("[$SP]", "$ACC1")
        # store SP
        code_creator.addi("$SP", "$SP", 1)
        code_creator.set("[$SP]", 0)

        # call function
        code_creator.addi("$SP", "$SP", 1)
        code_creator.call("[$SP]", self.name + "::" + self.name)
        code_creator.subi("$SP", "$SP", 1)

        # clear pointer
        code_creator.subi("$SP", "$SP", 1)

        # clear arguments
        code_creator.subi("$SP", "$SP", 1)

        # restore FP
        code_creator.set("$FP", "[$SP]")
        code_creator.subi("$SP", "$SP", 1)
        code_creator.set("$ACC1", "$ACC3")
