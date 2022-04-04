CC = gcc
CFLAGS = -Wall -ggdb
ARGS =

compile_programs:
	$(CC) $(CFLAGS) catcher.c -o catcher
	$(CC) $(CFLAGS) sender.c -o sender

clean:
	rm -rf catcher sender

help:
	@echo "To compile catcher and sender: make compile_programs"
	@echo "In order to test catcher and sender you have to run the command above first and then manually entry the parameters"
	@echo "To clean all not necessary files: make clean"