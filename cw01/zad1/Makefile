CFLAGS = -Wall -std=c11 -ggdb -O2

all: static shared clean

static:
	gcc $(CFLAGS) -c lib.c
	ar rcs lib.a lib.o

shared:
	gcc $(CFLAGS) -fPIC -c lib.c
	gcc -shared -fPIC -o lib.so lib.o

clean:
	rm -rf *.o *.a *.so *.gch lib tmp_file.txt

help:
	@echo "To compile and create static library: make static"
	@echo "To compile and create shared library: make shared"
	@echo "To clean all .o .a and temporary files: make clean"