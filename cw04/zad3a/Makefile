CC = gcc
CFLAGS = -Wall -g
ARG1 = 100
ARG2 = "KILL"

compile_programs:
	$(CC) $(CFLAGS) catcher.c -o catcher
	$(CC) $(CFLAGS) sender.c -o sender

run_test: compile_programs
	./catcher $(ARG) &
	./sender `pidof catcher` $(ARG1) $(ARG2)

clean:
	rm -rf catcher sender

help:
	@echo "To compile catcher and sender: make compile_programs"
	@echo "In order to test catcher and sender you can run the command above first and then manually enter the parameters"
	@echo "You can also test it by not passing parameters manually. To do so: make run_test ARG1='arg1' ARG2='arg2', where 'arg1' and 'arg2 are your parameters"
	@echo "\t Both parameters are optional, default value of 'arg1' is 100 and 'arg2' is 'KILL'"
	@echo "To clean all not necessary files: make clean"