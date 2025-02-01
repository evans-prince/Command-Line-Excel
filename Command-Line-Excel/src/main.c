#include "../include/display.h"
#include "../include/scrolling.h"
#include "../include/spreadsheet.h"
#include "../include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(void) {
    // insert code here...
    int r,c;
    scanf("Enter row : %d Enter col : %d ", &r , &c);
    sheet *s = create_sheet(r,c);
    //printf("%d , %d \n" , s->bounds.first_row , s->bounds.first_col );
    display_sheet(s);
    //scroll(s, 'd');
    //scroll(s, 'w');
    //printf("%d , %d \n" , s->bounds.first_row , s->bounds.first_col );

    return 0;
}
