#ifndef FORMULA_PARSER_H
#define FORMULA_PARSER_H

#include "spreadsheet.h"

int eval_formula(sheet *s , char *vale1 , char *v2,char * op);
char **parse_formula(const char *formula , int *dep_count);


#endif
