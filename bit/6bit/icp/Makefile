NAME=BlockEditor

ZIPNAME=xaubre02-xbedna62-100-0.zip
ZIPFILES =src/* src/core/* src/gui/* src/images/* doc/ examples/* Makefile README.txt

CC=g++
CFLAGS=-Wall -Wextra --std=c++11 -pedantic

all: gui

gui: clean
	qmake src/blockeditor.pro -o build/Makefile
	$(MAKE) -B CXX=$(CC) -C build -o $(NAME)
	rm -rf  build/*.o build/*.cpp build/*.h

$(NAME): src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp -o $(NAME)


doxygen:
	doxygen src/doxygen.config

run:
	build/blockeditor

pack:
	zip $(ZIPNAME) $(ZIPFILES)

clean:
	rm -rf build doc/* $(ZIPNAME)
