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
            s->grid[i][j].parent_ranges=NULL;
            s->grid[i][j].child_ranges=NULL;
            s->grid[i][j].num_parent_ranges=0;
            s->grid[i][j].num_child_ranges=0;
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

void free_sheet(sheet *s) {
    if (!s) {
        return;
    }

    // Free each cell's resources
    for (int i = 0; i < s->num_rows; i++) {
        for (int j = 0; j < s->num_cols; j++) {
            cell *current = &s->grid[i][j];

            // Free formula
            free(current->formula);

            // Free parent ranges
            free(current->parent_ranges);

            // Free child ranges
            free(current->child_ranges);
        }
        free(s->grid[i]); // Free each row
    }

    // Free the grid and calculation chain
    free(s->grid);
    free(s->calculation_chain);

    // Free the sheet structure itself
    free(s);
}


void set_cell_value(sheet *s , char* cell_reference, int value){
    
    int rowIndex, colIndex;
    cell_name_to_index(cell_reference, &rowIndex, &colIndex);
    
    s->grid[rowIndex][colIndex].val = value;
    return;
}

void add_range_dependency(sheet *sheet, int row_idx, int col_idx, CellRange *range, char message[]) {
    // Check for cycles using the first cell in the range (optimization)
    int start_row = range->start_row;
    int start_col = range->start_col;

    // Assuming you have access to the sheet structure
    cell *start_cell = &sheet->grid[start_row][start_col];

    cell *target = &sheet->grid[row_idx][col_idx];

    if (has_cycle(start_cell, target,sheet)) {
        strcpy(message, "Cycle detected in dependencies");
        return;
    }

    // Add the range to the target's parent_ranges
    if (target->num_parent_ranges == 0) {
        target->parent_ranges = (CellRange *)malloc(2 * sizeof(CellRange));
        if (!target->parent_ranges) {
            strcpy(message, "Memory allocation failed for parent ranges");
            return;
        }
    } else if (target->num_parent_ranges % 2 == 0) {
        target->parent_ranges = (CellRange *)realloc(target->parent_ranges,
                                (target->num_parent_ranges + 10) * sizeof(CellRange));
        if (!target->parent_ranges) {
            strcpy(message, "Memory reallocation failed for parent ranges");
            return;
        }
    }

    target->parent_ranges[target->num_parent_ranges++] = *range;

    // Similarly, add the target as a child to all cells in the range
    for (int i = range->start_row; i <= range->end_row; i++) {
        for (int j = range->start_col; j <= range->end_col; j++) {
            cell *parent_cell = &sheet->grid[i][j];

            if (parent_cell->num_child_ranges == 0) {
                parent_cell->child_ranges = (CellRange *)malloc(10 * sizeof(CellRange));
                if (!parent_cell->child_ranges) {
                    strcpy(message, "Memory allocation failed for child ranges");
                    return;
                }
            } else if (parent_cell->num_child_ranges % 10 == 0) {
                parent_cell->child_ranges = (CellRange *)realloc(parent_cell->child_ranges,
                                        (parent_cell->num_child_ranges + 10) * sizeof(CellRange));
                if (!parent_cell->child_ranges) {
                    strcpy(message, "Memory reallocation failed for child ranges");
                    return;
                }
            }
            
            // int target_row = (target - sheet->grid[0]) / sheet->num_cols;
            // int target_col = (target - sheet->grid[0]) % sheet->num_cols;
            CellRange target_range = { .start_row = row_idx,
                                       .start_col = col_idx,
                                       .end_row = row_idx,
                                       .end_col = col_idx };

            parent_cell->child_ranges[parent_cell->num_child_ranges++] = target_range;
        }
    }

    strcpy(message, "ok");
}


