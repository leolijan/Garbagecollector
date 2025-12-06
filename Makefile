OS := $(shell uname)
#different command depending on the operating system.(mac or not mac)
ifeq ($(OS),Darwin)
  CC = /opt/homebrew/opt/gcc/bin/gcc-14
  CFLAGS = -Wall -g -fprofile-arcs -ftest-coverage -I/opt/homebrew/Cellar/cunit/2.1-3/include
  LDFLAGS = -lcunit -L/opt/homebrew/opt/gcc/lib/gcc/14
  MEM_TEST_CMD = leaks --atExit --
  PROFILING_CMD = llvm-profdata merge -o profdata.prof
  COV_CMD = llvm-cov report
else
  CC = gcc
  CFLAGS = -Wall -g -fprofile-arcs -ftest-coverage
  LDFLAGS = -lcunit -lgcov
  MEM_TEST_CMD = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
  PROFILING_CMD = gprof
  COV_CMD = gcov
endif


# Object files
UTILS_OBJ = src/utils.c
HEADER_OBJ = src/header/bit_manip.o src/header/format_str.o src/header/header.o src/header/bit_vector.o src/header/raw_layout.o src/header/footer.o
GC_OBJ = src/gc.o
TRAVERSE_STACK_OBJ = src/traverse_stack.o

# For Github CI
check: all test

gc:  $(UTILS_OBJ) $(HEADER_OBJ) $(GC_OBJ) $(TRAVERSE_STACK_OBJ) 
	$(CC) $(CFLAGS) -o $@ $^ 
  
# Targets and rules
all: format_str_test bit_vector_test raw_layout_test forward_adress_test header_test bit_manip_test gc_test traverse_stack_test footer_test


<<<<<<< HEAD
# HEADER OBJECTS
format_str_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/format_str_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	./format_str_test
bit_vector_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/bit_vector_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	-./bit_vector_test
raw_layout_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/raw_layout_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	./raw_layout_test
forward_adress_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/forward_adress_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	./forward_adress_test
header_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/header_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	./header_test
bit_manip_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/bit_manip_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	./bit_manip_test
gc_test: $(UTILS_OBJ) $(HEADER_OBJ) $(TRAVERSE_STACK_OBJ) $(GC_OBJ) test/gc_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	-./gc_test
traverse_stack_test: $(UTILS_OBJ) $(HEADER_OBJ) $(TRAVERSE_STACK_OBJ) $(GC_OBJ) test/traverse_stack_test.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	./traverse_stack_test
=======
>>>>>>> footer
# Compiles object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ======  TEST MENU ===========
test: format_str_test bit_vector_test raw_layout_test forward_adress_test header_test bit_manip_test gc_test traverse_stack_test footer_test
	@for i in $$(seq 1 10); do echo ""; done
	@echo "Test Menu:"
	@echo "(1) format_str_test"
	@echo "(2) bit_vector_test"
	@echo "(3) raw_layout_test"
	@echo "(4) header_test"
	@echo "(5) bit_manip_test"
	@echo "(6) gc_test"
	@echo "(7) forward_address"
	@echo "(8) traverse_stack_test"
	@echo "(9) footer_test"
	@echo "Enter test number: "
	@read choice; \
	case $$choice in \
		1) ./format_str_test ;; \
		2) ./bit_vector_test ;; \
		3) ./raw_layout_test ;; \
		4) ./header_test ;; \
		5) ./bit_manip_test ;; \
		6) ./gc_test ;; \
		7) ./forward_adress_test ;; \
		8) ./traverse_stack_test ;; \
		9) ./footer_test ;; \
		*) echo "Invalid choice" ;; \
	esac

# ======= TESTS ==========

format_str_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/format_str_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
bit_vector_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/bit_vector_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
raw_layout_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/raw_layout_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
forward_adress_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/forward_adress_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
header_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/header_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
bit_manip_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/bit_manip_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
gc_test: $(UTILS_OBJ) $(HEADER_OBJ) $(TRAVERSE_STACK_OBJ) $(GC_OBJ) test/gc_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
traverse_stack_test: $(UTILS_OBJ) $(HEADER_OBJ) $(TRAVERSE_STACK_OBJ) $(GC_OBJ) test/traverse_stack_test.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
footer_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/footer_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Run tests
test_all: format_str_test bit_vector_test raw_layout_test forward_adress_test header_test bit_manip_test gc_test traverse_stack_test footer_test
	./format_str_test
	./bit_vector_test
	./raw_layout_test
	./header_test
	./bit_manip_test
	./gc_test

