#include "../include/display.h"
#include "../include/spreadsheet.h"
#include "../include/utils.h"


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
        char *name=col_index_to_name(j);
        printf("%-*s\t",width,name);
        free(name);
    }
    
    // Print the grid
    printf("\n");
    for (int i=first_row;i<=last_row;i++){
        printf("%d\t",i+1);
        for(int j=first_col;j<=last_col;j++){
            if(s->grid[i][j].val==INT_MIN){  
                // If there is a div by 0 error in the cell then display 'ERR'
                printf("%-*s\t",width,"ERR");
            }
            else{
                printf("%-*d\t",width,s->grid[i][j].val);
            }
        }
        printf("\n");
    }
}
