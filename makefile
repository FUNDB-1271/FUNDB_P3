CFLAGS=-g -Wall -pedantic -Wextra
CC=gcc 
OBJ=library.o
EXE=library


all: $(EXE)

-include local.mk
run: 
	./$(EXE) first_fit database

library: $(OBJ)
	$(CC) -o $@ $(OBJ)

library.o: library.c
	$(CC) $(CFLAGS) -c $< 
	
clean: 
	rm *.o $(EXE)