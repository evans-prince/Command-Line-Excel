#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include<stdbool.h>

typedef enum {
    NOT_DECIDED= -1,
    CELL_VALUE_ASSIGNMENT = 0,
    CELL_DEPENDENT_FORMULA = 1,
    SCROLL_COMMAND = 2,
    FUNCTION_CALL = 3,// function like MIN , MAX , STDEV , SUM , AVG , SLEEP
    QUIT_COMMAND = 4,
    INVALID_INPUT = 5
} InputType;

typedef struct Range {
    char *start_cell;
    char *end_cell;
} Range;

struct input {
    InputType input_type;         // Type of input (e.g., cell assignment, formula, command).
    char *raw_input;              // Original raw input string from the user.
    char *cell_reference;         // Cell reference like BB321
    char *value;                  // Value to assign to a cell like A1=20
    char *formula;                // Formula involving dependencies like  B3+2
    char *command;                // Command like scrolling w, a, q
    char *function_name;          // Function name SUM, AVG, STDEV
    Range *range;                 // Parsed range of cells.
    char *arithmetic_expression;  // Arithmetic expression (like "2+3").
};

// constructor function
struct input* create_input(void);//initializes the input struct.

// destructor function
void free_input(struct input* in); // Frees all dynamically allocated memory in the struct.

void parse_input(struct input* in,char error_message[]);// its function is : depending on input type it update our input object
// Like if its type is scroll_command just update command field rest need not to be cahanged.

// Function to find the type of input
InputType find_input_type(const char * raw_input, char error_message[],int msg_size);

bool is_scroll_command(const char * raw_input);
bool is_quit_command(const char * raw_input);
bool is_cell_value_assignment(const char * raw_input);
bool is_function_call(const char * raw_input);
bool is_cell_dependent_formula(const char * raw_input);
#endif

