#include "../include/display.h"
#include "../include/scrolling.h"
#include "../include/spreadsheet.h"
#include "../include/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(void) {
    // insert code here...
    sheet *s = create_sheet(100,100);
    //printf("%d , %d \n" , s->bounds.first_row , s->bounds.first_col );
    display_sheet(s);
    //scroll(s, 'd');
    //scroll(s, 'w');
    //printf("%d , %d \n" , s->bounds.first_row , s->bounds.first_col );

    return 0;
}
