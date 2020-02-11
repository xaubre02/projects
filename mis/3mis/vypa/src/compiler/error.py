#!/usr/bin/env python3
"""
- Project: VYPa19 language compiler
- Author: Tomas Aubrecht (xaubre02), Jan Kotras (xkotra01)
- Date:   2019-10-25
- Desc:   compiler.
"""

import enum
from generated import VYPa19Lexer
from antlr4.error.ErrorListener import ErrorListener

# 'import *' definition (exclude VYPa19Lexer and ErrorListener)
__all__ = [
    'CompilerError',
    'CompilationException',
    'InternalErrorException',
    'SemanticErrorException',
    'LexicalErrorListener',
    'SyntacticErrorListener'
]


class CompilerError(enum.Enum):
    """Compiler return error codes."""

    LEXICAL_ERROR = 11
    SYNTACTIC_ERROR = 12
    TYPE_ERROR = 13
    SEMANTIC_ERROR = 14
    CODE_GENERATION_ERROR = 15
    INTERNAL_ERROR = 19


class CompilationException(Exception):
    """Base exception raised during a compilation."""

    def __init__(self, msg, code):
        """Initialize the exception."""
        super().__init__(msg)
        self._errc = code  # error code

    @property
    def code(self) -> enum:
        """Return the error code of the exception."""
        return self._errc


class InternalErrorException(CompilationException):
    """Internal error exception."""

    def __init__(self, msg):
        """Initialize the internal exception."""
        super().__init__('Internal error: {}'.format(msg), CompilerError.INTERNAL_ERROR)


class LexicalErrorException(CompilationException):
    """Lexical error exception."""

    def __init__(self, msg, line, column):
        """Initialize the lexical exception."""
        super().__init__('Lexical error on line {}, column {}: {}'.format(line, column+1, msg), CompilerError.LEXICAL_ERROR)


class SyntacticErrorException(CompilationException):
    """Syntactic error exception."""

    def __init__(self, msg, line, column):
        """Initialize the syntactic exception."""
        super().__init__('Syntactic error on line {}, column {}: {}'.format(line, column+1, msg), CompilerError.SYNTACTIC_ERROR)


class SemanticErrorException(CompilationException):
    """Semantic error exception."""
    def __init__(self, msg, line=None, column=None, code=CompilerError.SEMANTIC_ERROR):
        """Initialize the semantic exception."""
        if line is not None and column is not None:
            msg = 'on line {}, column {}: {}'.format(line, column+1, msg)
        super().__init__('Semantic error: {}'.format(msg), code)


class LexicalErrorListener(ErrorListener):
    """Lexical error listener class."""

    def syntaxError(self, recognizer, offending_symbol, line, column, msg, e):
        """Overrides syntaxError() function."""
        raise LexicalErrorException(msg, line, column)


class SyntacticErrorListener(ErrorListener):
    """Syntactic error listener class."""

    def syntaxError(self, recognizer, offending_symbol, line, column, msg, e):
        """Overrides syntaxError() function."""
        symbol = offending_symbol.text
        ts = recognizer.getInputStream()  # token stream

        # additional lexical errors
        if symbol.isnumeric() and ts.get(ts.index-1).text == '0':
            raise LexicalErrorException('numbers cannot start with 0', line, column)
        elif symbol == '.' and ts.get(ts.index-1).type == VYPa19Lexer.INTEGER:
            raise LexicalErrorException('real numbers are not allowed', line, column)
        elif symbol == '/*':
            raise LexicalErrorException('unclosed block comment', line, column)
        elif symbol == '*/':
            raise LexicalErrorException('missing block comment opening', line, column)

        # syntactic errors
        raise SyntacticErrorException('unexpected \'{}\''.format(symbol), line, column)
