# -------------------------
# Subject: VYPa 2019/2020
# Project: VYPa19 compiler
# Author: Tomas Aubrecht
# Date:   2019-10-08
# -------------------------

OUTDIR=src/generated

# lexer and parser generation
all: clean
	java -Xmx500M -cp "/pub/courses/vyp/antlr-4.7.2-complete.jar:$CLASSPATH" org.antlr.v4.Tool -Dlanguage=Python3 VYPa19.g4 -o ${OUTDIR} -no-listener

# cleanup
clean:
	rm -rf ${OUTDIR}/VYPa19* ${OUTDIR}/__pycache__
