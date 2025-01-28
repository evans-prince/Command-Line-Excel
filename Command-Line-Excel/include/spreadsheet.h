#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct cell {
    int *val;
    bool *dependency;
}cell;

typedef struct spreadsheet_bounds {
    int *first_row;
    int *first_col;
}spreadsheet_bounds;

typedef struct sheet {
    cell ***grid;
    int rows;
    int cols;
    spreadsheet_bounds *bounds;
}sheet;

#endif
