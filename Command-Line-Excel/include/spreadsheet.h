#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include<stdbool.h>
#include "recalculation.h"

typedef struct {
    int start_row;
    int start_col;
    int end_row;
    int end_col;
} CellRange;

typedef struct cell {
    int val;
    bool dirty;
    char *formula;
    CellRange *parent_ranges; // * parents[] pointer to parents array
    CellRange *child_ranges;
    int num_parent_ranges;
    int num_child_ranges;
    bool visited;
    int topological_rank;
}cell;

typedef struct spreadsheet_bounds {
    int first_row;
    int first_col;
}spreadsheet_bounds;

typedef struct{
    char status_message[100];
    float elapsed_time;
}CommandStatus;

typedef struct sheet {
    cell **grid; // grid[][] this is a 2d array grid
    int num_rows;
    int num_cols;
    spreadsheet_bounds bounds;
    CommandStatus status;
    
    cell ** calculation_chain;
    int num_dirty_cells;
    int chain_capacity;
}sheet;

sheet *create_sheet(int rows, int cols);
void free_sheet (sheet * s );
void set_cell_value(sheet *s ,char* cell_reference, int value);
void add_range_dependency(sheet *sheet, int row_idx, int col_idx, CellRange *range, char message[]);
void remove_range_dependencies(sheet *sheet, cell *target);
void update_dependencies(sheet *s, char *cell_ref, CellRange *ranges, int range_count, char message[]);
bool has_cycle(cell *parent, cell *child, sheet *sheet);

#endif
