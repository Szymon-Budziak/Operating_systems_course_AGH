CC = gcc
CFLAGS = -Wall -ggdb

compile:
	make clean
	$(CC) $(CFLAGS) server.c -o server
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -rf client server server_records.txt

help:
	@echo "To compile all the necessary files for a simple chat: make compile"
	@echo "If you compiled the files now run: ./server and then ./client for how many clients you want"
	@echo "Examples of commands:"
	@echo "	'2ALL hello'"
	@echo "	'2ONE 8 hello client'"
	@echo "	'LIST '"
	@echo "	'STOP '"
	@echo "There has to be a space after LIST and STOP command."
	@echo "To clean all not necessary files: make clean."
