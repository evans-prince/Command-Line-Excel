#ifndef SCROLLING_H
#define SCROLLING_H

typedef struct sheet sheet;

void scroll(sheet *sheet , char scroll_direction); // scroll_direction belongs to { 'w' , 'a' , 's' , 'd' }
void scroll_to_cell(sheet *sheet , int row_index , int col_index);

#endif

