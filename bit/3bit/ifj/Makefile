#*************************************************************
#*****************************************************
#* 
#* Project: Implementace interpretu imperativního jazyka IFJ16
#* 
#* Authors: Vít Ambrož		(xambro15)
#*			Tomáš Aubrecht	(xaubre02)
#*			David Bednařík	(xbedna62)
#*			Andrej Čulen	(xculen02)
#* 
#* Description: https://wis.fit.vutbr.cz/FIT/st/course-files-st.php/course/IFJ-IT/projects/ifj2016.pdf
#*
#*****************************************************
#*************************************************************

CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic

all: ifj16
#lex_test

main.o: main.c parser.o scanner.o error.o
	$(CC) $(CFLAGS) main.c -c

scanner.o: scanner.c scanner.h
	$(CC) $(CFLAGS) scanner.c -c

error.o: error.c error.h
	$(CC) -std=gnu99 -Wextra -Werror -pedantic error.c -c

adt.o: adt.c adt.h scanner.o error.o
	$(CC) -std=gnu99 -Wextra -Werror -pedantic adt.c -c

ial.o: ial.c ial.h tsymbols.o error.o
	$(CC) $(CFLAGS) ial.c -c

tsymbols.o: tsymbols.c tsymbols.h error.o scanner.o
	$(CC) $(CFLAGS) tsymbols.c -c

kgenerator.o: kgenerator.c kgenerator.h error.o adt.o tsymbols.o
	$(CC) $(CFLAGS) kgenerator.c -c

interpret.o: interpret.c interpret.h kgenerator.o error.o adt.o tsymbols.o
	$(CC) -std=gnu99 -Wextra -Werror -pedantic interpret.c -c

parser.o: parser.c parser.h scanner.o error.o adt.o ial.o tsymbols.o kgenerator.o
	$(CC) $(CFLAGS) parser.c -c

ifj16: main.o parser.o adt.o scanner.o error.o ial.o tsymbols.o kgenerator.o interpret.o
	$(CC) $(CFLAGS) main.o parser.o adt.o scanner.o error.o ial.o tsymbols.o kgenerator.o interpret.o -o ifj16 -lm

.PHONY: clean clean-all test

clean:
	rm *.o

clean-all:
	rm *.o ifj16

test:
	./testy/tester
