# -*- coding: UTF-8 -*-
"""
- Project: VYPa19 language compiler
- Author: Tomas Aubrecht (xaubre02), Jan Kotras (xkotra01)
- Date:   2019-10-25
- Desc:   compiler.
"""
from __future__ import unicode_literals


class CodeCreator:

    code_creator_instance = None

    def __init__(self, silent_mode=True, create_comments=True) -> None:
        self.__code = ""
        self.create_comments = create_comments
        self.silent_mode = silent_mode
        self.clear()

    @classmethod
    def code_creator(cls):
        if not cls.code_creator_instance:
            cls.code_creator_instance = CodeCreator()
        return cls.code_creator_instance

    def clear(self):
        self.__code = ""

    def _add_command(self, command):
        self.__code += command + '\n'

    def comment(self, comment):
        if self.create_comments:
            self.__code += '# ' + comment + '\n'

    def code(self) -> str:
        return self.__code

    def alias(self, alias, id):
        self._add_command("ALIAS {} {}".format(alias,id))

    def label(self, label):
        self._add_command("LABEL {}".format(label))

    # CHUNK manipulation instructions

    def create(self, dest, size):
        self._add_command("CREATE {},{}".format(dest, size))

    def copy(self, dest, id):
        self._add_command("COPY {},{}".format(dest, id))

    def get_size(self, dest, id):
        self._add_command("GETSIZE {},{}".format(dest, id))

    def get_word(self, dest, id, index):
        self._add_command("GETWORD {},{},{}".format(dest, id, index))

    def resize(self, id, new_size):
        self._add_command("RESIZE {},{}".format(id, new_size))

    def set_word(self, id, index, value):
        self._add_command("SETWORD {},{},{}".format(id, index, value))

    def destroy(self, id):
        self._add_command("DESTROY {}".format(id))

    # CONTROL FLOW instructions

    def call(self, pc, where):
        self._add_command("CALL {},{}".format(pc, where))

    def return_i(self, pc):
        self._add_command("RETURN {}".format(pc))

    def set(self, dest, value):
        self._add_command("SET {},{}".format(dest, value))

    def jump(self, label):
        self._add_command("JUMP {}".format(label))

    def jumpz(self, label, src):
        self._add_command("JUMPZ {},{}".format(label, src))

    def jumpnz(self, label, src):
        self._add_command("JUMPNZ {},{}".format(label, src))

    # input / output

    def reads(self, dst):
        self._add_command("READS {}".format(dst))

    def writes(self, id):
        self._add_command("WRITES {}".format(id))

    def readi(self, dst):
        self._add_command("READI {}".format(dst))

    def readf(self, dst):
        self._add_command("READF {}".format(dst))

    def writei(self, src):
        self._add_command("WRITEI {}".format(src))

    def writef(self, src):
        self._add_command("WRITEF {}".format(src))

    # ARTIHMETIC

    # integers

    def addi(self, dest, src1, src2):
        self._add_command("ADDI {},{},{}".format(dest, src1, src2))

    def subi(self, dest, src1, src2):
        self._add_command("SUBI {},{},{}".format(dest, src1, src2))

    def muli(self, dest, src1, src2):
        self._add_command("MULI {},{},{}".format(dest, src1, src2))

    def divi(self, dest, src1, src2):
        self._add_command("DIVI {},{},{}".format(dest, src1, src2))

    def lti(self, dest, src1, src2):
        self._add_command("LTI {},{},{}".format(dest, src1, src2))

    def gti(self, dest, src1, src2):
        self._add_command("GTI {},{},{}".format(dest, src1, src2))

    def eqi(self, dest, src1, src2):
        self._add_command("EQI {},{},{}".format(dest, src1, src2))

    # float

    def addf(self, dest, src1, src2):
        self._add_command("ADDF {},{},{}".format(dest, src1, src2))

    def subf(self, dest, src1, src2):
        self._add_command("SUBF {},{},{}".format(dest, src1, src2))

    def mulf(self, dest, src1, src2):
        self._add_command("MULF {},{},{}".format(dest, src1, src2))

    def divf(self, dest, src1, src2):
        self._add_command("DIVF {},{},{}".format(dest, src1, src2))

    def ltf(self, dest, src1, src2):
        self._add_command("LTF {},{},{}".format(dest, src1, src2))

    def gtf(self, dest, src1, src2):
        self._add_command("GTF {},{},{}".format(dest, src1, src2))

    def eqf(self, dest, src1, src2):
        self._add_command("EQF {},{},{}".format(dest, src1, src2))

    # string

    def lts(self, dest, src1, src2):
        self._add_command("LTS {},{},{}".format(dest, src1, src2))

    def gts(self, dest, src1, src2):
        self._add_command("GTS {},{},{}".format(dest, src1, src2))

    def eqs(self, dest, src1, src2):
        self._add_command("EQS {},{},{}".format(dest, src1, src2))

    # boolean

    def and_i(self, dest, src1, src2):
        self._add_command("AND {},{},{}".format(dest, src1, src2))

    def or_i(self, dest, src1, src2):
        self._add_command("OR {},{},{}".format(dest, src1, src2))

    def not_i(self, dest, src):
        self._add_command("NOT {},{}".format(dest, src))

    # convert

    def int_2_float(self, dest, src):
        self._add_command("INT2FLOAT {},{}".format(dest, src))

    def float_2_int(self, dest, src):
        self._add_command("FLOAT2INT {},{}".format(dest, src))

    def int_2_string(self, dest, src):
        self._add_command("INT2STRING {},{}".format(dest, src))

    def float_2_string(self, dest, src):
        self._add_command("FLOAT2STRING {},{}".format(dest, src))

    # DEBUG instructions

    def _add_debug_command(self, command):
        if not self.silent_mode:
            self._add_command(command)

    def dprinti(self, src):
        self._add_debug_command("DPRINTI {}".format(src))

    def dprintf(self, src):
        self._add_debug_command("DPRINTF {}".format(src))

    def dprints(self, id):
        self._add_debug_command("DPRINTS {}".format(id))

    def dump_regs(self):
        self._add_debug_command("DDUMPREGS")

    def dump_stack(self):
        self._add_debug_command("DUMPSTACK")

    def dump_heap(self):
        self._add_debug_command("DUMPHEAP")

    def dump_chunk(self, id):
        self._add_debug_command("DUMPCHUNK {}".format(id))


