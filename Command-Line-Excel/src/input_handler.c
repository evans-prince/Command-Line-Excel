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

struct input* create_input(void){
    struct input *new_input =(struct input *) malloc(sizeof(struct input));
    
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

void free_input(struct input* in){// ALL THIS FUNCTION DO IS JUST FREE THE MEMORY ALLOCATED
                                // BY THE MALLAC TO STRUCT OBJECT INPUT
    
    if (in == NULL) {
        return;
    }
    
    free(in->raw_input);
    free(in->cell_reference);
    free(in->value);
    free(in->formula);
    free(in->command);
    free(in->function_name);
    
    if (in->range != NULL) {
        free(in->range->start_cell);
        free(in->range->end_cell);
        free(in->range);
    }
    free(in->arithmetic_expression);
    
    free(in);
}

InputType find_input_type(const char * raw_input){
    return NOT_DECIDED;
}

void parse_input(struct input* in){
    return;
}
