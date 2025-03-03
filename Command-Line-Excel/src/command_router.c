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

void command_router(sheet * s , char * user_input , bool is_output_enabled) {
    // double start_time=get_time();
        
    struct input *in = create_input();
    if(!in){
        strcpy(s->status.status_message,"Memory allocation failed for input struct");
        return;
    }
    remove_space(user_input);
    
    in->raw_input=my_strdup(user_input);
    
    char error_message[50]={0};
    strcpy(error_message,"ok");
    parse_input(in,error_message);
    
    if(strcmp(error_message,"ok")!=0){
        free_input(in);
        strcpy(s->status.status_message,error_message);
        return;
    }

    strcpy(s->status.status_message,"ok");
    
    switch(in->input_type){
            
        case NOT_DECIDED:
            strcpy(s->status.status_message,"Input type is not decided yet");
            break;
            
        case CELL_VALUE_ASSIGNMENT:
            
            if(!is_valid_cell(s->num_rows, s->num_cols, in->cell_reference)){
                strcpy(s->status.status_message,"Not a valid cell refrence");
                break;
            }
            
            if(in->value!=NULL){
                set_cell_value(s, in->cell_reference, atoi(in->value));
                
            }else{
                int val = calculate_arithmetic_expression(in->arithmetic_expression);
                if (val == INT_MAX) { // Assuming INT_MIN indicates an error in calculation
                    strcpy(s->status.status_message,"Invalid expression");
                    break;
                }
                else if(val==INT_MIN){
                    strcpy(s->status.status_message,"ok");
                }
                set_cell_value(s, in->cell_reference, val);
                
            }
            
            // Added a check so that older dependencies of cell_reference are removed
            // Only when the cell_reference's formula field is not NULL
            int row_idx, col_idx;
            cell_name_to_index(in->cell_reference, &row_idx, &col_idx);
            cell *target = &s->grid[row_idx][col_idx];
            
            // Remove old dependencies using range logic
            if (target->formula != NULL) {
                remove_range_dependencies(s, target);
                free(target->formula);
                target->formula = NULL;
            }
            
            mark_children_dirty(s, target);
            trigger_recalculation(s);
            break;
            
        case CELL_DEPENDENT_FORMULA: {
                    if (!is_valid_cell(s->num_rows, s->num_cols, in->cell_reference)) {
                        strcpy(s->status.status_message, "Not a valid cell reference");
                        break;
                    }

                    int dep_count = 0;
                    char **dependencies = parse_formula(in->formula, &dep_count);

                    if (dependencies == NULL || dep_count == 0) {
                        strcpy(s->status.status_message, "Failed to parse formula");
                        break;
                    }

                    int row2, col2;
                    cell_name_to_index(in->cell_reference, &row2, &col2);
                    cell *child = &s->grid[row2][col2];
                    child->formula = my_strdup(in->formula);

                    // Convert dependencies into ranges
                    CellRange ranges[dep_count];
                    int range_count = 0;

                    for (int i = 0; i < 3; i++) {
                        if (is_cell_name(dependencies[i])) {
                            int dep_row, dep_col;
                            cell_name_to_index(dependencies[i], &dep_row, &dep_col);
                            ranges[range_count++] = (CellRange){.start_row = dep_row,
                                                                .start_col = dep_col,
                                                                .end_row = dep_row,
                                                                .end_col = dep_col};
                        }
                    }

                    char message[100];
                    update_dependencies(s, in->cell_reference, ranges, range_count, message);
                    if (strcmp(message, "Cycle detected in dependencies") == 0) {
                        strcpy(s->status.status_message, message);
                        for (int i = 0; i < dep_count; i++) {
                            free(dependencies[i]);
                        }
                        free(dependencies);

                        break;
                    }

                    add_to_calculation_chain(s, child);

                    trigger_recalculation(s);

                    for (int i = 0; i < dep_count; i++) {
                        free(dependencies[i]);
                    }
                    
                    free(dependencies);

                    break;
                }
            
        case FUNCTION_CALL: {
            if (!is_valid_cell(s->num_rows, s->num_cols, in->cell_reference)) {
                strcpy(s->status.status_message, "Not a valid cell reference");
                break;
            }

            int function_type = give_function_type(in->function_name);
            if (function_type == -1) {
                strcpy(s->status.status_message, "Undefined function");
                break;
            }

            int target_row, target_col;
            cell_name_to_index(in->cell_reference, &target_row, &target_col);
            cell *target = &s->grid[target_row][target_col];

            // Assign formula to the target cell
            target->formula = my_strdup(in->formula);

            // Handle range-based dependencies
            if (in->range != NULL) {
                char *start_cell = in->range->start_cell;
                char *end_cell = in->range->end_cell;

                if (!is_valid_cell(s->num_rows, s->num_cols, start_cell) ||
                    !is_valid_cell(s->num_rows, s->num_cols, end_cell)) {
                    strcpy(s->status.status_message, "Invalid range");
                    break;
                }

                int start_row, start_col, end_row, end_col;
                cell_name_to_index(start_cell, &start_row, &start_col);
                cell_name_to_index(end_cell, &end_row, &end_col);

                if (start_row > end_row || start_col > end_col) {
                    strcpy(s->status.status_message, "Invalid range");
                    break;
                }

                // Create a CellRange object for the range
                CellRange range = {
                    .start_row = start_row,
                    .start_col = start_col,
                    .end_row = end_row,
                    .end_col = end_col
                };

                // Update dependencies using the range
                char message[100];
                update_dependencies(s, in->cell_reference, &range, 1, message);
                strcpy(s->status.status_message, message);

                if (strcmp(message, "Cycle detected in dependencies") == 0) {
                    free(target->formula);
                    target->formula = NULL;
                    free_input(in);
                    return;
                }
            }

            // Perform the specified function on the range
            int result = 0;
            switch (function_type) {
                case 0: // MIN
                    result = get_min(s, in->range);
                    break;
                case 1: // MAX
                    result = get_max(s, in->range);
                    break;
                case 2: // AVG
                    result = get_avg(s, in->range);
                    break;
                case 3: // SUM
                    result = get_sum(s, in->range);
                    break;
                case 4: // STDEV
                    result = get_stdev(s, in->range);
                    break;
                case 5: // SLEEP
                    if (in->value != NULL) {
                        if (atoi(in->value) > 0) {
                            sleep(atoi(in->value));
                            s->status.elapsed_time += atoi(in->value);
                        }
                        result = atoi(in->value);
                    } else {
                        int row, col;
                        cell_name_to_index(in->range->start_cell, &row, &col);
                        int cell_val = s->grid[row][col].val;
                        if (cell_val > 0) {
                            sleep(cell_val);
                            s->status.elapsed_time += cell_val;
                        }
                        result = cell_val;
                    }
                    break;
                default:
                    strcpy(s->status.status_message, "Unknown function type");
                    return;
            }

            // Set the result in the respective cell
            set_cell_value(s, in->cell_reference, result);

            // Mark children dirty and trigger recalculation
            mark_children_dirty(s, target);
            trigger_recalculation(s);

            break;
        }


            
        case SCROLL_COMMAND:
            if(in->cell_reference!=NULL){
                
                if(!is_valid_cell(s->num_rows, s->num_cols, in->cell_reference)){
                    strcpy(s->status.status_message,"Not a valid cell refrence");
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
            exit(0);
            break;
            
        case INVALID_INPUT:
            strcpy(s->status.status_message,"Invalid input");
            break;
            
    }
    
    if(is_output_enabled){
        printf("\n");
        display_sheet(s);
    }
    
    // double end_time=get_time();
    // double elapsed_time=end_time-start_time;
    // s->status.elapsed_time+=elapsed_time;
    
    free_input(in);
    return;
}
