# --------------------------
# Predmet: POS 2018/2019
# Projekt: ticket algoritmus
# Datum:   2019-04-07
# Autor:   Tomas Aubrecht
# Login:   xaubre02
# --------------------------

SOURCE=projekt1.c
TARGET=program
CC=gcc
CFLAGS=-Wall -Wextra -pedantic

# compilation
all: clean
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET) -lpthread

# cleanup
clean:
	rm -rf $(TARGET)
