#include "../include/spreadsheet.h"
#include "../include/utils.h"

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include <limits.h>

// Function to create a new sheet
sheet *create_sheet(int rows, int cols){
    
    // Allocate memory for the sheet
    sheet *s=(sheet *)malloc(sizeof(sheet));
    if(!s){
        fprintf(stderr, "Memory allocation failed for sheet in create_sheet. \n");
        exit(EXIT_FAILURE);
    }
    s->num_rows=rows;
    s->num_cols=cols;
    
    // Initialize the bounds of the sheet
    s->bounds.first_row=0;
    s->bounds.first_col=0;

    // Initialize the status of the sheet
    s->status.elapsed_time=0.0;
    strcpy(s->status.status_message,"ok");
    
    // Allocate memory for the grid
    s->grid=(cell **)malloc(rows*sizeof(cell *));
    
    // Initialize the grid
    for(int i=0;i<rows;i++){
        s->grid[i]=(cell *)malloc(cols*sizeof(cell));
        for(int j=0;j<cols;j++){
            s->grid[i][j].val=0;
            s->grid[i][j].dirty=false;
            s->grid[i][j].formula=NULL;
            s->grid[i][j].parents=NULL;
            s->grid[i][j].children=NULL;
            s->grid[i][j].num_parents=0;
            s->grid[i][j].num_children=0;
            s->grid[i][j].visited=false;
        }
    }
    
    s->chain_capacity=100;
    s->calculation_chain= (cell ** ) malloc(s->chain_capacity*sizeof(cell *));
    if(!s->calculation_chain){
        fprintf(stderr, "Memory allocation failed for calculation chain in create_sheet. \n");
        exit(EXIT_FAILURE);
    }
    s->num_dirty_cells=0;
    
    return s;
}

void free_sheet (sheet * s ){
    if(!s){
        return;
    }
    
    for(int i=0;i<s->num_rows;i++){
        for(int j=0;j<s->num_cols;j++){
            
            free( s->grid[i][j].formula);
            
            s->grid[i][j].formula=NULL;
            
            // for(int k=0;k < s->grid[i][j].num_parents ; k++){
            //     free(s->grid[i][j].parents[k]);
            //     s->grid[i][j].parents[k]=NULL;
            // }
            
            free( s->grid[i][j].parents);
            s->grid[i][j].parents=NULL;
            
            // for(int m=0;m < s->grid[i][j].num_children ; m++){
            //     free(s->grid[i][j].children[m]);
            //     s->grid[i][j].children[m]=NULL;
            // }
            
            free( s->grid[i][j].children);
            s->grid[i][j].children=NULL;
        }
        free( s->grid[i]);
    }
    free(s->grid);
    free(s);
    return;
}

void set_cell_value(sheet *s , char* cell_reference, int value){
    
    int rowIndex, colIndex;
    cell_name_to_index(cell_reference, &rowIndex, &colIndex);
    
    s->grid[rowIndex][colIndex].val = value;
    return;
}

void add_dependency(cell *target, cell *dependency,char message[]){//dependency means parent of target cell
    // target is child

    if(has_cycle(dependency, target)){
        strcpy(message, "Cycle detected in dependencies");
        return;
    }

//    int capacity=1000;
    if(target->num_parents==0){
        target->parents=( cell ** )malloc(10000*sizeof(cell*) );
        if(!target->parents){
            strcpy(message, "Memory allocation failed for parents in add_dependency function.");
            return;
        }
    } else if (target->num_parents % 10000 == 0){
//        capacity*=2;
        target->parents=( cell **) realloc(target->parents,(2*target->num_parents)* sizeof(cell*));
        if(!target->parents){
            strcpy(message, "Memory reallocation failed for parents in add_dependency function.");
            return;
        }
    }
    
    for(int i=0; i < target->num_parents;i++ ){
        if(target->parents[i] == dependency){
            return;
        }
    }
    
    target->parents[target->num_parents++]=dependency;// now target->num_parents increased
    // and dependency is parent of target cell
    
    // Same thing above should also be done for children
    // that is we have to add target as a child of dependency cell
//    int size=1000;
    if(dependency->num_children==0){
        dependency->children=( cell ** )malloc(10000*sizeof(cell*) );
        if(!dependency->children){
            strcpy(message, "Memory allocation failed for children in add_dependency function.");
            return;
        }
    } else if (dependency->num_children%10000 == 0){
//        size*=2;
        dependency->children=( cell **) realloc(dependency->children,(2*dependency->num_children)* sizeof(cell*));
        if(!dependency->children){
            strcpy(message, "Memory allocation failed for children in add_dependency function.");
            return;
        }
    }
    
    for(int i=0; i < dependency->num_children ;i++ ){
        if(dependency->children[i] == target){
            return;
        }
    }
    
    dependency->children[dependency->num_children++]=target;// now target->num_parents increased
    
    strcpy(message, "ok");
}

