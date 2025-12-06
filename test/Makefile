CC       = gcc
CFLAGS   = -Wall -g -I../src
LIBS     = -lcunit
TARGET   = test_format_str
TARGET_TRAVERSE = test_traverse_stack
SRC      = format_str_test.c ../src/format_str.c
SRC_TRAVERSE = traverse_stack_test.c ../src/traverse_stack.c ../src/gc.c ../src/format_str.c ../src/header.c
OBJ      = $(SRC:.c=.o)
OBJ_TRAVERSE = $(SRC_TRAVERSE:.c=.o)

all: $(TARGET) $(TARGET_TRAVERSE)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(TARGET_TRAVERSE): $(OBJ_TRAVERSE)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test_mem_format_str: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

test_format_str_run: $(TARGET)
	./$(TARGET)

test_mem_traverse_stack: $(TARGET_TRAVERSE)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET_TRAVERSE)

test_traverse_stack_run: $(TARGET_TRAVERSE)
	./$(TARGET_TRAVERSE)

clean:
	rm -f $(TARGET) $(TARGET_TRAVERSE) $(OBJ) $(OBJ_TRAVERSE)
