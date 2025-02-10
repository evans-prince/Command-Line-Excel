#include "../include/scrolling.h"
#include "../include/spreadsheet.h"
#include "../include/utils.h"

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include <limits.h>

void scroll ( sheet *sheet , char scroll_direction) {
    int first_row = sheet->bounds.first_row;
    int first_col= sheet->bounds.first_col;
    int last_row = sheet->num_rows;
    int last_col = sheet->num_cols;
    
    if(last_row <10 && (scroll_direction=='s' || scroll_direction=='w')){
        return;//nothing to be done
    }
    
    if(last_col<10 && (scroll_direction=='a' || scroll_direction=='d')){
        return;//nothing to be done
    }
    
    switch(scroll_direction){
        case 'w':
            if (first_row > 0)
                sheet->bounds.first_row = max(0,first_row-10);
            break;
        case 's':
            if (first_row + 10 < last_row)
                sheet->bounds.first_row =min(first_row+20,last_row-1)-10;
            break;
        case 'a':
            if (first_col > 0)
                sheet->bounds.first_col = max(0,first_col-10);
            break;
        case 'd':
            sheet->bounds.first_col = min(first_col+20,last_col-1)-10;
            break;
    }
    return;
}

void scroll_to_cell (sheet *sheet , int row_index , int col_index){
    sheet->bounds.first_row=row_index;
    sheet->bounds.first_col=col_index;
    return;
}
