CC = gcc
CFLAGS = -Wall -std=c11 -ggdb
LIB_PATH = ../zad1
TABLE_SIZE = 3

static:
	make -C ../zad1 static
	$(CC) $(CFLAGS) main.c -o main -I$(LIB_PATH) $(LIB_PATH)/lib.a

small_test:
	@echo "***TEST ON SMALL FILES ***" | tee -a raport2.txt
	./main create_table $(TABLE_SIZE) wc_files test_files/small_file_1.txt test_files/small_file_2.txt remove_block 0 | tee -a raport2.txt

medium_test:
	@echo "***TEST ON MEDIUM FILES ***" | tee -a raport2.txt
	./main create_table $(TABLE_SIZE) wc_files test_files/medium_file_1.txt test_files/medium_file_2.txt test_files/medium_file_3.txt \
	remove_block 0 | tee -a raport2.txt

large_test:
	@echo "***TEST ON LARGE FILES ***" | tee -a raport2.txt
	./main create_table $(TABLE_SIZE) wc_files test_files/large_file_1.txt test_files/small_file_2.txt remove_block 0 | tee -a raport2.txt

different_test:
	@echo "***TEST ON DIFFERENT FILES ***" | tee -a raport2.txt
	./main create_table $(TABLE_SIZE) wc_files test_files/small_file_3.txt test_files/medium_file_2.txt test_files/large_file_3.txt \
	remove_block 0 | tee -a raport2.txt

mult_block_remove_test:
	@echo "***TEST ON MULTIPLE BLOCK REMOVE ***" | tee -a raport2.txt
	./main create_table $(TABLE_SIZE) wc_files test_files/small_file_1.txt wc_files test_files/large_file_2.txt remove_block 0 \
	wc_files test_files/medium_file_3.txt remove_block 0 remove_block 1 | tee -a raport2.txt

data_leakage_test:
	@echo "***TEST ON DATA LEAKAGE***" | tee -a raport2.txt
	valgrind -s --leak-check=full --show-leak-kinds=all ./main create_table $(TABLE_SIZE) wc_files test_files/small_file_1.txt test_files/medium_file_2.txt \
	test_files/large_file_1.txt remove_block 0 | tee -a raport2.txt


run_all_tests:
	rm -rf raport2.txt
	make small_test
	make medium_test
	make large_test
	make different_test
	make mult_block_remove_test
	make data_leakage_test

clean:
	make -C ../zad1 clean
	rm -rf *.o *.a *.so *.gch main raport2.txt tmp_file.txt tmp_file.txtwc

clean_raport:
	> raport2.txt

help:
	@echo "!!To be able to run all possible tests, first compile the program: make static!!"
	@echo "You can optionally pass TABLE_SIZE='SIZE', where 'SIZE' is number which will be the size of table, default value is 3"
	@echo "To run test on small files: make small_test"
	@echo "To run test on medium files: make medium_test"
	@echo "To run test on large files: make large_test"
	@echo "To run test on different files: make different_test"
	@echo "To run multiple block removes: make mult_block_remove_test"
	@echo "To check if there are any data leakage: run data_leakage_test"
	@echo "To run all tests at once: make run_all_tests"
	@echo "To clean all not necessary files (it cleans zad1 and zad2 directory): make clean"
	@echo "To clean content of raport2.txt file: make clean_raport"