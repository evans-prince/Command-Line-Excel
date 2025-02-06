#ifndef UTILS_H
#define UTILS_H

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include <limits.h>

int max ( int a , int b ) ;
int min ( int a , int b ) ;
char *col_index_to_name(int col);
void remove_space(char *c);
bool is_integer(char *s);
bool is_operator(char s);
bool is_expression(char *s);
bool is_cell_name(char *s);
int is_function_name(char *s);
bool is_range(char *s);

#endif
