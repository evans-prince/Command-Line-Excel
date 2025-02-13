# Compiler and Flags
CC = gcc -std=c11
CFLAGS = -Wall -Wextra -g -Iinclude  # Include header files from include/
LDFLAGS = -lm

# Output executable
TARGET = sheet

# Source files from src/ and lib/
SRC = src/main.c src/display.c src/formula_parser.c src/input_handler.c \
      src/recalculation.c src/scrolling.c src/spreadsheet.c src/command_router.c lib/utils.c

# Object files (converting .c to .o)
OBJ = $(SRC:.c=.o)

# Default target (compile the program)
all: $(TARGET)

# Linking the final executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

# Rule for compiling .c files to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up compiled files
clean:
	rm -f $(OBJ) $(TARGET)

# Run the program (Optional)
run: all
	./$(TARGET)

# Run Valgrind to check for memory leaks
valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET) $(ARGS)

