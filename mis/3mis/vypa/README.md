# VYPa19
VYPa Project for 2019/2020

### How to use on Merlin
1. download the repository
2. run "_make_" inside the repo to generate the source files
3. run "_chmod +x vypcomp_" inside the repo
4. now you can run  _vypcomp_ file (or _src/main.py_)

### How to use on Windows
1. download the repository
2. download antlr4 tool from www.antlr.org/download/antlr-4.7.2-complete.jar
3. follow the instructions from https://github.com/antlr/antlr4/blob/master/doc/getting-started.md to setup the tool
4. download antlr4 python3 runtime(available from PyPi): run "_pip install antlr4-python3-runtime_"
5. run "_antlr4 -Dlanguage=Python3 VYPa19.g4 -o src/generated_" inside the repo
6. now you can run _src/main.py_

### How to use on Windows without ANTLR v4 Tool
1. setup antlr4 runtime: run "_pip install antlr4-python3-runtime_"
2. download the repository
3. generate files on Merlin and copy them to your computer _(from src/generated)_
6. now you can run _src/main.py_
