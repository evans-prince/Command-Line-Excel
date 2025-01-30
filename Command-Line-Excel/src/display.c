#include "../include/display.h"
#include "../lib/utils.c"
#include "spreadsheet.c"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MIN(a,b) (a<b?a:b);
#define MAX(a,b) (a<b?b:a);

// Function to display the sheet
void display_sheet(sheet *s){
    int rows=s->num_rows;
    int cols=s->num_cols;

    int first_row=s->bounds.first_row;
    int first_col=s->bounds.first_col;
    int last_row=MIN(first_row+9,rows-1);
    int last_col=MIN(first_col+9,cols-1);

    int width=10;

    // Print the column names
    printf("\t");
    for(int j=first_col;j<=last_col;j++){
        printf("%-*s\t",width,col_index_to_name(j));
    }

    // s->grid[3][3].val=INT_MAX;
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

// int main(){
//     sheet *s=create_sheet(55,55);
//     display_sheet(s);
//     return 0;
// }