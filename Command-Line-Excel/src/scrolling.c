#include "../include/display.h"
#include "../include/formula_parser.h"
#include "../include/input_handler.h"
#include "../include/recalculation.h"
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

    switch(scroll_direction){
        case 'w':
            sheet->bounds.first_row = max(0,first_row-10);
            break;
        case 's':
            sheet->bounds.first_row = min(first_row,first_row+10);
            break;
        case 'a':
            sheet->bounds.first_col = max(0,first_col-10);
            break;
        case 'd':
            sheet->bounds.first_col = min(first_col,first_col+10);
            break;
    }
    return;
}

void scroll_to_cell (sheet *sheet , int row_index , int col_index){
    sheet->bounds.first_row=row_index;
    sheet->bounds.first_col=col_index;
    return;
}