# ======  MEM TEST ===========
mem_test:  format_str_test bit_vector_test raw_layout_test forward_adress_test header_test bit_manip_test gc_test traverse_stack_test footer_test
	@echo "Memory Test Menu:"
	@echo "(1) gc_test"
	@echo "(2) bit_vector_test"
	@echo "(3) raw_layout_test"
	@echo "(4) header_test"
	@echo "(5) bit_manip_test"
	@echo "(6) forward_adress_test"
	@echo "(7) footer_test"
	@echo "Enter program number: "
	@read choice; \
	case $$choice in \
		1) $(MEM_TEST_CMD) ./gc_test ;; \
		2) $(MEM_TEST_CMD) ./bit_vector_test ;; \
		3) $(MEM_TEST_CMD) ./raw_layout_test ;; \
		4) $(MEM_TEST_CMD) ./header_test ;; \
		5) $(MEM_TEST_CMD) ./bit_manip_test ;; \
		6) $(MEM_TEST_CMD) ./forward_adress_test ;; \
		7) $(MEM_TEST_CMD) ./footer_test ;; \
		*) echo "Invalid choice" ;; \
	esac


PROFILE_DIR = profiling_results
create_profile_dir:
	@mkdir -p $(PROFILE_DIR)

# ==== Profiling =====
profiling: create_profile_dir
	@echo "Profiling Menu:"
	@echo "(1) gc_test"
	@echo "(2) bit_vector_test"
	@echo "(3) raw_layout_test"
	@echo "(4) header_test"
	@echo "(5) bit_manip_test"
	@echo "(6) forward_address_test"
	@echo "(7) footer_test"
	@echo "Enter program number: "
	@read choice; \
	case $$choice in \
		1) $(MAKE) profiling_gc_test ;; \
		2) $(MAKE) profiling_bit_vector_test ;; \
		3) $(MAKE) profiling_raw_layout_test ;; \
		4) $(MAKE) profiling_header_test ;; \
		5) $(MAKE) profiling_bit_manip_test ;; \
		6) $(MAKE) profiling_forward_address_test ;; \
		7) $(MAKE) profiling_footer_test ;; \
		*) echo "Invalid choice" ;; \
	esac

# Profiling rules for individual tests
profiling_gc_test: $(UTILS_OBJ) $(HEADER_OBJ) $(TRAVERSE_STACK_OBJ) $(GC_OBJ) test/gc_test.o
	$(CC) $(CFLAGS) -pg -o $@ $^ $(LDFLAGS)  
	$(PROFILING_CMD) ./profiling_gc_test gmon.out > $(PROFILE_DIR)/gc_test.prof  

profiling_bit_vector_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/bit_vector_test.o
	$(CC) $(CFLAGS) -pg -o $@ $^ $(LDFLAGS)  
	$(PROFILING_CMD) ./bit_vector_test gmon.out > $(PROFILE_DIR)/bit_vector_test.prof 

profiling_raw_layout_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/raw_layout_test.o
	$(CC) $(CFLAGS) -pg -o $@ $^ $(LDFLAGS)  
	$(PROFILING_CMD) ./raw_layout_test gmon.out > $(PROFILE_DIR)/raw_layout_test.prof 

profiling_header_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/header_test.o
	$(CC) $(CFLAGS) -pg -o $@ $^ $(LDFLAGS)  
	$(PROFILING_CMD) ./header_test gmon.out > $(PROFILE_DIR)/header_test.prof  

profiling_bit_manip_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/bit_manip_test.o
	$(CC) $(CFLAGS) -pg -o $@ $^ $(LDFLAGS)  
	$(PROFILING_CMD) ./bit_manip_test gmon.out > $(PROFILE_DIR)/bit_manip_test.prof 

profiling_forward_address_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/forward_address_test.o
	$(CC) $(CFLAGS) -pg -o $@ $^ $(LDFLAGS)  
	$(PROFILING_CMD) ./forward_address_test gmon.out > $(PROFILE_DIR)/forward_address_test.prof 

profiling_footer_test: $(UTILS_OBJ) $(HEADER_OBJ) test/header/footer_test.o
	$(CC) $(CFLAGS) -pg -o $@ $^ $(LDFLAGS)  
	$(PROFILING_CMD) ./footer_test gmon.out > $(PROFILE_DIR)/footer_test.prof  

# ========== Demo project ==========

