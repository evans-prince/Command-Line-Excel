#include "../include/display.h"
#include "../include/formula_parser.h"
#include "../include/input_handler.h"
#include "../include/recalculation.h"
#include "../include/scrolling.h"
#include "../include/spreadsheet.h"
#include "../include/utils.h"

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include <limits.h>

struct input* create_input(void){
    struct input *new_input =(struct input *) malloc(sizeof(struct input));
    
    if (new_input == NULL) {
        fprintf(stderr, "Memory allocation failed for input struct.\n");
        return NULL;
    }
    
    // now just initialising the input object
    //like setting all values to null and input type to undecided
    new_input->input_type=NOT_DECIDED;
    new_input->raw_input=NULL;
    new_input->value=NULL;
    new_input->formula=NULL;
    new_input->function_name=NULL;
    new_input->command=NULL;
    new_input->arithmetic_expression=NULL;
    new_input->cell_reference=NULL;
    new_input->range=NULL;
    
    return new_input;
}

// Function to free the memory allocated to the input struct
void free_input(struct input* in){// ALL THIS FUNCTION DO IS JUST FREE THE MEMORY ALLOCATED
    // BY THE MALLAC TO STRUCT OBJECT INPUT
    
    if (in == NULL) {
        return;
    }
    
    free(in->raw_input);
    in->raw_input=NULL;
    free(in->cell_reference);
    in->cell_reference=NULL;
    free(in->value);
    in->value=NULL;
    free(in->formula);
    in->formula=NULL;
    free(in->command);
    in->command=NULL;
    free(in->function_name);
    in->function_name=NULL;
    
    if (in->range != NULL) {
        free(in->range->start_cell);
        free(in->range->end_cell);
        free(in->range);
    }
    free(in->arithmetic_expression);
    
    free(in);
}

// Function to find the type of input
InputType find_input_type(char * raw_input){
    // please don't mess with raw_input
    
    //    NOT_DECIDED= -1,
    //    CELL_VALUE_ASSIGNMENT = 0,
    //    CELL_DEPENDENT_FORMULA = 1,
    //    SCROLL_COMMAND = 2,
    //    FUNCTION_CALL = 3, function like MIN , MAX , STDEV , SUM , AVG , SLEEP
    //    QUIT_COMMAND = 4,
    //    INVALID_INPUT = 5
    
    // note: before starting remove spaces from raw_input
    remove_space(raw_input);
    // if input is empty or null then not decided
    if (raw_input == NULL || strlen(raw_input) == 0) {
        return INVALID_INPUT; // Empty input is invalid
    }
    
    if (is_scroll_command(raw_input)) {
        // if it contains only one char and is one of them w ,a ,s ,d then scroll
        return SCROLL_COMMAND;
    }
    if (is_quit_command(raw_input)) {
        // if it contains only one char and is q then quit command
        return QUIT_COMMAND;
    }
    if (is_cell_value_assignment(raw_input)) {
        // if it contains at most  3 upper case letter at most 3 digits then '=' then some value or value operand value
        //we can say it is cell value assignment , but we are also dealing like direct airthmatic operations here only
        return CELL_VALUE_ASSIGNMENT;
    }
    if (is_function_call(raw_input)) {
        // if it includes range then it is function_call or sleep
        return FUNCTION_CALL;
    }
    if (is_cell_dependent_formula(raw_input)) {
        // if input contains cellname = cellname or cell name operand another cell name
        // then we can say it is cell_dependent_formula
        return CELL_DEPENDENT_FORMULA;
    }
    
    //else invalid_input
    return INVALID_INPUT;
}

void parse_input(struct input* in){ // ! To be edited
    in->input_type=find_input_type(in->raw_input);
    return;
}

// Function to check if the input is a scroll command
bool is_scroll_command(const char * raw_input){ 
    
    if (raw_input == NULL) {
        return false; // Handle null input safely
    }
    
    if(strlen(raw_input)==1 && strchr("wasd", raw_input[0]) != NULL){
        return true;
    }
    return false;
}

// Function to check if the input is a quit command
bool is_quit_command(const char * raw_input){
    
    if (raw_input == NULL) {
        return false; // Handle null input safely
    }
    
    if(strlen(raw_input)==1 && raw_input[0]=='q'){
        return true;
    }
    return false;
}

// Function to check if the input is a direct numeric assignment of cell value
bool is_cell_value_assignment(const char * raw_input){  
    if (raw_input == NULL) {
        return false; // Handle null input safely
    }
    
    char *f=strchr(raw_input,'='); // Gets the first occurence of '='
    if(f!=NULL){
        char *after=f+1; // We can use this to get the expression
        printf("%s\n",after);

        char *before=(char *)malloc((f-raw_input+1)*sizeof(char));
        strncpy(before,raw_input,f-raw_input); // we can use this to get the cell name
        before[f-raw_input]='\0';
        printf("%s\n",before);

        if(is_arithmetic_expression(after) && is_cell_name(before)){
            return true;
        }

        free(before);
    }
    return false;
}

// Function to check if the input is a function call
bool is_function_call(const char * raw_input){ // ! To be edited            
    if (raw_input == NULL) {
        return false; // Handle null input safely
    }
    
    char *f=strchr(raw_input,'='); // Gets the first occurence of '='
    if(f!=NULL){

        // Gets the string after '=' from the input
        char *after=f+1; // We can use this to get the expression
        printf("AFTER = %s\n",after);

        // Gets the cell name from the input
        char *before=(char *)malloc((f-raw_input+1)*sizeof(char));
        strncpy(before,raw_input,f-raw_input); // We can use this to get the cell name
        before[f-raw_input]='\0';
        printf("BEFORE = %s\n",before);

        char *open=strchr(after,'('); // Gets the first occurence of '('
        if(open==NULL){ // If there is no '(' then it is not a function call
            return false;
        }

        // Gets the function name from the after string
        char *func=(char *)malloc((open-after+1)*sizeof(char));
        strncpy(func,after,open-after);
        func[open-after]='\0';
        printf("FUNC NAME = %s\n",func);

        char *close=strrchr(after,')'); // Gets the first occurence of ')'
        if(close==NULL){ // If there is no ')' then it is not a function call
            return false;
        }

        // Gets the range from the after string
        char *range=(char *)malloc((close-open)*sizeof(char));
        strncpy(range,open+1,close-open-1);
        range[close-open-1]='\0';
        printf("RANGE = %s\n",range);

        if(is_cell_name(before) && (is_function_name(func)==1 || is_function_name(func)==2) && (is_range(range) || is_cell_name(range))){
            return true;
        }

        free(before);
        free(func);
        free(range);
    }
    return false;
}

// Function to check if the input is a cell dependent formula
bool is_cell_dependent_formula(const char * raw_input){ // ! To be edited
    if (raw_input == NULL) {
        return false; // Handle null input safely
    }

    char *f=strchr(raw_input, '='); // it gives the first occurrence of '='
    if(f!=NULL){
        char *after=f+1;
       
        char *before=(char *)malloc((f-raw_input+1)*sizeof(char)); // Just copying string before '=' from raw_input
        strncpy(before,raw_input,f-raw_input);
        before[f-raw_input]='\0';
        
        if(!is_cell_name(before)){
            return false;
        }
        if(is_cell_expression(after)){
            return true;
        }
        free(before);
    }
    return false;
}
