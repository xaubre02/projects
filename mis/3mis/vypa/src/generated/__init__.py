#!/usr/bin/env python3
"""
- Project: VYPa19 language compiler
- Author: Tomas Aubrecht (xaubre02)
- Date:   2019-10-08
- Desc:   __init__ module.
"""

import sys

# add path to the antlr4 runtime on merlin.fit.vutbr.cz
antlr = '/pub/courses/vyp/antlr4-python3-runtime-4.7.2/src'
sys.path.insert(0, antlr)
if antlr in sys.path:
    from generated.VYPa19Lexer import VYPa19Lexer
    from generated.VYPa19Parser import VYPa19Parser
