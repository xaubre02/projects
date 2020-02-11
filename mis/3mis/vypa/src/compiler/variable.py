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


class VariableType(Enum):
    """Class representing a type."""

    # types
    VOID = 'void'
    INT = 'int'
    STRING = 'string'
    OBJECT = 'object'

    @staticmethod
    def recognize(ctx) -> 'VariableType':
        """Recognize the type from the context."""
        # void
        if hasattr(ctx, 'VOID'):
            t = ctx.VOID()
            if t is not None:
                return VariableType.VOID
        # data type
        t = ctx.data_type()
        if t is not None:
            # integer
            dt = t.INT()
            if dt is not None:
                return VariableType.INT
            # string
            dt = t.STRING()
            if dt is not None:
                return VariableType.STRING
            # object
            return VariableType.OBJECT

    @staticmethod
    def recognize_class(ctx) -> str:
        """Recognize the object class from the context."""
        ocls = None
        if VariableType.recognize(ctx) == VariableType.OBJECT:
            ocls = ctx.data_type().ID().getText()

        return ocls


class Variable(Entity):
    """Class representing a variable."""

    def __init__(self, ctx=None, name=None, dtype=None, vclass=None, line=None, column=None, class_owner=None):
        """Initialize the variable."""
        # context has to be specified and nothing else or name and data type have to be specified
        if (ctx is not None and (name is not None or dtype is not None or vclass is not None or line is not None or column is not None)) or \
           (ctx is None and (name is None or dtype is None)):
            raise InternalErrorException('invalid Variable initialization arguments')

        # initialize from context
        if ctx is not None:
            super().__init__(EntityType.VARIABLE, ctx=ctx)
            self._dtype = VariableType.recognize(ctx)
            self._cls = VariableType.recognize_class(ctx)
        else:
            super().__init__(EntityType.VARIABLE, name=name, line=line, column=column)
            self._dtype = dtype
            self._cls = vclass
        self._class_type = class_owner
        self.stack_index = 1
        self.attribute_index = 0

    def code(self, code_creator: CodeCreator):
        """set value into accumulator"""
        if not self._class_type:
            if self.stack_index > 0:
                code_creator.set('$ACC1', '[$FP+{}]'.format(self.stack_index))
            else:
                code_creator.set('$ACC1', '[$FP-{}]'.format(-self.stack_index))
        else:
            code_creator.get_word("$ACC2", "$VT", self._class_type.cls_index)
            code_creator.get_word("$ACC1", "$ACC2", "$SELF")

    @property
    def data_type(self) -> VariableType:
        """Return the data type of this variable."""
        return self._dtype

    @property
    def object_class(self) -> 'Class':
        """Return the class of this variable, if it is an object."""
        return self._cls
