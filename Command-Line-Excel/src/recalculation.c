#include "../include/recalculation.h"
#include "../include/spreadsheet.h"
#include "../include/utils.h"

#include<stdbool.h>
#include<string.h>
#include <limits.h>

void trigger_recalculation(sheet *s){
    return;
}
void mark_children_dirty(cell * target){
    for(int i=0;i<target->num_children;i++){
        target->children[i]->dirty=true;
        mark_children_dirty(target->children[i]);
    }
}
