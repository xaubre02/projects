# Author: Tomas Aubrecht
# Date:   May 2018


NAME=detector
CC=c++
CFLAGS=--std=c++17 -Wall -Wextra -pedantic
LIBS=`pkg-config --libs opencv` -lboost_filesystem -lboost_system -lX11

all: $(NAME)

# modules
main.o: src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp -c
  
detection.o: src/detection.cpp
	$(CC) $(CFLAGS) src/detection.cpp -c

tester.o: src/tester.cpp
	$(CC) $(CFLAGS) src/tester.cpp -c

# compile
$(NAME): main.o detection.o tester.o
	$(CC) $(CFLAGS) main.o detection.o tester.o -o $(NAME) $(LIBS)

# show detector usage
run:
	./detector --help

# clean build files and doxygen files
clean:
	rm -rf $(NAME) *.o doc

# source code documentation
doxygen:
	mkdir -p doc
	doxygen doxygen.config
