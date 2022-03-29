CC = gcc
CFLAGS = -Wall -ggdb
WIDTH = 0.0000000001
n = 100

run_example_test:
	$(CC) $(CFLAGS) main.c -o main
	./main 0.0000000001 100

run_test:
	$(CC) $(CFLAGS) main.c -o main
	./main $(WIDTH) $(n)

run_valgrind_test:
	$(CC) $(CFLAGS) main.c -o main
	valgrind -s --leak-check=full --show-leak-kinds=all ./main 0.000000001 100

clean:
	rm -rf main wn_files/w* report.txt

help:
	@echo "To compile and run example test with width of each rectangle equal 0.00000000001 and number of rectangles 100: make run_example_test"
	@echo "To compile and run test with optional arguments: make run_test WIDTH='ARG1' n='ARG2'"
	@echo "\t (where 'ARG1' is the width of each rectangle and n is the number of rectangles)"
	@echo "To compile and run test to check if there are any data leakage: make run_valgrind_test"
	@echo "To clean all not necessary files: make clean"