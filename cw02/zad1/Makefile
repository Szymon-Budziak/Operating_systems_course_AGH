CC = gcc
CFLAGS = -Wall -std=c11 -ggdb

run_test_with_files:
	$(CC) $(CFLAGS) main.c -o main
	./main test_files/in_file.txt test_files/out_file.txt

run_test_without_files:
	$(CC) $(CFLAGS) main.c -o main
	./main

run_valgrind_test:
	$(CC) $(CFLAGS) main.c -o main
	valgrind -s --leak-check=full --show-leak-kinds=all ./main test_files/in_file.txt test_files/out_file.txt

clean:
	rm -rf main
	> pomiar_zad_1.txt
	> test_files/out_file.txt

help:
	@echo "To compile and run example test on in_file.txt from test_files and copy it to out_file.txt from test_files: make run_test_with_files"
	@echo "To compile and run example test without specified files (you have to pass them in command line): make run_test_without_files"
	@echo "To compile and run example test on in_file.txt from test_files and copy it to out_file.txt from test_files and if there are any data leakage: make run_test_with_files"
	@echo "To clean all not necessary files: make clean"