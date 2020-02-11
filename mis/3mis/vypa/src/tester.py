#!/usr/bin/env python3
"""
- Project: VYPa19 language compiler
- Author: Tomas Aubrecht (xaubre02), Jan Kotras (xkotra01)
- Date:   2019-10-25
- Desc:   compiler.
"""

import unittest

import os
from antlr4 import *
from src.generated.VYPa19Lexer import VYPa19Lexer
from src.generated.VYPa19Parser import VYPa19Parser
from antlr4.error.ErrorListener import ErrorListener
from compiler import Compiler, CompilerError


class VYpa19Exception(Exception):
    """Lexer Exception."""
    pass


class VYPa19ErrorListener(ErrorListener):
    """Lexer Error Listener Class."""

    def syntaxError(self, recognizer, offending_symbol, line, column, msg, e):
        """Overrides syntaxError() function."""
        raise VYpa19Exception()


class TestVYPa19Lexer(unittest.TestCase):
    """Unit tests for the VYPa19 lexer."""

    @staticmethod
    def scan(source) -> [Token]:
        """Scan the given source and tokenize it."""
        lexer = VYPa19Lexer(InputStream(source))
        lexer.removeErrorListeners()
        lexer.addErrorListener(VYPa19ErrorListener())
        return lexer.getAllTokens()

    def test_block_comments(self) -> None:
        """Test valid block comments."""
        block_comments = [
            '/*something*/',
            '/*something on\n another line*/',
            '/*\nmultiple\nline\ncomment*/',
            '/* there is a line comment inside this block comment // right here */'
        ]

        for comment in block_comments:
            tokens = self.scan(comment)
            self.assertEqual(len(tokens), 0)

    def test_block_comments_invalid(self) -> None:
        """Test ivalid block comments."""
        tokens = self.scan('/* unclosed block comment')
        self.assertEqual(len(tokens), 4)
        self.assertEqual(tokens[0].type, VYPa19Lexer.BC_START)

        tokens = self.scan('*/')
        self.assertEqual(len(tokens), 1)
        self.assertEqual(tokens[0].type, VYPa19Lexer.BC_END)

        tokens = self.scan('/* there is a block comment inside this block comment /* right here */ */')
        self.assertEqual(len(tokens), 1)
        self.assertEqual(tokens[0].type, VYPa19Lexer.BC_END)

    def test_line_comments(self) -> None:
        """Test valid line comments."""
        line_comments = [
            '// something',
            '// something\r',
            '// something\n',
            '// something\n// new line comment',
            '//something /* block comment */',
            '//something /* bad block comment',
            '//something  bad block comment */'
        ]

        for comment in line_comments:
            tokens = self.scan(comment)
            self.assertEqual(len(tokens), 0)

    def test_keywords(self) -> None:
        """Test valid keywords."""
        keywords = [
            ['class', VYPa19Lexer.CLASS],
            ['else', VYPa19Lexer.ELSE],
            ['if', VYPa19Lexer.IF],
            ['int', VYPa19Lexer.INT],
            ['new', VYPa19Lexer.NEW],
            ['return', VYPa19Lexer.RETURN],
            ['string', VYPa19Lexer.STRING],
            ['super', VYPa19Lexer.SUPER],
            ['this', VYPa19Lexer.THIS],
            ['void', VYPa19Lexer.VOID],
            ['while', VYPa19Lexer.WHILE]
        ]

        for keyword in keywords:
            tokens = self.scan(keyword[0])
            self.assertEqual(len(tokens), 1)
            self.assertEqual(tokens[0].type, keyword[1])

    def test_strings(self) -> None:
        """Test valid strings."""
        strings = [
            r'""',
            r'"  "',
            '"something"',
            r'"some\x12345Athing"',
            r'"escape seq.: \n \t \\ \" \x00002F \xFFFFFF"',
            '"something"//this is a string'
        ]

        for string in strings:
            tokens = self.scan(string)
            self.assertEqual(len(tokens), 1)
            self.assertEqual(tokens[0].type, VYPa19Lexer.STRING_LITERAL)

    def test_strings_invalid(self) -> None:
        """Test invalid strings."""
        strings = [
            '"\r"',
            '"\n"',
            '"\t"',
            '"\""',
            '"\u0000"',
            '"\u000B"',
            '"\u001F"',
            '"prefix\u0022and suffix"',
            r'"\x12345"',
            r'"\x12345g"',
            r'"\"',
            r'"return \r wow"'
        ]

        for string in strings:
            with self.assertRaises(VYpa19Exception):
                self.scan(string)

    def test_integers(self) -> None:
        """Test valid integerss."""
        integers = [
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
            '10', '100', '420', '9999999',
            '123456789101112131415161718192021222324252627282930'
        ]

        for integer in integers:
            tokens = self.scan(integer)
            self.assertEqual(len(tokens), 1)
            self.assertEqual(tokens[0].type, VYPa19Lexer.INTEGER)

    def test_integers_invalid(self) -> None:
        """Test invalid integers."""
        integers = [
            ['00', 2],   # Tokens: zero and zero
            ['09', 2],   # Tokens: zero and nine
            ['.5', 2],   # Tokens: dot and five
            ['1.5', 3],  # Tokens: one, dot and five
            ['1.f', 3]   # Tokens: one, dot and ID
        ]

        for integer in integers:
            tokens = self.scan(integer[0])
            self.assertEqual(len(tokens), integer[1])

    def test_IDs(self) -> None:
        """Test valid IDs."""
        ids = [
            'x',
            '_',
            'x_',
            '_x',
            'x1',
            'x_1',
            'X',
            'While',
            'eLse',
            'IF',
            'this2',
            'something_long42'
        ]

        for i in ids:
            tokens = self.scan(i)
            self.assertEqual(len(tokens), 1)
            self.assertEqual(tokens[0].type, VYPa19Lexer.ID)

    def test_IDs_invalid(self) -> None:
        """Test invalid IDs."""
        ids = [
            '^',
            '&',
            '~',
            '"',
            '\'',
            '@nd',
            '#blabla',
            '$omething',
            'midd%e',
            'příklad'
        ]

        for i in ids:
            with self.assertRaises(VYpa19Exception, msg='failed ID: \'{}\''.format(i)):
                self.scan(i)


