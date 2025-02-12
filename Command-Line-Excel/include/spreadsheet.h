#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include<stdbool.h>

typedef struct cell {
    int val;
    bool dirty;
    char *formula;
    struct cell **parents; // * parents[] pointer to parents array
    struct cell **children;
    int num_parents;
    int num_children;
}cell;

typedef struct spreadsheet_bounds {
    int first_row;
    int first_col;
}spreadsheet_bounds;

typedef struct sheet {
    cell **grid; // grid[][] this is a 2d array grid
    int num_rows;
    int num_cols;
    spreadsheet_bounds bounds;
}sheet;

sheet *create_sheet(int rows, int cols);
void set_cell_value(sheet *s ,char* cell_reference, int value);
void add_dependency(cell *target, cell *dependency);
void remove_dependencies(cell *target);
void update_dependencies(sheet *s, char *cell_ref, char **dependencies, int dep_count);
bool has_cycle(cell *start);

#endif
