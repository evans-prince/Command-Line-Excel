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
    s->num_rows=rows;
    s->num_cols=cols;
    
    // Initialize the bounds of the sheet
    s->bounds.first_row=0;
    s->bounds.first_col=0;
    
    // Allocate memory for the grid
    s->grid=(cell **)malloc(rows*sizeof(cell *));
    
    // Initialize the grid
    for(int i=0;i<rows;i++){
        s->grid[i]=(cell *)malloc(cols*sizeof(cell));
        for(int j=0;j<cols;j++){
            s->grid[i][j].val=0;
            s->grid[i][j].dependency=false;
        }
    }
    
    return s;
}

void set_cell_value(sheet *s , char* cell_reference, int value){
    
    int rowIndex, colIndex;
    cell_name_to_index(cell_reference, &rowIndex, &colIndex);
    
    s->grid[rowIndex][colIndex].val = value;
    return;
}
