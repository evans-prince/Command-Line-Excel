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
#include <unistd.h>
#include <errno.h>

CommandStatus command_router(sheet * s , char * user_input , bool is_output_enabled) {
    double start_time=get_time();
    
    CommandStatus c={.elapsed_time=0.0, .status_message="ok"};
    
    struct input *in = create_input();
    if(!in){
        strcpy(c.status_message,"Memory allocation failed for input struct");
        return c;
    }
    remove_space(user_input);
    
    in->raw_input=my_strdup(user_input);
    
    char error_message[50]={0};
    strcpy(error_message,"ok");
    parse_input(in,error_message);
    
    if(strcmp(error_message,"ok")!=0){
        free_input(in);
        strcpy(c.status_message,error_message);
        return c;
    }
    
    switch(in->input_type){
            
        case NOT_DECIDED:
            strcpy(c.status_message,"Input type is not decided yet");
            // fprintf(stderr,"Error: input type is not decided yet.\n");
            break;
            
        case CELL_VALUE_ASSIGNMENT:
            
            if(!is_valid_cell(s->num_rows, s->num_cols, in->cell_reference)){
                strcpy(c.status_message,"Not a valid cell refrence");
                // fprintf(stderr,"Error: not a valid cell refrence '%s'.\n",in->cell_reference);
                break;
            }
            
            if(in->value!=NULL){
                set_cell_value(s, in->cell_reference, atoi(in->value));
                
            }else{
                int val = calculate_arithmetic_expression(in->arithmetic_expression);
                if (val == INT_MAX) { // Assuming INT_MIN indicates an error in calculation
                    strcpy(c.status_message,"Invalid expression");
                    // fprintf(stderr,"Error: Invalid arithmetic expression '%s'.\n", in->arithmetic_expression);
                    break;
                }
                else if(val==INT_MIN){
                    strcpy(c.status_message,"Division by zero error");
                    break;
                }
                set_cell_value(s, in->cell_reference, val);
                
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
            
            mark_children_dirty(s, target);
            trigger_recalculation(s);
            break;
            
        case CELL_DEPENDENT_FORMULA:
            
            if(!is_valid_cell(s->num_rows, s->num_cols, in->cell_reference)){
                strcpy(c.status_message,"Not a valid cell reference");
                // fprintf(stderr,"Error: not a valid cell refrence.\n");
                break;
            }
            
            int dep_count=0;
            char ** dependencies=parse_formula(in->formula,&dep_count);
            
            if(dependencies==NULL || dep_count==0){
                strcpy(c.status_message,"Failed to parse formula");
                // fprintf(stderr,"Error , failed to parse formula : '%s.\n" , in->formula);
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
                strcpy(c.status_message,"Circular reference detected");
                // fprintf(stderr, "Circular reference detected.\n");
                for(int i=0; i<dep_count;i++){
                    free(dependencies[i]);
                }
                
                free(dependencies);
                free_input(in);
                return c;
            }
            
            update_dependencies(s, in->cell_reference, valid_dependencies, valid_dep_count);
            
            //            int ans = eval_formula(s,dependencies[0],dependencies[2],dependencies[1]);
            //            if (ans == INT_MAX) { // Assuming INT_MIN indicates an error in calculation
            //                printf("Error: Invalid formula '%s'.\n", in->formula);
            //                break;
            //            }
            //            // else if(ans==INT_MIN){
            //            //     printf("Error: Division by zero error.\n");
            //            //     break;
            //            // }
            //
            //            set_cell_value(s, in->cell_reference, ans);
            //
            //            if(child->num_children!=0){
            //                trigger_recalculation(s, child);
            //            }
            
            int row, col;
            cell_name_to_index(in->cell_reference, &row, &col);
            add_to_calculation_chain(s, &s->grid[row][col]);
            
            trigger_recalculation(s);
            
            for(int i=0; i<dep_count;i++){
                free(dependencies[i]);
            }
            
            free(dependencies);
            
            break;
            
        case FUNCTION_CALL:{
            
            if(!is_valid_cell(s->num_rows, s->num_cols, in->cell_reference)){
                strcpy(c.status_message,"Not a valid cell refrence");
                // fprintf(stderr,"Error: not a valid cell refrence.\n");
                break;
            }
            // ! To be edited
            int function_type=give_function_type(in->function_name);
            switch (function_type) {
                case -1:
                    //UNDEFINED FUNCTION HERE PROCEED Accordingly
                    break;
                    
                case 0:{
                    int min_val;
                    min_val=get_min(in->range);
                    set_cell_value(s, in->cell_reference, min_val);
                    break;
                }
                    
            }
            
            break;
        }
            
        case SCROLL_COMMAND:
            if(in->cell_reference!=NULL){
                
                if(!is_valid_cell(s->num_rows, s->num_cols, in->cell_reference)){
                    strcpy(c.status_message,"Not a valid cell refrence");
                    // printf("Error: not a valid cell refrence.\n");
                    break;
                }
                
                int row_index, col_index;
                cell_name_to_index(in->cell_reference, &row_index, &col_index);
                scroll_to_cell(s, row_index, col_index);
                break;
            }
            scroll(s, *in->command);
            break;
            
        case QUIT_COMMAND:
            free_input(in);
            free_sheet(s);
            fprintf(stderr,"Exiting program. Goodbye!\n");
            exit(0);
            break;
            
        case INVALID_INPUT:
            strcpy(c.status_message,"Invalid input");
            // fprintf(stderr,"Error: Invalid input '%s'.\n", user_input);
            break;
            
    }
    
    if(is_output_enabled){
        printf("\n");
        display_sheet(s);
    }
    
    double end_time=get_time();
    c.elapsed_time=end_time-start_time;
    
    free_input(in);
    return c;
}
