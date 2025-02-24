# Compiler and Flags
CC = gcc -std=c11
CFLAGS = -Wall -Wextra -g -Iinclude  # Include header files from include/
LDFLAGS = -lm

# Output executable
TARGET = target/release/spreadsheet

# Source files from src/ and lib/
SRC = src/main.c src/display.c src/formula_parser.c src/input_handler.c \
      src/recalculation.c src/scrolling.c src/spreadsheet.c src/command_router.c lib/utils.c

# Convert source file paths to object files inside target/release/
OBJ = $(patsubst %.c, target/release/%.o, $(SRC))

# Default target (compile the program)
all: target/release $(TARGET)

# Create the target/release directory if it doesn’t exist
target/release:
	mkdir -p target/release

# Linking the final executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

# Rule for compiling .c files to .o (Fixing source path issues)
target/release/%.o: %.c
	mkdir -p $(dir $@)  # Ensure target/release structure exists
	$(CC) $(CFLAGS) -c $< -o $@

# Fix for source files being inside 'src/' or 'lib/'
target/release/src/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

target/release/lib/%.o: lib/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up compiled files
clean:
	rm -rf target/release

# Run the program (Optional)
run: all
	./$(TARGET)

# Run Valgrind to check for memory leaks
valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET) $(ARGS)
