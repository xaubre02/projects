#**************************************
# **********
# Projekt: IPK - Matematicky klient
# Datum: 2017-04-15
# Autor: Tomas Aubrecht
# Login: xaubre02
# **********
#**************************************

CC=c++
CFLAGS=-Wall -Wextra -Werror -pedantic

all: projekt

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp -c

client.o: client.cpp client.h
	$(CC) $(CFLAGS) client.cpp -c

projekt: main.o client.o
	$(CC) $(CFLAGS) main.o client.o -o ipk-client -lcrypto
	rm *.o
