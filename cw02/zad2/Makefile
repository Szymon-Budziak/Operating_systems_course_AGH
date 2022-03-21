CC = gcc
CFLAGS = -Wall -std=c11 -ggdb

run_test:
	$(CC) $(CFLAGS) main.c -o main
	./main 'j' test_files/test_file.txt

run_valgrind_test:
	$(CC) $(CFLAGS) main.c -o main
	valgrind -s --leak-check=full --show-leak-kinds=all ./main 'z' test_files/test_file.txt

clean:
	rm -rf main
	> pomiar_zad_2.txt

help:
	@echo "To compile and run example test on test_file.txt from test_files and find j occurrences: make run_test"
	@echo "To compile and run example test on test_file.txt from test_files, find j occurrences and check if there are any data leakage: make run_valgrind_test"
	@echo "To clean all not necessary files: make clean"