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
NAME=vyroba_vina

all: wine

wine: src/$(NAME).cpp
	$(CC) $(CFLAGS) src/$(NAME).cpp -o $(NAME) -lsimlib

run:
	./$(NAME) 1 3 10 120
	./$(NAME) 2 4 10 120
	./$(NAME) 3 4 15 240
	./$(NAME) 4 8 10 120
	./$(NAME) 5 1 10 120

clean:
	rm $(NAME) experiment_*.out
