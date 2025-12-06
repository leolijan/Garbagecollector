<<<<<<< HEAD
CC = gcc
CFLAGS = -g3 -Wall
=======
OS := $(shell uname)
#different command depending on the operating system.(mac or not mac)
ifeq ($(OS),Darwin)
  CC = /opt/homebrew/opt/gcc/bin/gcc-14
  CFLAGS = -Wall -g -fprofile-arcs -ftest-coverage -I/opt/homebrew/Cellar/cunit/2.1-3/include
  LDFLAGS = -lcunit -L/opt/homebrew/opt/gcc/lib/gcc/14
else
  CC = gcc
  CFLAGS = -Wall -g -fprofile-arcs -ftest-coverage
  LDFLAGS = -lcunit -lgcov
endif
>>>>>>> footer

store: common.o utils.o hash_table-dynamic.o linked_list.o store.o stor_functions.o gc.o
	$(CC) $(CFLAGS) hash_table-dynamic.o common.o linked_list.o store.o stor_functions.o utils.o gc.o -o store

store_tests: common.o utils.o hash_table-dynamic.o linked_list.o stor_functions.o store_functions_tests.o 
	$(CC) $(CFLAGS) hash_table-dynamic.o common.o linked_list.o store_functions_tests.o stor_functions.o utils.o -o store_tests -lcunit

store_functions_tests.o: store_functions_tests.c hash_table-dynamic.h iterator.h linked_list.h common.h stor_functions.h ../../src/gc.h
	$(CC) -c $(CFLAGS) store_functions_tests.c

store.o: store.c hash_table-dynamic.h iterator.h linked_list.h common.h stor_functions.h ../../src/gc.h
	$(CC) -c $(CFLAGS) store.c

stor_functions.o: common.h utils.h iterator.h linked_list.h hash_table-dynamic.h stor_functions.h ../../src/gc.h
	$(CC) -c $(CFLAGS) stor_functions.c

utils.o: utils.h
	$(CC) -c $(CFLAGS) utils.c

hash_table-dynamic.o: hash_table-dynamic.c hash_table-dynamic.h linked_list.h ../../src/gc.h
	$(CC) -c $(CFLAGS) hash_table-dynamic.c

common.o: common.h
	$(CC) -c $(CFLAGS) common.c 

linked_list.o: linked_list.c linked_list.h common.h ../../src/gc.h
	$(CC) -c $(CFLAGS) linked_list.c

gc.o: ../../src/gc.h
	$(CC) -c $(CFLAGS) ../../src/gc.c


clean:
	rm -f *.o store store_tests
