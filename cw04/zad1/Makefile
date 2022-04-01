CC = gcc
CFLAGS = -Wall -ggdb

run_test_fork_ignore:
	$(CC) $(CFLAGS) test_fork.c -o test_fork
	./test_fork "ignore"

run_test_fork_handler:
	$(CC) $(CFLAGS) test_fork.c -o test_fork
	./test_fork "handler"

run_test_fork_mask:
	$(CC) $(CFLAGS) test_fork.c -o test_fork
	./test_fork "mask"

run_test_fork_pending:
	$(CC) $(CFLAGS) test_fork.c -o test_fork
	./test_fork "pending"

run_test_exec_ignore:
	$(CC) $(CFLAGS) test_exec.c -o test_exec
	$(CC) $(CFLAGS) child.c -o child
	./test_exec "ignore"

run_test_exec_mask:
	$(CC) $(CFLAGS) test_exec.c -o test_exec
	$(CC) $(CFLAGS) child.c -o child
	./test_exec "mask"

run_test_exec_pending:
	$(CC) $(CFLAGS) test_exec.c -o test_exec
	$(CC) $(CFLAGS) child.c -o child
	./test_exec "pending"

clean:
	rm -rf test_fork test_exec child

help:
	@echo "To compile and run test with fork and 'ignore' argument: make run_test_fork_ignore"
	@echo "To compile and run test with fork and 'handler' argument: make run_test_fork_handler"
	@echo "To compile and run test with fork and 'mask' argument: make run_test_fork_mask"
	@echo "To compile and run test with fork and 'pending' argument: make run_test_fork_pending"
	@echo "To compile and run test with exec and 'ignore' argument: make run_test_exec_ignore"
	@echo "To compile and run test with exec and 'mask' argument: make run_test_exec_mask"
	@echo "To compile and run test with exec and 'pending' argument: make run_test_exec_pending"
	@echo "To clean all not necessary files: make clean"