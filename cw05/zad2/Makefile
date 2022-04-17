CC = gcc
CFLAGS = -Wall -ggdb

run_sort_sender_test:
	$(CC) $(CFLAGS) main.c -o main
	./main "sender"

run_sort_date_test:
	$(CC) $(CFLAGS) main.c -o main
	./main "date"

run_send_test:
	$(CC) $(CFLAGS) main.c -o main
	./main $(ARG1) "$(ARG2)" "$(ARG3)"

clean:
	rm -rf main

help:
	@echo "To compile and run sort sender test: make run_sort_sender_test. This test has 'sender' as a default argument."
	@echo "To compile and run sort date test: make run_sort_date_test. This test has 'date' as a default argument."
	@echo "To compile and run send test: make run_send_test ARG1='arg1' ARG2='arg2' ARG3='arg3'. You have to pass all of this arguments."
	@echo "\tFirst arguments is the sender email, second is title and last is the content."
	@echo "To clean all not necessary files: make clean"