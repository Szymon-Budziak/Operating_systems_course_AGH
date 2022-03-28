CC = gcc
CFLAGS = -Wall -ggdb
n = 10

run_example_test:
	$(CC) $(CFLAGS) main.c -o main
	./main 10

run_test:
	$(CC) $(CFLAGS) main.c -o main
	./main $(n)

clean:
	rm -rf main

help:
	@echo "To compile and run example test with argument 10: make run_example_test"
	@echo "To compile and run test with optional argument: make run_test n='ARG' (where 'ARG' is your number)"
	@echo "To clean all not necessary files: make clean"