#include "../include/recalculation.h"
#include "../include/spreadsheet.h"
#include "../include/utils.h"
#include "../include/formula_parser.h"
#include "../include/input_handler.h"

#include<stdbool.h>
#include<string.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>

void recalculate_cells(sheet *s, cell **order, int len){
    for (int i=0;i<len;i++){
        if(order[i]->dirty){
            order[i]->dirty=false;

            int dep_count=0;
            char **dependencies=parse_formula(order[i]->formula, &dep_count);

            if(dependencies==NULL || dep_count==0){
                printf("Error , failed to parse formula : '%s.\n" , order[i]->formula);
                continue;
            }

            int ans=eval_formula(s, dependencies[0], dependencies[2], dependencies[1]);
            if (ans==INT_MAX) { // Assuming INT_MAX indicates an error in calculation
                strcpy(s->status.status_message,"Invalid formula");
                continue;
            }

            order[i]->val=ans;

            for(int j=0;j<dep_count;j++){
                free(dependencies[j]);
            }
            
            free(dependencies);
        }
    }

    return;
}
void mark_children_dirty(sheet *s, cell *target){
    // Iterate over all child ranges
    for (int i = 0; i < target->num_child_ranges; i++){
        CellRange range = target->child_ranges[i];

        // Iterate over all cells in the range
        for (int row = range.start_row; row <= range.end_row; row++){
            for (int col = range.start_col; col <= range.end_col; col++){
                cell *child = &s->grid[row][col];

                // Add the child to the calculation chain if not already dirty
                add_to_calculation_chain(s, child);

                // Recursively mark its children as dirty
                mark_children_dirty(s, child);
            }
        }
    }
}

void trigger_recalculation(sheet *s){
    sort_calculation_chain(s); // Ensure calculation chain is sorted

    while (s->num_dirty_cells > 0) {
        cell *c = s->calculation_chain[0]; // Process first dirty cell
        
        char *formula=my_strdup(c->formula);
        char *open=strchr(formula,'(');
        if(open==NULL){
            int dep_count;
            char **dependencies = parse_formula(c->formula, &dep_count);
            c->val = eval_formula(s, dependencies[0], dependencies[2], dependencies[1]);
            for(int i=0; i<dep_count;i++){
                free(dependencies[i]);
            }
            free(dependencies); // Free allocated memory
        }
        else{
            *open='\0';
            char *close=strchr(open+1,')');
            *close='\0';
            // char *range=open+1;
            Range *r=(Range *)malloc(sizeof(Range));
            
            parse_range(open+1, r);
            char *fun_name=formula;
            int function_type=give_function_type(fun_name);
            int result = 0;
            if(function_type!=5){
                result=get_function_output(function_type,r,s);
                c->val=result;
            }else if (function_type==5){
                int row, col;
                cell_name_to_index(r->start_cell, &row, &col);
                int cell_val=s->grid[row][col].val;
                if(cell_val>0){
                    sleep(cell_val);
                    s->status.elapsed_time+=cell_val;
                }
                result=cell_val;
                c->val=result;
            }
           
        }

        remove_from_calculation_chain(s, c); // Remove processed cell from chain
        c->dirty = false; // Reset dirty flag
    }
}

void add_to_calculation_chain(sheet *s, cell *c){
    if(c->dirty){
        return;
    }
    
    c->dirty=true;
    if(s->num_dirty_cells==s->chain_capacity){
        s->chain_capacity*=2;
        
        s->calculation_chain=(cell **) realloc(s->calculation_chain, s->chain_capacity*sizeof(cell *));
        if(!s->calculation_chain){
            strcpy(s->status.status_message, "Memory reallocation failed in add_to_calculation_chain function. \n");
            exit(EXIT_FAILURE);
        }
    }
    
    s->calculation_chain[s->num_dirty_cells] = c;
    s->num_dirty_cells++;
    return;
}

void remove_from_calculation_chain(sheet *s, cell *c){
    
    for (int i = 0; i < s->num_dirty_cells; i++){
        
        if (s->calculation_chain[i] == c){
            
            for(int j=i; j< s->num_dirty_cells-1; j++){
                s->calculation_chain[j] = s->calculation_chain[j+1];
            }
            s->num_dirty_cells--;
            c->dirty=false;
            return;
        }
        
    }
    return;
}

void update_topological_ranks(cell *target, sheet *s){
    if (target->num_parent_ranges == 0){
        target->topological_rank = 0;
    } else {
        int max_rank = -1;

        // Iterate over all parent ranges
        for (int i = 0; i < target->num_parent_ranges; i++){
            CellRange range = target->parent_ranges[i];

            // Iterate over all cells in the parent range
            for (int row = range.start_row; row <= range.end_row; row++){
                for (int col = range.start_col; col <= range.end_col; col++){
                    cell *parent = &s->grid[row][col];
                    if (parent->topological_rank > max_rank){
                        max_rank = parent->topological_rank;
                    }
                }
            }
        }

        // Update the topological rank of the target cell
        target->topological_rank = max_rank + 1;
    }

    // Propagate rank updates to children
    for (int i = 0; i < target->num_child_ranges; i++){
        CellRange range = target->child_ranges[i];

        // Iterate over all cells in the child range
        for (int row = range.start_row; row <= range.end_row; row++){
            for (int col = range.start_col; col <= range.end_col; col++){
                cell *child = &s->grid[row][col];
                update_topological_ranks(child, s);
            }
        }
    }
}


int compare_cells(const void *a , const void *b){ // negative means cellA < cellB
                        // zero means equal and positive means cellB < cellA
    
    cell *cellA = *(cell **) a;// a is void ptr so first cast to cell** which is ptr of ptr to cell
                                // another * is used for dereferencing
    cell *cellB = *(cell **) b;
    
    return cellA->topological_rank - cellB->topological_rank;

}

void sort_calculation_chain(sheet *s){
    if (s->num_dirty_cells <= 1) return; // Already sorted

    for (int i = 1; i < s->num_dirty_cells; i++){
        cell *key = s->calculation_chain[i];
        int left = 0, right = i - 1;

        // Binary search for insertion point
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (compare_cells(&s->calculation_chain[mid], &key) > 0)
                right = mid - 1;
            else
                left = mid + 1;
        }

        // Shift elements to make space
        for (int j = i; j > left; j--){
            s->calculation_chain[j] = s->calculation_chain[j - 1];
        }

        // Insert the key at found position
        s->calculation_chain[left] = key;
    }
}
