#!/usr/bin/env python3
"""
- Project: VYPa19 language compiler
- Author: Tomas Aubrecht (xaubre02), Jan Kotras (xkotra01)
- Date:   2019-10-25
- Desc:   compiler.
"""

from compiler import *


class Path:
    """Class representing a path to a symbol."""

    def __init__(self, ctx, sym_table, instance=None):
        """Initialize the path."""
        self._vars = []  # list of variables
        self._val_type = None

        # sequence of object members specifying the path
        sequence = []

        for ap in ctx.atomic_path():
            # variable
            if ap.ID() is not None:
                # check whether the ID is defined
                sequence.append(ap.ID().getText())
            # this
            elif ap.THIS() is not None:
                sequence.append(ap.THIS().getText())
            # super
            elif ap.SUPER() is not None:
                sequence.append(ap.SUPER().getText())
            # should not happen
            else:
                raise InternalErrorException('unprocessed atomic path')

        prev_cls = instance  # previous object class specified in a path
        for i, name in enumerate(sequence):
            # print('{:d}: {}'.format(i, name))
            if name in ['this', 'super']:
                # can only specify the beginning of a path
                if i != 0:
                    raise SemanticErrorException('pseudo-variables \'this\' and \'super\' can only start the path')
                if instance is None:
                    raise SemanticErrorException('pseudo-variables \'this\' and \'super\' can only be accessed from a method body of a given class')

                if name == 'this':
                    self._target = instance
                else:
                    self._target = instance.parent
            else:
                # first path specifier
                if i == 0:
                    ent = sym_table.get(name)
                    if ent.type != EntityType.VARIABLE:
                        raise SemanticErrorException('{} is not a variable'.format(name), ent.line, ent.column)
                    # variable is an object -> specify its class
                    if ent.data_type == VariableType.OBJECT:
                        if not sym_table.is_defined(ent.object_class):
                            raise SemanticErrorException('class \'{}\' is not defined'.format(ent.object_class))
                        prev_cls = sym_table.get(ent.object_class)
                else:
                    # check the class of the previous variable(object)
                    if prev_cls is None:
                        raise InternalErrorException('object \'{}\' without a specified class'.format(prev_cls))

                    # get a member from the previous class
                    ent = prev_cls.get_member(name)
                    prev_cls = ent

                # data type of a variable or a return type of a function
                self._val_type = ent.data_type if ent.type == EntityType.VARIABLE else ent.return_type
                self._target = ent

    @property
    def value_type(self) -> VariableType:
        """Return the type of the value specified by this path."""
        return self._val_type

    @property
    def target(self) -> Entity:
        """Return the entity specified by this path."""
        return self._target
