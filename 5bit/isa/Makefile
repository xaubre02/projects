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

main.o: src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp -c

params.o: src/params.cpp src/params.hpp
	$(CC) $(CFLAGS) src/params.cpp -c

myldap.o: src/myldap.cpp src/myldap.hpp
	$(CC) $(CFLAGS) src/myldap.cpp -c

projekt: main.o params.o myldap.o
	$(CC) $(CFLAGS) main.o params.o myldap.o -o myldap -lpthread
	rm *.o

clear:
	rm *.o

test.o: test/test.cxx
	$(CC) $(CFLAGS) test/test.cxx -c -lpthread

test: test.o
	$(CC) $(CFLAGS) test.o -o test/test -lpthread
	rm test.o