void remove_range_dependencies(sheet *sheet, cell *target) {
    // If there are no parent ranges, nothing to remove
    if (target->num_parent_ranges == 0) {
        return;
    }

    // Iterate through all parent ranges
    for (int i = 0; i < target->num_parent_ranges; i++) {
        // Get the ith parent range
        CellRange range = target->parent_ranges[i];

        // Now each ith range consists of a start row, start col, end row and end col
        for (int row = range.start_row; row <= range.end_row; row++) {
            for (int col = range.start_col; col <= range.end_col; col++) {
                // We extract the parent cell from the indices
                cell *parent_cell = &sheet->grid[row][col];

                // Remove the target from the parent's child ranges
                for (int j = 0; j < parent_cell->num_child_ranges; j++) {
                    // Now we go to the child range of each parent cell
                    CellRange child_range = parent_cell->child_ranges[j];

                    // each child range consists of a start row, start col, end row and end col
                    // where start_row=end_row and start_col=end_col
                    // If the target is found in the child ranges, we remove

                    // We compare the memory location of the target cell with the memory location of the child range, if it matches we remove target
                    if (target == &sheet->grid[child_range.start_row][child_range.start_col]) {
                        parent_cell->child_ranges[j] = parent_cell->child_ranges[parent_cell->num_child_ranges-1];

                        // Decrement the number of child ranges
                        parent_cell->num_child_ranges--;
                        continue;
                    }
                }
            }
        }
    }

    // Free memory for the parent's ranges and reset count
    free(target->parent_ranges);
    target->parent_ranges = NULL;
    target->num_parent_ranges = 0;
}


void update_dependencies(sheet *s, char *cell_ref, CellRange *ranges, int range_count, char message[]) {
    // Validate input
    if (!cell_ref || !ranges || range_count <= 0) {
        strcpy(message, "Invalid input to update_dependencies");
        return;
    }

    // Get the target cell's row and column indices
    int rowIndex, colIndex;
    cell_name_to_index(cell_ref, &rowIndex, &colIndex);

    // Get the target cell
    cell *target = &s->grid[rowIndex][colIndex];

    // Step 1: Remove old dependencies
    remove_range_dependencies(s, target);

    // Step 2: Add new dependencies
    for (int i = 0; i < range_count; i++) {
        add_range_dependency(s, rowIndex, colIndex, &ranges[i], message);

        // If a cycle is detected, stop further processing
        if (strcmp(message, "Cycle detected in dependencies") == 0) {
            return;
        }
    }

    // Step 3: Mark children as dirty for recalculation
    mark_children_dirty(s, target);

    // Set success message
    strcpy(message, "ok");
}



bool dfs(cell *current, cell *child, cell ***modified_cells, int *num_modified_cells, int *size, sheet *s) {
    if (current == NULL) {
        return false;
    }

    // If the current cell is the root (child), a cycle is detected
    if (current == child) {
        return true;
    }

    // If the cell is already visited or has no parent ranges, no cycle is there
    if (current->visited || current->num_parent_ranges == 0) {
        return false;
    }

    // Mark the cell as visited
    current->visited = true;

    // If the array of modified cells is full, double its size using realloc
    if (*num_modified_cells == *size) {
        *size *= 2;
        *modified_cells = (cell **)realloc(*modified_cells, *size * sizeof(cell *));
        if (!*modified_cells) {
            fprintf(stderr, "Memory reallocation failed in dfs.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Add the current cell to the array of modified cells
    (*modified_cells)[*num_modified_cells] = current;
    (*num_modified_cells)++;

    // Recursively check all parent ranges of the current cell for cycles
    for (int i = 0; i < current->num_parent_ranges; i++) {
        CellRange range = current->parent_ranges[i];

        // Iterate over all cells in the range
        for (int row = range.start_row; row <= range.end_row; row++) {
            for (int col = range.start_col; col <= range.end_col; col++) {
                cell *parent_cell = &s->grid[row][col];

                // Perform DFS on each parent cell in the range
                if (dfs(parent_cell, child, modified_cells, num_modified_cells, size, s)) {
                    return true;
                }
            }
        }
    }

    return false;
}



bool has_cycle(cell *parent, cell *child, sheet *s) {
    int size = 1000;
    cell **modified_cells = (cell **)malloc(size * sizeof(cell *)); // Pointer to an array of cells
    if (!modified_cells) {
        fprintf(stderr, "Memory allocation failed in has_cycle.\n");
        exit(EXIT_FAILURE);
    }

    int num_modified_cells = 0;

    // Perform DFS to detect cycles
    bool cycle_found = dfs(parent, child, &modified_cells, &num_modified_cells, &size, s);

    // Reset the visited flag for all modified cells
    for (int i = 0; i < num_modified_cells; i++) {
        modified_cells[i]->visited = false;
    }

    // Free the allocated memory for modified cells
    free(modified_cells);

    return cycle_found;
}


