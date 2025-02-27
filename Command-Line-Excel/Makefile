# compiler and flags
CC = gcc -std=c11
CFLAGS = -Wall -Wextra -g -Iinclude  # Wall and Wextra for Warnings and g for gdb -Iinclude to include header files
LDFLAGS = -lm # lm for linker flags to include func like pow() , etc from math.h library.

# executable output object
TARGET = target/release/spreadsheet

# source files from src and lib
SRC = src/main.c src/display.c src/formula_parser.c src/input_handler.c \
      src/recalculation.c src/scrolling.c src/spreadsheet.c src/command_router.c lib/utils.c

# Convert source file paths to object files inside target/release/
OBJ = $(patsubst %.c, target/release/%.o, $(SRC)) # pattern substitute

# Default target (compile the program)
all: target/release $(TARGET)

# rule for creating target/release directories
target/release:
	mkdir -p target/release

# Linking the final executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

## Rule for compiling .c files to .o (Fixing source path issues)
#target/release/%.o: %.c
#	mkdir -p $(dir $@)  # Ensure target/release structure exists
#	$(CC) $(CFLAGS) -c $< -o $@ # $@ for what we are building , $< for first prereq , $^ for all prereq space sperated

# rule for compiling files inside src and lib
target/release/src/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

target/release/lib/%.o: lib/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# for test rule , @ commands in front of line supresses the command from being printed in terminal
test : $(OBJ)
	@echo "Running tests..."
	@for test_file in tests/*.c; do \
		test_exec=tests/$$(basename $$test_file .c)_exec; \
		$(CC) $(CFLAGS) -o $$test_exec $$test_file $(filter-out target/release/src/main.o, $(OBJ)) $(LDFLAGS); \
		./$$test_exec; \
	done
	
# Rule for testing a specific test file
tests/%: tests/%.c $(OBJ)
	@echo "Running test: $<..."
	$(CC) $(CFLAGS) -o $@ $< $(filter-out target/release/src/main.o, $(OBJ)) $(LDFLAGS)
	./$@
	
# creates report.pdf from LaTeX src.
report: report.tex
	@echo "Creating Latex report using mpdflatex..."
	@pdflatex -interaction=batchmode report.tex # aux file created first
	@pdflatex -interaction=batchmode report.tex  # run twice for references so that pdf can be created now

# Clean up compiled files
clean:
	@rm -rf target/release $(TARGET)  report.aux report.log report.pdf tests/*.dSYM tests/test_*_exec
	
# Run the program interactively with default dimensions (10x10)
run: all
	./$(TARGET) 999 18278

