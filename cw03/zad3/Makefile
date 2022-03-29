CC = gcc
CFLAGS = -Wall -ggdb
S_PATH = /home/szymon/Studia_AGH
SUBSTRING = "Try to find me :)"
DEPTH = 10

run_example_test:
	$(CC) $(CFLAGS) main.c -o main
	./main /home/szymon/Studia_AGH/Semestr_4 "Try to find me :)" 10

run_test:
	$(CC) $(CFLAGS) main.c -o main
	./main $(S_PATH) $(SUBSTRING) $(DEPTH)

run_valgrind_test:
	$(CC) $(CFLAGS) main.c -o main
	valgrind -s --leak-check=full --show-leak-kinds=all ./main /home/szymon/Studia_AGH/Semestr_4 "Try to find me :)" 10

clean:
	rm -rf main

help:
	@echo "To compile and run example test with substring 'see' and depth 10: make run_example_test"
	@echo "To compile and run test with optional argument: make run_test S_PATH='ARG1' SUBSTRING='ARG2' DEPTH='ARG3'"
	@echo "\t (where 'ARG1' path to the folder you want to start, 'ARG2' is a substring that you are looking for and 'ARG3' is max depth)"
	@echo "To compile and run test to check if there are any data leakage: make run_valgrind_test"
	@echo "To clean all not necessary files: make clean"