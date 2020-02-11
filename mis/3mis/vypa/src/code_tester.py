# -*- coding: UTF-8 -*-
"""
- Project: VYPa19 language compiler
- Author: Tomas Aubrecht (xaubre02), Jan Kotras (xkotra01)
- Date:   2019-10-25
- Desc:   compiler.
"""
from __future__ import unicode_literals

import unittest

from compiler.code_creator import CodeCreator


class TestCodeCreator(unittest.TestCase):

    def test_instructions(self):
        code = CodeCreator()
        code.copy("12",5)
        self.assertEqual(code.code(), "COPY 12,5\n")
        code.clear()
        code.float_2_int(52, "$asd")
        self.assertEqual(code.code(), "FLOAT2INT 52,$asd\n")
        code.clear()
        code.and_i("a", "b", "c")
        self.assertEqual(code.code(), "AND a,b,c\n")
        code.clear()

    def test_not_silent_mode(self):
        code = CodeCreator()
        code.dump_heap()
        self.assertEqual(code.code(), "")
        code.clear()

        code.silent_mode = False
        code.dump_heap()
        self.assertEqual(code.code(), "DUMPHEAP\n")
        code.clear()
