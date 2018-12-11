# IPK: Client-Server app
# Tomas Aubrecht (xaubre02)
# 2017-03-01

CC=c++
CFLAGS=-Wall -Wextra -Werror -pedantic

all: projekt

client.o:
	$(CC) $(CFLAGS) client.cpp -c

server.o: server.cpp netcom.o
	$(CC) $(CFLAGS) server.cpp -c

netcom.o: netcom.cpp netcom.h
	$(CC) $(CFLAGS) netcom.cpp -c

projekt: client.o server.o netcom.o
	$(CC) $(CFLAGS) client.o netcom.o -o ftrest
	$(CC) $(CFLAGS) server.o netcom.o -o ftrestd
	rm *.o
