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
                fprintf(stderr, "Error: Invalid formula '%s'.\n", order[i]->formula);
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
void mark_children_dirty(sheet *s ,cell * target){
    for(int i=0;i<target->num_children;i++){
        add_to_calculation_chain(s, target->children[i]);
        mark_children_dirty(s,target->children[i]);
    }
    return;
}

void trigger_recalculation(sheet *s){
    
//    // you will be given only sheet pointer s ,
//    //from this you have to recalculate every cell available in calculation chain
//    
//    // so update this function accordingly
//    
//    if(current==NULL){
//        return;
//    }
//
//    current->dirty=true;
//    mark_children_dirty(current); // ! To be made by PRINCE
//
//    int len=0;
//    cell **order=topological_sort(current, &len); // ! To be made by PRINCE
//    // ! Don't include current cell in the topo sort order as we have already evaluated its value
//
//    // order is a pointer to an array of cells
//
//    recalculate_cells(s, order, len);
//    
//    free(order);
//    return;
    
    sort_calculation_chain(s); // Ensure calculation chain is sorted

       while (s->num_dirty_cells > 0) {
           cell *c = s->calculation_chain[0]; // Process first dirty cell

           if (c->formula != NULL) {
               int dep_count;
               char **dependencies = parse_formula(c->formula, &dep_count);
               c->val = eval_formula(s, dependencies[0], dependencies[2], dependencies[1]);
               for(int i=0; i<dep_count;i++){
                   free(dependencies[i]);
               }
               free(dependencies); // Free allocated memory
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
            fprintf(stderr, "Memory reallocation failed in add_to_calculation_chain function. \n");
            exit(EXIT_FAILURE);
        }
    }
    
    s->calculation_chain[s->num_dirty_cells] = c;
    s->num_dirty_cells++;
    return;
}

void remove_from_calculation_chain(sheet *s, cell *c){
    
    for (int i = 0; i < s->num_dirty_cells; i++) {
        
        if (s->calculation_chain[i] == c){
            
            for(int j=i; j< s->num_dirty_cells-1; j++) {
                s->calculation_chain[j] = s->calculation_chain[j+1];
            }
            s->num_dirty_cells--;
            c->dirty=false;
            return;
        }
        
    }
    return;
}

void update_topological_ranks(cell *target){
    if (target->num_parents == 0){
        target->topological_rank = 0;
    }else{
        int max_rank = -1;
        for (int i=0; i < target->num_parents; i++){
            if(target->parents[i]->topological_rank > max_rank){
                max_rank = target->parents[i]->topological_rank;
            }
        }
        target->topological_rank = max_rank+1;
    }

    for(int i=0; i < target->num_children; i++){
        update_topological_ranks(target->children[i]);
    }
    
    return;
}

int compare_cells(const void *a , const void *b){ // negative means cellA < cellB
                        // zero means equal and positive means cellB < cellA
    
    cell *cellA = *(cell **) a;// a is void ptr so first cast to cell** which is ptr of ptr to cell
                                // another * is used for dereferencing
    cell *cellB = *(cell **) b;
    
    return cellA->topological_rank - cellB->topological_rank;

}

void sort_calculation_chain(sheet *s){
    qsort(s->calculation_chain, s->num_dirty_cells, sizeof(cell *), compare_cells);
    // the sorting algo can be made by ouselves for improving efficiency of our program
    // likely we  will use binary search to find the position of new incoming elem
    // and place it in already sorted chain.
    // we have to shift the entire chain then
    // we may have to remove some elem from chain , the function is already made use them
    // if needed
}
