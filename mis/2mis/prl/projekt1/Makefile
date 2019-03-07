# -----------------------
# Předmět: PRL 2018/2019
# Projekt: Bucket sort
# Datum:   2019-03-07
# Autor:   Tomáš Aubrecht
# Login:   xaubre02
# -----------------------

SOURCE=bks.cpp
TARGET=bks
CC=mpic++
CFLAGS=--prefix /usr/local/share/OpenMPI
ZIPFILE=xaubre02.zip

# compilation
all: clean
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET)
	rm -rf *.o

# zip
zip:
	zip $(ZIPFILE) bks.cpp bks.h test.sh xaubre02.pdf

# cleanup
clean:
	rm -rf *.o $(TARGET) $(ZIPFILE)