#include <stdio.h>
#include <stdlib.h>
#include "../include/spreadsheet.h"

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
            s->grid[i][j].val=NULL;
            s->grid[i][j].dependency=false;
        }
    }

    return s;
}

// int main(){
//     sheet *s=create_sheet(5,5);
//     printf("Sheet created with %d rows and %d columns\n",s->num_rows,s->num_cols);
//     printf("First row: %d\n",s->bounds.first_row);
//     printf("First column: %d\n",s->bounds.first_col);
//     return 0;
// }