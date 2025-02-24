#ifndef UTILS_H
#define UTILS_H

typedef struct sheet sheet;
typedef struct Range Range;

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include <limits.h>

double get_time(void);
char *my_strdup(const char *s);
int max ( int a , int b ) ;
int min ( int a , int b ) ;
void cell_name_to_index(const char* str, int *rowIndex, int* colIndex);
char *col_index_to_name(int col);
void remove_space(char *c);
bool is_integer(char *s);
bool is_operator(char s);
bool is_arithmetic_expression(char *s);
bool is_cell_expression(char *s);
bool is_cell_name(char *s);
int is_function_name(char *s);
void parse_range(const char *s,Range *r); // move this function from utils may be to formula parser
// new functions to write
int calculate_arithmetic_expression(const char * expr);
bool is_valid_cell(int num_rows, int num_cols, const char * cell);
int give_function_type(const char* fun_name);

int get_min(sheet *s, const Range*  range);
int get_max(sheet *s, const Range*  range);
int get_avg(sheet *s, const Range*  range);
int get_sum(sheet *s, const Range*  range);
int get_stdev(sheet *s, const Range*  range);

#endif
