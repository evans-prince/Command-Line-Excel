#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include<stdbool.h>

typedef struct cell {
    int val;
    bool dirty;
    char *formula;
    struct cell **parents;
    struct cell **children;
    int num_parents;
    int num_children;
}cell;

typedef struct spreadsheet_bounds {
    int first_row;
    int first_col;
}spreadsheet_bounds;

typedef struct sheet {
    cell **grid;
    int num_rows;
    int num_cols;
    spreadsheet_bounds bounds;
}sheet;

sheet *create_sheet(int rows, int cols);

void set_cell_value(sheet *s ,char* cell_reference, int value);

#endif
