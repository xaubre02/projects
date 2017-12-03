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
CFLAGS=-O2 -g -Wall -Wextra -Werror -pedantic

all: wine

wine: src/vinobrani.cpp
	$(CC) $(CFLAGS) src/vinobrani.cpp -o vinobrani -lsimlib

run:
	./vinobrani

