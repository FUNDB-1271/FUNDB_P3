CFLAGS?=-g -Wall -pedantic -Wextra
CC=gcc 
OBJ=library.o index.o delete.o commands.o
EXE=library
DBNAME=test

all: $(EXE)

-include local.mk

run: 
	./$(EXE) first_fit $(DBNAME)

$(EXE): $(OBJ)
	$(CC) -o $@ $(OBJ)

library.o: library.c index.h commands.h
	$(CC) $(CFLAGS) -c $< 
	
index.o: index.c index.h
	$(CC) $(CFLAGS) -c $<

commands.o: commands.c
	$(CC) $(CFLAGS) -c $<

delete.o: delete.c 
	$(CC) $(CFLAGS) -c $<

clean: 
	rm *.o $(EXE) 

clean_db:
	rm $(DBNAME).db $(DBNAME).ind $(DBNAME).lst 
