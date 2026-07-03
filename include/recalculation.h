#ifndef RECALCULATION_H
#define RECALCULATION_H

typedef struct sheet sheet;
typedef struct cell cell;

void trigger_recalculation(sheet *s);
void recalculate_cells(sheet *s, cell **order, int len);
void mark_children_dirty(sheet *s ,cell * target);
void add_to_calculation_chain(sheet *s, cell *c);
void remove_from_calculation_chain(sheet *s, cell *c);

void update_topological_ranks(cell *target,sheet *s);
int compare_cells(const void *a , const void *b);
void sort_calculation_chain(sheet *s);
#endif
