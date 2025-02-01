#include "../include/display.h"
#include "../include/utils.h"
#include "../include/spreadsheet.h"
#include "../include/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


// Function to display the sheet
void display_sheet(sheet *s){
    int rows=s->num_rows;
    int cols=s->num_cols;

    int first_row=s->bounds.first_row;
    int first_col=s->bounds.first_col;
    int last_row=min(first_row+9,rows-1);
    int last_col=min(first_col+9,cols-1);

    int width=10;

    // Print the column names
    printf("\t");
    for(int j=first_col;j<=last_col;j++){
        printf("%-*s\t",width,col_index_to_name(j));
    }

    printf("\n");
    // Add a horizontal line separator for column headers
    printf("\t");
    for (int j = first_col; j <= last_col; j++) {
        for (int k = 0; k <= width; k++) {
            printf("-");
        }
        printf("-");
    }
    
    // Print the grid
    printf("\n");
    for (int i=first_row;i<=last_row;i++){
        printf("%d\t",i+1);
        for(int j=first_col;j<=last_col;j++){
            printf("%-*d\t",width,s->grid[i][j].val);
        }
        printf("\n");
    }
}