class TestVYPa19Parser(unittest.TestCase):
    """Unit tests for the VYPa19 parser."""

    @staticmethod
    def setup_parser(source) -> VYPa19Parser:
        """Scan the given source and tokenize it."""
        # setup lexer
        lexer = VYPa19Lexer(InputStream(source))
        lexer.removeErrorListeners()
        lexer.addErrorListener(VYPa19ErrorListener())
        # setup parser
        parser = VYPa19Parser(CommonTokenStream(lexer))
        parser.removeErrorListeners()
        parser.addErrorListener(VYPa19ErrorListener())
        return parser

    def test_data_types(self) -> None:
        """Test valid data types."""
        data_types = [
            'int',
            'string',
            'my_type',
            '_my_random_type_42',
        ]

        for dt in data_types:
            try:
                self.setup_parser(dt).data_type()
            except VYpa19Exception:
                self.fail()

    def test_param_lists(self) -> None:
        """Test valid parameter lists."""
        param_lists = [
            'void',
            'int param1',
            'int param1, int param2',
            'int param1, int param2, string param3, my_type param4'
        ]

        for pl in param_lists:
            try:
                self.setup_parser(pl).param_list()
            except VYpa19Exception:
                self.fail('failed param list: \'{}\''.format(pl))

    def test_parameters_invalid(self) -> None:
        """Test invalid parameters."""
        params = [
            '123',
            'something',
            'while param',
            'void param',
            'int 666',
            '123 param'
        ]

        for pr in params:
            with self.assertRaises(VYpa19Exception, msg='failed param_list: \'{}\''.format(pr)):
                self.setup_parser(pr).parameter()

    def test_program(self) -> None:
        """Test a valid(syntax) program."""
        programs = [
            '',
            'int my_function(){}',
            'int my_function(){}\nmy_type my_function(int param1, string param2){\nint a; a = 5 + 11;}',
            'class MyClass : Object {}'
        ]

        for program in programs:
            try:
                self.setup_parser(program).program()
            except VYpa19Exception:
                self.fail(msg='failed program: \'{}\''.format(program))


