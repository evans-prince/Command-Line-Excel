#ifndef RECALCULATION_H
#define RECALCULATION_H

#include "../include/spreadsheet.h"

void trigger_recalculation(sheet *s, cell *current);
void recalculate_cells(sheet *s, cell **order, int len);
void mark_children_dirty(sheet *s ,cell * target);
void add_to_calculation_chain(sheet *s, cell *c);
void remove_from_calculation_chain(sheet *s, cell *c);

void update_topological_ranks(cell *target);
int compare_cells(const void *a , const void *b);
void sort_calculation_chain(sheet *s);
#endif
