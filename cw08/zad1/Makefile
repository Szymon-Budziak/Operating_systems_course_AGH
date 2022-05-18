CC = gcc
CFLAGS = -Wall -ggdb

compile:
	make clean
	$(CC) $(CFLAGS) main.c -o main -lpthread

run_example_numbers_test:
	make compile
	./main 5 numbers images/baboon.ascii.pgm negatives/baboon.pgm

run_example_block_test:
	make compile
	./main 5 block images/brain_508.ascii.pgm negatives/brain.pgm

run_test:
	make compile
	./main $(THREADS) $(METHOD) $(INPUT_FILE) $(OUTPUT_FILE)

clean:
	rm -rf main

help:
	@echo "To compile program: make compile"
	@echo "To run example block test: make run_example_block_test."
	@echo "To run example numbers test: run_example_numbers_test."
	@echo "You can also run it with your own parameters. Available images are:"
	@echo "\t baboon.ascii.pgm"
	@echo "\t brain_508.ascii.pgm"
	@echo "\t totem.ascii.pgm"
	@echo "To run your test: make run_test THREADS=arg1 METHOD=arg2 INPUT_FILE=arg3 OUTPUT_FILE=arg4"
	@echo "Where arg1 is number of threads, arg2 is method (numbers / block), arg3 name of input file, arg4 name of output file"
	@echo "To clean all not necessary files: make clean."