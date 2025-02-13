#include "../include/recalculation.h"
#include "../include/spreadsheet.h"
#include "../include/utils.h"
#include "../include/formula_parser.h"

#include<stdbool.h>
#include<string.h>
#include <limits.h>

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
                // printf("Error: Invalid formula '%s'.\n", order[i]->formula);
                // ! To be edited by ME
                continue;
            }
            else if(ans==INT_MIN){ // It means that formula is of the form div by 0 
                // ! To be edited by ME
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
void mark_children_dirty(cell * target){
    for(int i=0;i<target->num_children;i++){
        target->children[i]->dirty=true;
        mark_children_dirty(target->children[i]);
    }
}

void trigger_recalculation(sheet *s, cell *current){
    if(current==NULL){
        return;
    }

    current->dirty=true;
    mark_children_dirty(current); // ! To be made by PRINCE

    int len=0;
    cell **order=topological_sort(current, &len); // ! To be made by PRINCE
    // ! Don't include current cell in the topo sort order as we have already evaluated its value

    // order is a pointer to an array of cells

    recalculate_cells(s, order, len);
    
    free(order);
}
