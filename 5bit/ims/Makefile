#**************************************
# **********
# Predmet: IMS
# Projekt: Vyroba vina
# Datum: 2017-12-01
# Autor: Tomas Aubrecht
# Login: xaubre02
# **********
#**************************************

CC=g++
CFLAGS=-Wall -Wextra -Werror -pedantic

all: projekt

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp -c

projekt: main.o
	$(CC) $(CFLAGS) main.o -o wine
	rm *.o

run:
	./wine