void remove_dependencies(cell *target){// It removes for every  parents in target->parents
    // remove its children target cell
    // Then free the memory for target->parents
    
    if(target->num_parents==0){
        return;
    }
    
    for(int i=0;i<target->num_parents;i++){
        cell * parent = target->parents[i];
        
        for(int j=0;j<parent->num_children; j++) {
            if(parent->children[j] == target ){
                
                for(int k=j;k<parent->num_children-1;k++) {
                    parent->children[k]=parent->children[k+1];
                }
                parent->num_children--;
                break;
            }
        }
    }
    
    free(target->parents);
    target->parents=NULL;
    target->num_parents=0;
    return;
}

void update_dependencies(sheet *s, char *cell_ref, char **dependencies, int dep_count,char message[]){
    // this function will remove all old dependency of cell_ref and add all new dependencies from char** dependencies
    
    if(cell_ref==NULL || dependencies==NULL || dep_count<=0){
        strcpy(message, "Wrong input to update_dependcies");
        return;
    }
    
    int rowIndex , colIndex;
    cell_name_to_index(cell_ref, &rowIndex, &colIndex);
    
    // target cell is the cell of cell_ref
    cell *target = &s->grid[rowIndex][colIndex];
    
    // remove old parents of target cell and from their children list remove target
    remove_dependencies(target);
    
    for(int i=0;i<dep_count;i++){
        int rowIndex_of_dep , colIndex_of_dep;
        
        if(!is_valid_cell(s->num_rows,s->num_cols,dependencies[i])){
            strcpy(message, "Invalid cell reference");
            continue;
        }
        
        cell_name_to_index(dependencies[i], &rowIndex_of_dep, &colIndex_of_dep);
        // parent is cell correspoding to ith cell in dependencies
        cell *parent = &s->grid[rowIndex_of_dep][colIndex_of_dep];
        
        //target depends on these parents so add its dependencies
        add_dependency(target, parent,message);
        if(strcmp(message, "Cycle detected in dependencies")==0){
            return;
        }
    }
  
    update_topological_ranks(target);
    mark_children_dirty(s , target);
    
    return;
}

bool dfs(cell *current, cell *child, cell ***modified_cells, int *num_modified_cells, int *size){
    if(current==NULL){
        return false;
    }

    // If the current cell is the root, a cycle is detected
    if(current==child){
        return true;
    }
    
    // If the cell is already visited or has no children, no cycle is there
    if(current->visited || current->num_parents==0){
        return false;
    }
    
    // Mark the cell as visited (dirty)
    current->visited=true;

    // If the array of modified cells is full, we double its size using realloc
    if(*num_modified_cells==*size){
        *size*=2;
        *modified_cells = (cell **)realloc(*modified_cells, *size * sizeof(cell *));
        // *modified_cells gives us the actual array of pointers
    }

    // Add the current cell to the array of modified cells
    (*modified_cells)[*num_modified_cells]=current;
    (*num_modified_cells)++;

    // Recursively checking all children of the current cell for cycles
    for(int i=0; i<current->num_parents; i++){
        if(dfs(current->parents[i], child, modified_cells, num_modified_cells, size)){
            return true;
        }
    }
    
    return false;
}

bool has_cycle(cell *parent, cell *child){
    int size=1000;
    cell **modified_cells=(cell **)malloc(size*sizeof(cell *)); // pointer to an array of cells
    int num_modified_cells=0;

    bool cycle_found=dfs(parent, child, &modified_cells, &num_modified_cells, &size);

    // Resetting the visited flag for all modified cells
    for(int i=0; i<num_modified_cells; i++){
        modified_cells[i]->visited=false;
    }

    // Free the allocated memory for modified cells
    free(modified_cells);
    return cycle_found;
}
