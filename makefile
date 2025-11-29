CFLAGS?=-g -Wall -pedantic -Wextra
CC=gcc 
OBJ=library.o index.o database.o commands.o book.o deletedlist.o
EXE=library
DBNAME=test

all: $(EXE)

-include local.mk

run: 
	./$(EXE) first_fit $(DBNAME)

$(EXE): $(OBJ)
	$(CC) -o $@ $(OBJ)

library.o: library.c index.h commands.h database.h types.h deletedlist.h book.h
	$(CC) $(CFLAGS) -c $< 
	
index.o: index.c index.h 
	$(CC) $(CFLAGS) -c $<

commands.o: commands.c commands.h
	$(CC) $(CFLAGS) -c $<

database.o: database.c database.h
	$(CC) $(CFLAGS) -c $<

deletedlist.o: deletedlist.c deletedlist.h
	$(CC) $(CFLAGS) -c $<

book.o: book.c book.h
	$(CC) $(CFLAGS) -c $<

valgrind: $(EXE)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(EXE) first_fit $(DBNAME)

clean: 
	rm *.o $(EXE) 

clean_db:
	rm $(DBNAME).db $(DBNAME).ind $(DBNAME).lst 
