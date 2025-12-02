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

# Regla para ejecutar todos los tests de la carpeta tests
test-all:
	@echo "Ejecutando todos los tests..."
	@echo "======================================"
	@failed=0; \
	total=0; \
	for test in *.sh; do \
		total=$$((total + 1)); \
		echo "Ejecutando $$(basename "$$test")..."; \
		chmod +x "$$test" 2>/dev/null; \
		echo "--------------------------------------"; \
		if timeout 30s expect -c "./$$test"; then \
			echo "PASO: $$(basename "$$test")"; \
		else \
			echo "FALLO: $$(basename "$$test")"; \
			failed=$$((failed + 1)); \
		fi; \
		echo ""; \
	done; \
	echo "Resultados: $$((total - failed))/$$total tests pasaron"; \
	if [ $$failed -eq 0 ]; then \
		echo "Todos los tests pasaron correctamente"; \
	else \
		echo "$failed test(s) fallaron"; \
		exit 1; \
	fi

# Regla para ejecutar un test específico
test-%:
	@if [ -f "$*.sh" ]; then \
		echo "Ejecutando $*.sh"; \
		chmod +x "$*.sh"; \
		"$*.sh"; \
	else \
		echo "Error: Test $*.sh no encontrado"; \
		exit 1; \
	fi

# Regla para listar todos los tests disponibles
list-tests:
	@echo "Tests disponibles:"; \
	for test in *.sh; do \
		echo "  - $$(basename "$$test" .sh)"; \
	done

# Crear la carpeta tests si no existe
tests-dir:
	@mkdir -p tests

# Limpiar archivos generados por los tests
clean-tests:
	@echo "Limpiando archivos de tests..."
	@rm -f test*.db test*.ind test*.lst

# Ejecutar tests con valgrind
test-valgrind:
	@echo "Ejecutando tests con valgrind..."
	@for test in *.sh; do \
		echo "Test: $$test con valgrind"; \
		chmod +x "$$test"; \
		if ! valgrind --leak-check=full --error-exitcode=1 "$$test" 2>/dev/null; then \
			echo "FALLO: $$test tiene memory leaks"; \
			exit 1; \
		else \
			echo "PASO: $$test sin memory leaks"; \
		fi; \
	done