Project_Folder = demo/inlupp2/
store: $(UTILS_OBJ) $(HEADER_OBJ) $(TRAVERSE_STACK_OBJ) $(GC_OBJ) \
	   $(Project_Folder)common.o $(Project_Folder)utils.o \
	   $(Project_Folder)hash_table-dynamic.o $(Project_Folder)linked_list.o store.o \
	   $(Project_Folder)stor_functions.o $(Project_Folder)gc.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	./store

store_tests: $(UTILS_OBJ) $(HEADER_OBJ) \
			 $(TRAVERSE_STACK_OBJ) $(GC_OBJ) \
			 $(Project_Folder)common.o $(Project_Folder)utils.o $(Project_Folder)hash_table-dynamic.o \
			 $(Project_Folder)linked_list.o $(Project_Folder)stor_functions.o $(Project_Folder)store_functions_tests.o 
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	-./store_tests

store_functions_tests.o: $(UTILS_OBJ) $(HEADER_OBJ) $(TRAVERSE_STACK_OBJ) $(GC_OBJ) $(Project_Folder)store_functions_tests.c \ 
						 $(Project_Folder)hash_table-dynamic.h $(Project_Folder)iterator.h $(Project_Folder)linked_list.h \
						 $(Project_Folder)common.h $(Project_Folder)stor_functions.h $(Project_Folder)../../src/gc.h
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

store.o: $(UTILS_OBJ) $(HEADER_OBJ) $(TRAVERSE_STACK_OBJ) $(GC_OBJ) $(Project_Folder)store.c $(Project_Folder)hash_table-dynamic.h \
					  $(Project_Folder)iterator.h $(Project_Folder)linked_list.h $(Project_Folder)common.h $(Project_Folder)stor_functions.h \
					  $(Project_Folder)../../src/gc.h
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

stor_functions.o: $(UTILS_OBJ) $(HEADER_OBJ) $(TRAVERSE_STACK_OBJ) $(GC_OBJ) $(Project_Folder)common.h \
				  $(Project_Folder)utils.h $(Project_Folder)iterator.h $(Project_Folder)linked_list.h \
				  $(Project_Folder)hash_table-dynamic.h $(Project_Folder)stor_functions.h $(Project_Folder)../../src/gc.h
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

utils.o: $(UTILS_OBJ) $(HEADER_OBJ) $(TRAVERSE_STACK_OBJ) $(GC_OBJ) $(Project_Folder)utils.h
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

hash_table-dynamic.o: $(UTILS_OBJ) $(HEADER_OBJ) $(TRAVERSE_STACK_OBJ) $(GC_OBJ) $(Project_Folder)hash_table-dynamic.c $(Project_Folder)hash_table-dynamic.h $(Project_Folder)linked_list.h $(Project_Folder)../../src/gc.h
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

common.o: $(UTILS_OBJ) $(HEADER_OBJ) $(TRAVERSE_STACK_OBJ) $(GC_OBJ) $(Project_Folder)common.h
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 

linked_list.o: $(UTILS_OBJ) $(HEADER_OBJ) $(TRAVERSE_STACK_OBJ) $(GC_OBJ) $(Project_Folder)linked_list.c $(Project_Folder)linked_list.h $(Project_Folder)common.h $(Project_Folder)../../src/gc.h
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

gc.o: $(UTILS_OBJ) $(HEADER_OBJ) $(TRAVERSE_STACK_OBJ) $(GC_OBJ) $(Project_Folder)../../src/gc.h
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)




# ====== UTILS ===========
# Clean rule
clean:
	rm -f \
        $(FORMAT_STR_OBJ) \
        format_str_test \
        bit_vector_test \
        raw_layout_test \
        forward_adress_test \
        header_test \
        bit_manip_test \
        gc_test \
        *.gcda \
        *.gcno \
        test/test_format_str \
        test/*.gcda \
        test/*.gcno \
        test/*.gcov \
        test/*.o \
        test/header/*.o \
        test/header/*.gcda \
        test/header/*gcno \
        test/.fuse* \
        src/*.gcno \
        src/*.gcda \
        src/*.gcov \
        src/*.o \
        src/header/*.o \
        src/header/*.gcda \
        src/header/*gcno \
		$(Project_Folder)store \
		$(Project_Folder)store_tests
	rm -rf *.dSYM



<<<<<<< HEAD
# Run Valgrind
valgrind: gc_test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./gc_test

=======
>>>>>>> footer
# Generate GCOV coverage report
gcov: format_str_test
	./format_str_test
	@gcov src/format_str.c
	@echo "GCOV coverage generated."

.PHONY: all test clean valgrind gcov


