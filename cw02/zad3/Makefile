CC = gcc
CFLAGS = -Wall -ggdb

run_normal_test:
	$(CC) $(CFLAGS) read_directories.c -o read_directories
	./read_directories /home/szymon/Studia_AGH/Semestr_4

run_nftw_test:
	$(CC) $(CFLAGS) nftw_read_directories.c -o nftw_read_directories
	./nftw_read_directories /home/szymon/Studia_AGH/Semestr_4

run_valgrind_normal_test:
	$(CC) $(CFLAGS) read_directories.c -o read_directories
	valgrind -s --leak-check=full --show-leak-kinds=all ./read_directories /home/szymon/Studia_AGH/Semestr_4

run_valgrind_nftw_test:
	$(CC) $(CFLAGS) nftw_read_directories.c -o nftw_read_directories
	valgrind -s --leak-check=full --show-leak-kinds=all ./nftw_read_directories /home/szymon/Studia_AGH/Semestr_4

clean:
	rm -rf read_directories nftw_read_directories