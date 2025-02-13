#ifndef RECALCULATION_H
#define RECALCULATION_H

#include "../include/spreadsheet.h"

void trigger_recalculation(sheet *s);
void mark_children_dirty(cell * target);
#endif
