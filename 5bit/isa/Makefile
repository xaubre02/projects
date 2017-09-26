#**************************************
# **********
# Predmet: ISA
# Projekt: LDAP Server
# Datum: 2017-09-26
# Autor: Tomas Aubrecht
# Login: xaubre02
# **********
#**************************************

CC=g++
CFLAGS=-Wall -Wextra -Werror -pedantic

all: projekt

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp -c
	
params.o: params.cpp params.hpp
	$(CC) $(CFLAGS) params.cpp -c

#server.o:

projekt: main.o params.o
	$(CC) $(CFLAGS) params.o main.o -o myldap
	rm *.o

clear:
	rm *.o
