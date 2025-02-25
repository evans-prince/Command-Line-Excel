#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include<stdbool.h>
#include "recalculation.h"

typedef struct cell {
    int val;
    bool dirty;
    char *formula;
    struct cell **parents; // * parents[] pointer to parents array
    struct cell **children;
    int num_parents;
    int num_children;
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
void add_dependency(cell *target, cell *dependency,char message[]);
void remove_dependencies(cell *target);
void update_dependencies(sheet *s, char *cell_ref, char **dependencies, int dep_count,char message[]);
bool has_cycle(cell *parent, cell *child);

#endif
