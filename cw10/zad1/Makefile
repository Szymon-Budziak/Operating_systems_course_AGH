CC = gcc
CFLAGS = -Wall -ggdb -pedantic -g -pthread

all:
	make clean
	make server
	make client
	./server 6666 socket

server:
	$(CC) $(CFLAGS) server.c -o server

client:
	$(CC) $(CFLAGS) client.c -o client

run_server:
	./server 6666 socket

client_1_local:
	./client Szymon local socket

client_2_local:
	./client Piotr local socket

client_1_host:
	./client Szymon xxx 6666

client_2_host:
	./client Piotr xxx 6666

your_client:
	./client $(ARG1) $(ARG2) $(ARG3)

clean:
	rm -rf client server socket

help:
	@echo "To compile all files and run the server: make all."
	@echo "To just compile the server: make server"
	@echo "To just compile the client: make client"
	@echo "If you have your server running, then you can run some example clients: make client_1_local,"
	@echo "\tmake client_2_local, make client_1_host, make client_2_host"
	@echo "You can also create your own clients: make your_client ARG1=arg1 ARG2=arg2 ARG3=arg3 where"
	@echo "\targ1 is the name of your client, arg2 is the way to connect to the server and"
	@echo "\targ3 is server address"
	@echo "To clean all not necessary files: make clean."