CC = gcc
CFLAGS = -Wall -ggdb
PROD = 5
CONS = 5

compile:
	$(CC) $(CFLAGS) consumer.c -o consumer
	$(CC) $(CFLAGS) producer.c -o producer
	$(CC) $(CFLAGS) main.c -o main

run_test:
	make compile
	./main 5 5

run_your_test:
	make compile
	./main $(PROD) $(CONS)

run_valgrind_test:
	make compile
	valgrind -s --leak-check=full --show-leak-kinds=all ./main $(PROD) $(CONS)

test:
	make compile
	make run_your_test PROD=5 CONS=1
	make run_your_test PROD=1 CONS=5
	make run_your_test PROD=5 CONS=5

clean:
	rm -rf main consumer producer fifo consumer_files/consumer_result.txt

help:
	@echo "To compile and run example test: make run_test. Default arguments are 5 5."
	@echo "To compile and run test with your arguments: make run_your_test PROD='arg1' CONS='arg2'. If you do not pass the arguments, the default ones will be used (5 5)."
	@echo "\t(where PROD is the number of producers and CONS is the number of consumers)."
	@echo "To compile and run to check if there is any data leakage: make run_valgrind_test. You can also pass here your arguments or default ones will be used as above (5 5)."
	@echo "To run multiple tests on multiple arguments: make test."
	@echo "To clean all not necessary files: make clean."