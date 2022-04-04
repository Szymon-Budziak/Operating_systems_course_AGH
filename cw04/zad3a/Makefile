CC = gcc
CFLAGS = -Wall -g
ARG="KILL"

compile_programs:
	$(CC) $(CFLAGS) catcher.c -o catcher
	$(CC) $(CFLAGS) sender.c -o sender

run_test: compile_programs
	./catcher $(ARG) &
	./sender `pidof catcher` 100 $(ARG)

clean:
	rm -rf catcher sender

help:
	@echo "To compile catcher and sender: make compile_programs"
	@echo "In order to test catcher and sender you have to run the command above first and then manually entry the parameters"
	@echo "You can also test it by not passing parameters manually. To do so: make run_test ARG='arg', where 'arg' is yours parameter"
	@echo "\t if you don not pass arg, the default value will be set to ARG: 'KILL'"
	@echo "To clean all not necessary files: make clean"