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