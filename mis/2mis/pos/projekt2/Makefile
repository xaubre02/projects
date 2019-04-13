# --------------------------
# Predmet: POS 2018/2019
# Projekt: shell
# Datum:   2019-04-13
# Autor:   Tomas Aubrecht
# Login:   xaubre02
# --------------------------

SOURCE=projekt2.c
TARGET=shell
CC=gcc
CFLAGS=-Wall -Wextra -pedantic

# compilation
all: clean
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET) -lpthread

# cleanup
clean:
	rm -rf $(TARGET)
