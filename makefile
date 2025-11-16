CFLAGS=-g -Wall -pedantic -Wextra
CC=gcc 
OBJ=library.o
EXE=library

all: $(EXE)

library: $(OBJ)
	$(CC) $(OBJ) -o $@ 

library.o: library.c
	$(CC) $(CFLAGS) -c $< -o library.o
	
clean: 
	rm *.o $(EXE)