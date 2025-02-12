#ifndef FORMULA_PARSER_H
#define FORMULA_PARSER_H

int eval_formula(char * expr);
char **parse_formula(const char *formula , int *dep_count);


#endif
