# ------------------------
# Předmět: FLP 2018/2019
# Projekt: simplify-bkg
# Datum:   2019-02-16
# Autor:   Tomáš Aubrecht
# Login:   xaubre02
# ------------------------

SOURCE=simplify-bkg.hs
TARGET=simplify-bkg
ZIPFILE=flp-fun-xaubre02.zip

# compilation
all: clean
	ghc ${SOURCE} -o ${TARGET}
	rm -rf *.hi *.o

# zip
zip:
	zip ${ZIPFILE} ${SOURCE} README Makefile tests

# cleanup
clean:
	rm -rf *.hi *.o ${TARGET} ${ZIPFILE}

# run program
run: all
	./${TARGET} ${arg1} ${arg2}

# run tests
tests:
	chmod +x ./run_tests.sh
	./run_tests.sh

.PHONY: tests
