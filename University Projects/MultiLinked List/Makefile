CC=gcc
FLAGS=-Wall -Werror
SRCS = mtll.c main.c
OBJS = mtll.o main.o
MEMCHECKER = valgrind
MEMFLAGS = --leak-check=full
TARGET=mtll

all: $(TARGET)

# Run the commands to compile the program
build: $(TARGET)
	$(CC) $(FLAGS) $(SRCS) -o $(TARGET)

$(TARGET): $(OBJS)
	@$(CC) $(FLAGS) $^ -o $@

%.o: %.c mtll.h
	@$(CC) -c $(FLAGS) $< -o $@

run: build
	@./$(TARGET)

mem_run: build
	@$(MEMCHECKER) $(MEMFLAGS) ./$(TARGET)

.PHONY: tests
tests:
	@echo "Available tests: \nCorner Test 1\nCorner Test 2\nCorner Test 3\nLarge Test\nNormal Test"

.PHONY: run_tests
run_tests: $(TARGET)
	chmod +x tests.sh
	@./tests.sh

.PHONY: clean
clean:
	@rm -f $(OBJS) $(TARGET)
	@rm -f main