class TestVYPa19Semantics(unittest.TestCase):
    """Test VYPa19 Semantic analyzer."""

    @ staticmethod
    def compile_file(filename, silent=True) -> int:
        """Get a file from the 'tests' directory."""
        tester_path = os.path.dirname(os.path.realpath(__file__))
        tests_dir = os.path.abspath(os.path.join(tester_path, '../tests'))
        # list tests directory
        file = None
        for item in os.listdir(tests_dir):
            if item == filename:
                file = os.path.join(tests_dir, item)
                break
        comp = Compiler()
        comp.setup_compilation(True, False, 0, 0)
        return comp.compile(file)

    def test_missing_main(self) -> None:
        """Test a missing main function."""
        files = [
            'main_missing1.cpp',
            'main_missing2.cpp',
            'main_missing3.cpp'
        ]
        for f in files:
            self.assertEqual(self.compile_file(f), CompilerError.SEMANTIC_ERROR.value)

    def test_invalid_main(self) -> None:
        """Test an invalid main function signature."""
        files = [
            'main_invalid_signature1.cpp',
            'main_invalid_signature2.cpp',
            'main_invalid_signature3.cpp'
        ]
        for f in files:
            self.assertEqual(self.compile_file(f), CompilerError.SEMANTIC_ERROR.value)

    def test_function_redefinition(self) -> None:
        """Test a function redefinition."""
        files = [
            'function_redefinition1.cpp',
            'function_redefinition2.cpp'
        ]
        for f in files:
            self.assertEqual(self.compile_file(f), CompilerError.SEMANTIC_ERROR.value)

    def test_function_parameter_redefinition(self) -> None:
        """Test a function parameter redefinition."""
        files = [
            'parameter_redefinition1.cpp',
            'parameter_redefinition2.cpp',
            'parameter_redefinition3.cpp',
            'parameter_redefinition4.cpp',
            'parameter_redefinition5.cpp',
            'parameter_redefinition6.cpp',
            'parameter_redefinition7.cpp',
            'parameter_redefinition8.cpp'
        ]
        for f in files:
            self.assertEqual(self.compile_file(f), CompilerError.SEMANTIC_ERROR.value)

    def test_invalid_assignment(self) -> None:
        """Test an invalid assignments."""
        files = [
            'invalid_assignment1.cpp',
            'invalid_assignment2.cpp',
            'invalid_assignment3.cpp',
            'invalid_assignment4.cpp',
            'invalid_assignment5.cpp',
            'invalid_assignment6.cpp',
            'invalid_assignment7.cpp',
            'invalid_assignment8.cpp',
            'invalid_assignment9.cpp'
        ]
        for f in files:
            self.assertEqual(self.compile_file(f), CompilerError.TYPE_ERROR.value)

    def test_undefined_symbol(self) -> None:
        """Test an undefined symbols."""
        files = [
            'undefined_symbol1.cpp',
            'undefined_symbol2.cpp',
            'undefined_symbol3.cpp',
            'undefined_symbol4.cpp',
            'undefined_symbol5.cpp'
        ]
        for f in files:
            self.assertEqual(self.compile_file(f), CompilerError.SEMANTIC_ERROR.value)

    def test_invalid_condition(self) -> None:
        """Test an invalid condition."""
        files = [
            'invalid_condition1.cpp',
            'invalid_condition2.cpp',
            'invalid_condition3.cpp',
            'invalid_condition4.cpp'
        ]
        for f in files:
            self.assertEqual(self.compile_file(f), CompilerError.TYPE_ERROR.value)

    def test_invalid_iteration(self) -> None:
        """Test an invalid iteration."""
        files = [
            'invalid_iteration1.cpp',
            'invalid_iteration2.cpp',
            'invalid_iteration3.cpp',
            'invalid_iteration4.cpp'
        ]
        for f in files:
            self.assertEqual(self.compile_file(f), CompilerError.TYPE_ERROR.value)

    def test_invalid_function_call(self) -> None:
        """Test an invalid function call."""
        files = [
            'invalid_function_call1.cpp',
            'invalid_function_call2.cpp',
            'invalid_function_call4.cpp',
            'invalid_function_call5.cpp',
            'invalid_function_call8.cpp',
            'invalid_function_call9.cpp'
        ]
        for f in files:
            self.assertEqual(self.compile_file(f), CompilerError.SEMANTIC_ERROR.value)

    def test_invalid_function_call_type_error(self) -> None:
        """Test an invalid function call."""
        files = [
            'invalid_function_call3.cpp',
            'invalid_function_call6.cpp',
            'invalid_function_call7.cpp'
        ]
        for f in files:
            self.assertEqual(self.compile_file(f), CompilerError.TYPE_ERROR.value)

    def test_invalid_return(self) -> None:
        """Test an invalid return statement."""
        files = [
            'invalid_return1.cpp',
            'invalid_return2.cpp',
            'invalid_return3.cpp',
            'invalid_return4.cpp',
            'invalid_return5.cpp',
            'invalid_return6.cpp',
            'invalid_return7.cpp',
            'invalid_return8.cpp',
            'invalid_return9.cpp'
        ]
        for f in files:
            self.assertEqual(self.compile_file(f), CompilerError.TYPE_ERROR.value)

    def test_invalid_casting(self) -> None:
        """Test an invalid casting."""
        files = [
            'invalid_casting1.cpp',
            'invalid_casting2.cpp',
            'invalid_casting3.cpp'
        ]
        for f in files:
            self.assertEqual(self.compile_file(f), CompilerError.TYPE_ERROR.value)

    def test_invalid_negation(self) -> None:
        """Test an invalid negation."""
        files = [
            'invalid_negation1.cpp',
            'invalid_negation2.cpp',
            'invalid_negation3.cpp'
        ]
        for f in files:
            self.assertEqual(self.compile_file(f), CompilerError.TYPE_ERROR.value)

    def test_invalid_arithmetic_operation(self) -> None:
        """Test an invalid arithmetic operation."""
        files = [
            'invalid_arithmetic_operation1.cpp',
            'invalid_arithmetic_operation2.cpp',
            'invalid_arithmetic_operation3.cpp'
        ]
        for f in files:
            self.assertEqual(self.compile_file(f), CompilerError.SEMANTIC_ERROR.value)

    def test_invalid_relation(self) -> None:
        """Test an invalid relation."""
        files = [
            'invalid_relation1.cpp',
            'invalid_relation2.cpp',
            'invalid_relation3.cpp'
        ]
        for f in files:
            self.assertEqual(self.compile_file(f), CompilerError.SEMANTIC_ERROR.value)

    def test_invalid_logical_operation(self) -> None:
        """Test an invalid logical operation."""
        files = [
            'invalid_logical_operation1.cpp',
            'invalid_logical_operation2.cpp',
            'invalid_logical_operation3.cpp',
            'invalid_logical_operation4.cpp'
        ]
        for f in files:
            self.assertEqual(self.compile_file(f), CompilerError.SEMANTIC_ERROR.value)
