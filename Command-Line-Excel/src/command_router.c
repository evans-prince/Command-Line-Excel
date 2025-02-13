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
            
            if(!is_valid_cell(s->num_rows, s->num_cols, in->cell_reference)){
                printf("Error: not a valid cell refrence '%s'.\n",in->cell_reference);
                break;
            }
            if(in->value!=NULL){
                set_cell_value(s, in->cell_reference, atoi(in->value));
                // break;
            }else{
                int val = calculate_arithmetic_expression(in->arithmetic_expression);
                if (val == INT_MIN) { // Assuming INT_MIN indicates an error in calculation
                    printf("Error: Invalid arithmetic expression '%s'.\n", in->arithmetic_expression);
                    break;
                }
                set_cell_value(s, in->cell_reference, val);
                
                // break;
            }

            // Added a check so that older dependencies of cell_reference are removed
            // Only when the cell_reference's formula field is not NULL
            int row_idx, col_idx;
            cell_name_to_index(in->cell_reference, &row_idx, &col_idx);
            cell *target=&s->grid[row_idx][col_idx];
            if(target->formula!=NULL){
                remove_dependencies(target);
                free(target->formula);
                target->formula=NULL;
            }

            break;
            
        case CELL_DEPENDENT_FORMULA:
            
            if(!is_valid_cell(s->num_rows, s->num_cols, in->cell_reference)){
                printf("Error: not a valid cell refrence.\n");
                break;
            }
            
            int dep_count=0;
            char ** dependencies=parse_formula(in->formula,&dep_count);
            
            if(dependencies==NULL || dep_count==0){
                printf("Error , failed to parse formula : '%s.\n" , in->formula);
                break;
            }
           // I have to update the depndencies here as per new logic
            //  one of this may contain integer value dependencies[0] ,  dependencies[2] and dependencies[1] has op
            char * valid_dependencies[2];
            int valid_dep_count=0;
            if(is_cell_name(dependencies[0])){
                valid_dependencies[valid_dep_count++]=dependencies[0];
            }
            
            if(is_cell_name(dependencies[2])){
                valid_dependencies[valid_dep_count++]=dependencies[2];
            }

            char *parent;
            if(is_cell_name(valid_dependencies[0]) && is_valid_cell(s->num_rows, s->num_cols, valid_dependencies[0])){
                parent=valid_dependencies[0];
            }
            else if(is_cell_name(valid_dependencies[1]) && is_valid_cell(s->num_rows, s->num_cols, valid_dependencies[1])){
                parent=valid_dependencies[1];
            }

            int row1,col1;
            cell_name_to_index(parent, &row1, &col1);
            cell *parent1=&s->grid[row1][col1]; // Getting the parent cell 

            int row2,col2;
            cell_name_to_index(in->cell_reference, &row2, &col2);
            cell *child=&s->grid[row2][col2]; // Getting the child cell
            child->formula=my_strdup(in->formula); // Updated the child's formula as it is dependent on the parents

            if(has_cycle(parent1, child)){
                fprintf(stderr, "Circular reference detected.\n");
                for(int i=0; i<dep_count;i++){
                    free(dependencies[i]);
                }
                
                free(dependencies);
                free_input(in);
                return;
            }

            update_dependencies(s, in->cell_reference, valid_dependencies, valid_dep_count);
            
            int ans = eval_formula(s,dependencies[0],dependencies[2],dependencies[1]);
            if (ans == INT_MIN) { // Assuming INT_MIN indicates an error in calculation
                printf("Error: Invalid formula '%s'.\n", in->formula);
                break;
            }
            set_cell_value(s, in->cell_reference, ans);
            trigger_recalculation(s);
            
            for(int i=0; i<dep_count;i++){
                free(dependencies[i]);
            }
            
            free(dependencies);
            
            break;
            
        case FUNCTION_CALL:
            
            if(!is_valid_cell(s->num_rows, s->num_cols, in->cell_reference)){
                printf("Error: not a valid cell refrence.\n");
                break;
            }
            // ! To be edited
            
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
