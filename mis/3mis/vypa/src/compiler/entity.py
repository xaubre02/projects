#!/usr/bin/env python3
"""
- Project: VYPa19 language compiler
- Author: Tomas Aubrecht (xaubre02), Jan Kotras (xkotra01)
- Date:   2019-10-25
- Desc:   compiler.
"""

from enum import Enum
from abc import abstractmethod


class EntityType(Enum):
    """Class representing a type of entity."""
    # types
    VARIABLE = 'variable'
    FUNCTION = 'function'
    CLASS = 'class'


class Entity:
    """Base class for each entity(e.g. function, class)."""

    def __init__(self, etype, ctx=None, name=None, line=None, column=None):
        """Initialize the entity."""
        # type of entity
        self._type = etype

        # no context provided
        if ctx is None:
            self._name = name   # entity name
            self._line = line   # entity line number in the source code
            self._col = column  # entity column number in the source code
        else:
            name = ctx.ID()
            # in class definition, there are 2 ID (class name and parent class name)
            if isinstance(name, list):
                name = name[0]

            self._name = name.symbol.text
            self._line = name.symbol.line
            self._col = name.symbol.column

    @property
    def name(self) -> str:
        """Return the name of this entity."""
        return self._name

    @property
    def line(self) -> str:
        """Return the line where the entity starts in the source code."""
        return self._line

    @property
    def column(self) -> str:
        """Return the column where the entity starts in the source code."""
        return self._col

    @property
    def type(self) -> EntityType:
        """Return the type of this entity."""
        return self._type

    @abstractmethod
    def code(self) -> str:
        """Generate an assembly code."""
