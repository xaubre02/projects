 #****************************************#
 #*  Datum:	28.4.2016                	*#
 #*  Autor: 	Tomáš Aubrecht (xaubre02)	*#
 #*  Předmět: Operační systém (IOS)		*#
 #*  Projekt:	Synchronizace procesů    	*#
 #****************************************#

CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic
NAME=proj2

all: $(NAME).c
	$(CC) $(CFLAGS) $(NAME).c -o $(NAME) -lrt -pthread
