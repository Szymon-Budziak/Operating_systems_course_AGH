CC = gcc
CFLAGS = -Wall -ggdb

compile:
	make clean
	$(CC) $(CFLAGS) main.c -o main -lpthread

run:
	make compile
	./main

clean:
	rm -rf main

help:
	@echo "To compile program: make compile."
	@echo "To run program: make run."
	@echo "To clean all not necessary files: make clean."