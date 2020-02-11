#!/usr/bin/env python3
"""
- Project: VYPa19 language compiler
- Author: Tomas Aubrecht (xaubre02), Jan Kotras (xkotra01)
- Date:   2019-10-25
- Desc:   compiler.
"""

import os
import sys
from antlr4 import CommonTokenStream, FileStream

from compiler.code_generator import CodeGenerator
from generated import VYPa19Lexer, VYPa19Parser
from compiler import *


class Compiler:
    """VYPa19 Compiler."""

    def __init__(self):
        """Class constructor."""
        self.error_code = 0  # no error
        self.input = None    # input stream
        self.outfile = None  # output file
        self.scanner = None  # input scanner
        self.parser = None   # syntax parser
        self.silent = False   # is silent mode activated
        self.verbose = False
        self.stack_size = 65535
        self.regs_num = 8

        # global symbol table
        self.sym_table = SymbolTable()

    def setup_compilation(self, silent: bool, verbose: bool, stack_size: int, regs_num: int):
        self.silent = silent
        self.verbose = verbose
        self.stack_size = stack_size
        self.regs_num = regs_num

    def _error(self, msg, code, silent=False) -> None:
        """Formatted error message."""
        if not silent:
            print(msg, file=sys.stderr)
        self.error_code = code.value

    def _check_input(self, infile, outfile) -> bool:
        """Check input arguments of the compiler."""
        # check whether the input file exists
        if not os.path.exists(infile):
            self._error('Internal error: specified input file does not exist', CompilerError.INTERNAL_ERROR)
            return False
        # check whether the input file is actually a file
        if not os.path.isfile(infile):
            self._error('Internal error: specified input is not a file', CompilerError.INTERNAL_ERROR)
            return False
        # possible error: e.g. if trying to open a pdf file
        try:
            self.input = FileStream(infile, encoding='utf-8')
        except Exception as e:
            self._error('Internal error: {}'.format(str(e)), CompilerError.INTERNAL_ERROR)
            return False

        # default output file
        if outfile is None:
            self.outfile = os.path.join(os.getcwd(), 'out.vc')
        else:
            self.outfile = os.path.abspath(outfile)

        # check whether the file is accessible
        if os.path.exists(self.outfile):
            try:
                os.rename(self.outfile, self.outfile)
            except OSError:
                self._error('Internal error: specified output file is inaccessible', CompilerError.INTERNAL_ERROR)
                return False

        # everything is OK
        return True

    def check_tokens(self) -> None:
        """Perform lexical analysis."""
        self.scanner = VYPa19Lexer(self.input)
        self.scanner.removeErrorListeners()
        self.scanner.addErrorListener(LexicalErrorListener())
        self.scanner.getAllTokens()
        self.scanner.reset()

    def check_syntax(self) -> None:
        """Perform syntactic analysis."""
        self.parser = VYPa19Parser(CommonTokenStream(self.scanner))
        self.parser.removeErrorListeners()
        self.parser.addErrorListener(SyntacticErrorListener())
        self.parser.program()
        self.parser.reset()

    def check_semantics(self) -> None:
        """Perform semantic analysis and construct the program."""
        program = self.parser.program()
        for definition in program.definition():
            # function definition
            if definition.function_definition() is not None:
                # store the function
                self.sym_table.store(Function(ctx=definition.function_definition(), sym_table=self.sym_table))
            # class definition
            elif definition.class_definition() is not None:
                # store the class
                self.sym_table.store(Class(ctx=definition.class_definition(), sym_table=self.sym_table))

        # second pass
        for e in self.sym_table.symbols.values():  # entity is a function or a class
            if not e.built_in and e.type != EntityType.FUNCTION:
                e.init()

        # third pass
        for e in self.sym_table.symbols.values():  # entity is a function or a class
            if not e.built_in and e.type == EntityType.FUNCTION:
                e.init()

        # check the main function
        main = self.sym_table.get('main')
        # check if the main is a function
        if not isinstance(main, Function):
            raise SemanticErrorException('\'main\' is not a function')
        # check the signature
        if main.return_type != VariableType.VOID or len(main.parameters) > 0:
            raise SemanticErrorException('function \'main\' has invalid type signature')


    def generate_code(self) -> None:
        """Generate a code in the target language - VYPcode."""

        code_generator = CodeGenerator(self.sym_table)
        code_generator.set_silent_mode(self.silent)
        generated_code = code_generator.generate()

        # write code to file
        try:
            dirname = os.path.dirname(self.outfile)
            if not os.path.exists(dirname):
                os.makedirs(dirname)
            with open(self.outfile, encoding='utf-8', mode='w+') as output_file:
                output_file.write(generated_code)
        except IOError as e:
            raise InternalErrorException(str(e))

    def compile(self, infile, outfile=None) -> int:
        """Compile a given source code to the specified file."""
        if not self._check_input(infile, outfile):
            return self.error_code

        try:
            # lexical analysis
            self.check_tokens()
            # syntactic analysis
            self.check_syntax()
            # initialize built-in functions and classes
            self.init_built_in()
            # semantics analysis
            self.check_semantics()
            # code generation
            self.generate_code()
        except CompilationException as e:
            self._error(str(e), e.code, self.silent)

        return self.error_code

    def init_built_in(self) -> None:
        """Initialize all built-in functions and classes."""
        # 'print' function
        self.sym_table.store(Function(name='print', rtype=VariableType.VOID, built_in=True))  # parameters are processed later in check_signature()
        # 'readInt' function
        self.sym_table.store(Function(name='readInt', rtype=VariableType.INT, built_in=True))
        # 'readString' function
        self.sym_table.store(Function(name='readString', rtype=VariableType.STRING, built_in=True))
        # 'length' function
        params = [Variable(name='s', dtype=VariableType.STRING)]
        self.sym_table.store(Function(name='length', rtype=VariableType.INT, params=params, built_in=True))
        # 'subStr' function
        params = [Variable(name='s', dtype=VariableType.STRING),
                  Variable(name='i', dtype=VariableType.INT),
                  Variable(name='n', dtype=VariableType.INT)]
        self.sym_table.store(Function(name='subStr', rtype=VariableType.STRING, params=params, built_in=True))

        # 'Object' class
        f1 = Function(name='toString', rtype=VariableType.STRING, built_in=True)
        f2 = Function(name='getClass', rtype=VariableType.STRING, built_in=True)
        f3 = Function(name='Object', rtype=VariableType.VOID, built_in=True)
        methods = [f1,f2, f3]
        sym_table = SymbolTable(self.sym_table)
        for method in methods:
            sym_table.store(method)
        my_class = Class(sym_table=sym_table, name='Object', methods=methods, built_in=True)
        self.sym_table.store(my_class)
        for method in methods:
            method._dclass = my_class
            method.sym_table= sym_table
