#include "../include/display.h"
#include "../include/formula_parser.h"
#include "../include/input_handler.h"
#include "../include/recalculation.h"
#include "../include/scrolling.h"
#include "../include/spreadsheet.h"
#include "../include/utils.h"
#include "../include/command_router.h"

#include<stdbool.h>
#include<string.h>
#include <limits.h>

void command_router(sheet * s , char * user_input , bool is_output_enabled) {
    struct input *in = create_input();
    remove_space(user_input);
    //    in->raw_input=user_input;
    in->raw_input=my_strdup(user_input);
    parse_input(in);
    
    switch(in->input_type){
            
        case NOT_DECIDED:
            printf("Error: input type is not decided yet.\n");
            break;
            
        case CELL_VALUE_ASSIGNMENT:
            
            if(!is_valid_cell(in->cell_reference)){
                printf("Error: not a valid cell refrence '%s'.\n",in->cell_reference);
                break;
            }
            if(in->value!=NULL){
                set_cell_value(s, in->cell_reference, atoi(in->value));
                break;
            }else{
                int val = calculate_arithmetic_expression(in->arithmetic_expression);
                if (val == INT_MIN) { // Assuming INT_MIN indicates an error in calculation
                    printf("Error: Invalid arithmetic expression '%s'.\n", in->arithmetic_expression);
                    break;
                }
                set_cell_value(s, in->cell_reference, val);
                
                break;
            }
            
        case CELL_DEPENDENT_FORMULA:
            
            if(!is_valid_cell(in->cell_reference)){
                printf("Error: not a valid cell refrence.\n");
                break;
            }
            
            int ans = eval_formula(in->formula);
            if (ans == INT_MIN) { // Assuming INT_MIN indicates an error in calculation
                printf("Error: Invalid formula '%s'.\n", in->formula);
                break;
            }
            set_cell_value(s, in->cell_reference, ans);
            trigger_recalculation(s);
            
            break;
            
        case FUNCTION_CALL:
            
            if(!is_valid_cell(in->cell_reference)){
                printf("Error: not a valid cell refrence.\n");
                break;
            }
            // to be edited
            
            break;
            
        case SCROLL_COMMAND:
            scroll(s, *in->command);
            break;
            
        case QUIT_COMMAND:
            free_input(in);
            printf("Exiting program. Goodbye!\n");
            exit(0);
            break;
            
        case INVALID_INPUT:
            printf("Error: Invalid input '%s'.\n", user_input);
            break;
            
    }
    
    if(is_output_enabled){
        printf("\n");
        display_sheet(s);
    }
    
    free_input(in);
    return;
}
