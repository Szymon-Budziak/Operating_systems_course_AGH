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