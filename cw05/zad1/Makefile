CC = gcc
CFLAGS = -Wall -ggdb

run_test:
	$(CC) $(CFLAGS) main.c -o main
	./main "commands.txt"

run_valgrind_test:
	$(CC) $(CFLAGS) main.c -o main
	valgrind -s --leak-check=full --show-leak-kinds=all ./main "commands.txt"

clean:
	rm -rf main

help:
	@echo "To compile and run test on commands.txt file: make run_test."
	@echo "To compile and run test on commands.txt file and check for data leakage: make run_valgrind_test."
	@echo "To clean all not necessary files: make clean"