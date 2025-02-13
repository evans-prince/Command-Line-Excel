#ifndef RECALCULATION_H
#define RECALCULATION_H

#include "../include/spreadsheet.h"

void trigger_recalculation(sheet *s, cell *current);
void recalculate_cells(sheet *s, cell **order, int len);
void mark_children_dirty(cell * target);

#endif