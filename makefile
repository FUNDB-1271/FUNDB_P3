CFLAGS?=-g -Wall -pedantic -Wextra
CC=gcc 
OBJ=library.o index.o database.o commands.o book.o deletedlist.o
EXE=library
DBNAME=test

all: $(EXE)

-include local.mk

run: 
	./$(EXE) first_fit $(DBNAME)

first_fit:
	./$(EXE) first_fit $(DBNAME)

worst_fit:
	./$(EXE) worst_fit $(DBNAME)

best_fit:
	./$(EXE) best_fit $(DBNAME)

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

valgrind_first_fit: $(EXE)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(EXE) first_fit $(DBNAME)

valgrind_best_fit: $(EXE)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(EXE) best_fit $(DBNAME)

valgrind_worst_fit: $(EXE)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(EXE) worst_fit $(DBNAME)

clean: 
	rm *.o $(EXE) 

clean_db:
	rm $(DBNAME).db $(DBNAME).ind $(DBNAME).lst 

# Regla para ejecutar todos los tests de la carpeta tests
test-all:
	@echo "Executing all tests"
	@make add_data_test
	@make add_delete_test_01
	@make add_delete_test_02
	@make add_delete_test_03
	@make add_delete_test_04
	@make add_index_test
	@make cli_tests
	@make many_entries
	@make reload_index
	@make use_deleted_books
	@echo "All done :)"	

add_data_test:
	@echo "Executing add_data_test.sh"
	@chmod +x add_data_test.sh
	rm -f test.ind test.db test.lst
	expect add_data_test.sh
	@chmod -x add_data_test.sh


add_delete_test_01: 
	@echo "Executing add_delete_test_01.sh"
	@chmod +x add_delete_test_01.sh
	rm -f test.ind test.db test.lst
	./add_delete_test_01.sh	
	@chmod -x add_delete_test_01.sh

add_delete_test_02: 
	@echo "Executing add_delete_test_02.sh"
	@chmod +x add_delete_test_02.sh
	rm -f test.ind test.db test.lst
	./add_delete_test_02.sh
	@chmod -x add_delete_test_02.sh

add_delete_test_03: 
	@echo "Executing add_delete_test_03.sh"
	@chmod +x add_delete_test_03.sh
	rm -f test.ind test.db test.lst
	./add_delete_test_03.sh
	@chmod -x add_delete_test_03.sh

add_delete_test_04: 
	@echo "Executing add_delete_test_04.sh"
	@chmod +x add_delete_test_04.sh
	rm -f test.ind test.db test.lst
	./add_delete_test_04.sh
	@chmod -x add_delete_test_03.sh

add_index_test:
	@echo "Executing add_index_test.sh"
	@chmod +x add_index_test.sh
	rm -f test.ind test.db test.lst
	./add_index_test.sh
	@chmod -x add_index_test.sh

cli_tests:
	@echo "Executing cli_tests.sh"
	@chmod +x cli_tests.sh
	rm -f test.ind test.db test.lst
	./cli_tests.sh
	@chmod -x cli_tests.sh

many_entries:
	@echo "Executing many_entries.sh"
	@chmod +x many_entries.sh
	rm -f test.ind test.db test.lst
	./many_entries.sh
	@chmod -x many_entries.sh

reload_index:
	@echo "Executing reload_index.sh"
	@chmod +x reload_index.sh
	@chmod +x add_index_test.sh
	rm -f test.ind test.db test.lst
	./reload_index.sh
	@chmod -x reload_index.sh
	@chmod -x add_index_test.sh

use_deleted_books:
	@echo "Executing test_use_deleted_books.sh"
	@chmod +x test_use_deleted_books.sh
	rm -f test.ind test.db test.lst
	./test_use_deleted_books.sh
	@chmod -x test_use_deleted_books.sh

# Regla para listar todos los tests disponibles
list-tests:
	@echo "Tests disponibles:"; \
	for test in *.sh; do \
		echo "  - $$(basename "$$test" .sh)"; \
	done
