#!/usr/bin/env python3
"""
- Project: VYPa19 language compiler
- Author: Tomas Aubrecht (xaubre02)
- Date:   2019-10-20
- Desc:   Symbol table module.
"""

from compiler import *


class SymbolTable:
    """Class representing symbol table."""

    def __init__(self, parent=None):
        """Initialize the symbol table."""
        self._symbols = {}    # stored symbols
        self.parent = parent  # parent symbol table

    @property
    def symbols(self) -> [str]:
        """Return all defined symbols."""
        return self._symbols

    def clear(self) -> None:
        """Clear the symbol table."""
        del self._symbols
        self. _symbols = {}
        self.parent = None

    def is_defined(self, symbol, ent_type=None) -> bool:
        """Return True if the symbol is defined, False otherwise."""
        if isinstance(symbol, Entity):
            symbol = symbol.name

        # recursively check whether the symbol is defined here(current scope) or in the parent symbol table
        def_here = symbol in self.symbols and (self.symbols[symbol].type == ent_type if ent_type is not None else True)
        return def_here or (self.parent.is_defined(symbol, ent_type) if self.parent is not None else False)

    def store(self, e) -> None:
        """Store the given entity(function, class or variable)"""
        if self.is_defined(e):
            if self.parent is not None:
                defined = self.parent.get(e.name)  # has to be in a parent
                # class constructor: a method of the same name as the class returning void and without parameters
                if defined.type == EntityType.CLASS and e.type == EntityType.FUNCTION and e.return_type == VariableType.VOID and len(e.parameters) == 0:
                    self.symbols[e.name] = e
                    return
                # function overriding: same signatures, different bodies
                elif defined.type == e.type == EntityType.FUNCTION:
                    if not defined.has_same_signature(e):
                        raise SemanticErrorException('cannot override method \'{}\': signatures are different'.format(defined.name), e.line, e.column)
                    self.symbols[e.name] = e
                    return

            defined = self.get(e.name)
            raise SemanticErrorException('\'{}\' is already used in the {} \'{}\''.format(defined.name, defined.type.value, defined.name), e.line, e.column)
        else:
            self.symbols[e.name] = e

    def get(self, symbol, ent_type=None) -> Entity:
        """Return a stored entity."""
        self.raise_ifndef(symbol, ent_type)
        # recursively find the symbol with the given type
        sym = self.symbols.get(symbol)
        if sym is not None and (sym.type == ent_type if ent_type is not None else True):
            return sym
        else:
            return self.parent.get(symbol, ent_type)

    def raise_ifdef(self, symbol, ent_type=None) -> None:
        """Raise a SemanticErrorException if the given symbol is already defined."""
        if self.is_defined(symbol, ent_type):
            if isinstance(symbol, Entity):
                raise SemanticErrorException('symbol \'{}\' is already defined'.format(symbol.name), symbol.line, symbol.column)
            else:
                raise SemanticErrorException('symbol \'{}\' is already defined'.format(symbol))

    def raise_ifndef(self, symbol, ent_type=None) -> None:
        """Raise a SemanticErrorException if the given symbol is not defined."""
        if not self.is_defined(symbol, ent_type):
            if isinstance(symbol, Entity):
                raise SemanticErrorException('\'{}\' is not defined'.format(symbol.name), symbol.line, symbol.column)
            else:
                raise SemanticErrorException('{}\'{}\' is not defined'.format((ent_type.value + ' ') if ent_type is not None else '', symbol))
