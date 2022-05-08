CC = gcc
CFLAGS = -Wall -ggdb

compile:
	make clean
	@touch oven
	@touch table
	$(CC) $(CFLAGS) cook.c -o cook
	$(CC) $(CFLAGS) supplier.c -o supplier
	$(CC) $(CFLAGS) main.c -o main


clean:
	rm -rf main cook supplier table oven

help:
	@echo "To compile all the necessary files: make compile"
	@echo "If you compiled the files now run: ./main with 2 parameters"
	@echo "The first parameter in above run is the number of cooks and the second one is the number of suppliers"
	@echo "Example run of main: ./main 4 5"
	@echo "To clean all not necessary files: make clean."