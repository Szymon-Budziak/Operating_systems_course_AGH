CC = gcc
CFLAGS = -Wall -std=c11 -ggdb
LIB_PATH = ../zad1
TEST_PATH = ../zad2
LIB_NAME = lib
TABLE_SIZE = 3

static:
	make -C $(LIB_PATH) static
	$(CC) $(CFLAGS) $(TEST_PATH)/main.c -o main -I$(LIB_PATH) $(LIB_PATH)/lib.a

shared:
	make -C $(LIB_PATH) shared
	$(CC) $(CFLAGS) $(TEST_PATH)/main.c -o main -I$(LIB_PATH) $(LIB_PATH)/lib.so

dynamic:
	make -C $(LIB_PATH) shared
	$(CC) $(CFLAGS) $(TEST_PATH)/main.c -ldl -D DYNAMIC -o main

small_test:
	@echo "***TEST ON SMALL FILES ***" >> results3b.txt
	./main create_table $(TABLE_SIZE) wc_files $(TEST_PATH)/test_files/small_file_1.txt $(TEST_PATH)/test_files/small_file_2.txt remove_block 0 >> results3b.txt

medium_test:
	@echo "***TEST ON MEDIUM FILES ***" >> results3b.txt
	./main create_table $(TABLE_SIZE) wc_files $(TEST_PATH)/test_files/medium_file_1.txt $(TEST_PATH)/test_files/medium_file_2.txt \
 	$(TEST_PATH)/test_files/medium_file_3.txt remove_block 0 >> results3b.txt

large_test:
	@echo "***TEST ON LARGE FILES ***" >> results3b.txt
	./main create_table $(TABLE_SIZE) wc_files $(TEST_PATH)/test_files/large_file_1.txt $(TEST_PATH)/test_files/small_file_2.txt remove_block 0 >> results3b.txt

different_test:
	@echo "***TEST ON DIFFERENT FILES ***" >> results3b.txt
	./main create_table $(TABLE_SIZE) wc_files $(TEST_PATH)/test_files/small_file_3.txt $(TEST_PATH)/test_files/medium_file_2.txt \
	$(TEST_PATH)/test_files/large_file_3.txt remove_block 0 >> results3b.txt

mult_block_remove:
	@echo "***TEST ON MULTIPLE BLOCK REMOVE ***" >> results3b.txt
	./main create_table $(TABLE_SIZE) wc_files $(TEST_PATH)/test_files/small_file_1.txt wc_files $(TEST_PATH)/test_files/large_file_2.txt \
	remove_block 0 wc_files $(TEST_PATH)/test_files/medium_file_3.txt remove_block 0 >> results3b.txt

run_all_tests:
	make small_test
	make medium_test
	make large_test
	make different_test
	make mult_block_remove

run_static_optimization:
	for opt in O0 O1 O2 O3 Os Ofast ; do \
		make static OPTIMIZATION=-$$opt; \
		make run_all_tests; \
	done

run_shared_optimization:
	for opt in O0 O1 O2 O3 Os Ofast ; do \
		make shared OPTIMIZATION=-$$opt; \
		make run_all_tests; \
	done

run_dynamic_optimization:
	for opt in O0 O1 O2 O3 Os Ofast ; do \
		make dynamic OPTIMIZATION=-$$opt; \
		make run_all_tests; \
	done

clean:
	rm -rf main results3b.txt tmp_file.txtwc
	make -C ../zad3a clean

clean_results:
	> results3b.txt

help:
	@echo "!!To be able to run all possible tests, first compile the program!!"
	@echo "You can compile program by creating static, shared or dynamic library"
	@echo "Create static library: make static"
	@echo "Create shared library: make shared"
	@echo "Create dynamic library: make dynamic"
	@echo "While running tests you can optionally pass TABLE_SIZE='SIZE'"
	@echo "   where 'SIZE' is number which will be the size of table, default value is 3"
	@echo "To run test on small files: make small_test"
	@echo "To run test on medium files: make medium_test"
	@echo "To run test on large files: make large_test"
	@echo "To run test on different files: make different_test"
	@echo "To run multiple block removes: make mult_block_remove"
	@echo "To run all tests at once: make run_all_tests"
	@echo "To clean all not necessary files (it cleans zad1, zad2 and zad3 directory): make clean"
	@echo "To clean content of results3b.txt file: make clean_results"