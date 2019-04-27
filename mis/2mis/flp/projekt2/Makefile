# ------------------------
# Předmět: FLP 2018/2019
# Projekt: Turingův stroj
# Datum:   2019-04-24
# Autor:   Tomáš Aubrecht
# Login:   xaubre02
# ------------------------

SOURCE=flp19-log.pl
TARGET=flp19-log
ZIPFILE=flp-log-xaubre02.zip

# compilation
all: clean
	swipl -q -g start -o ${TARGET} -c ${SOURCE}
	rm -rf *.out

# zip
zip:
	zip ${ZIPFILE} ${SOURCE} README Makefile tests/*

# cleanup
clean:
	rm -rf ${TARGET} ${ZIPFILE}
