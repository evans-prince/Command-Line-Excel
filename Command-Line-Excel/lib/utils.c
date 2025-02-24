#include "../include/utils.h"
#include "../include/input_handler.h"
#include "../include/spreadsheet.h"

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<limits.h>
#include<ctype.h>
#include<math.h>
#include<unistd.h>
#include<errno.h>

int get_min(sheet *s, const Range*  range){
    char *start_cell=range->start_cell;
    char *end_cell=range->end_cell;

    int start_row, start_col, end_row, end_col;
    cell_name_to_index(start_cell, &start_row, &start_col);
    cell_name_to_index(end_cell, &end_row, &end_col);

    int min_val=INT_MAX;
    for(int i=start_row;i<=end_row;i++){
        for(int j=start_col;j<=end_col;j++){
            if(s->grid[i][j].val==INT_MIN){
                continue;
            }
            min_val=min(min_val,s->grid[i][j].val);
        }
    }
    return min_val;
}

int get_max(sheet *s, const Range*  range){
    char *start_cell=range->start_cell;
    char *end_cell=range->end_cell;

    int start_row, start_col, end_row, end_col;
    cell_name_to_index(start_cell, &start_row, &start_col);
    cell_name_to_index(end_cell, &end_row, &end_col);

    int max_val=INT_MIN;
    for(int i=start_row;i<=end_row;i++){
        for(int j=start_col;j<=end_col;j++){
            if(s->grid[i][j].val==INT_MAX){
                continue;
            }
            max_val=max(max_val,s->grid[i][j].val);
        }
    }
    return max_val;
}

int get_avg(sheet *s, const Range*  range){
    char *start_cell=range->start_cell;
    char *end_cell=range->end_cell;

    int start_row, start_col, end_row, end_col;
    cell_name_to_index(start_cell, &start_row, &start_col);
    cell_name_to_index(end_cell, &end_row, &end_col);

    int sum=0;
    int num=(end_col-start_col+1)*(end_row-start_row+1);
    for(int i=start_row;i<=end_row;i++){
        for(int j=start_col;j<=end_col;j++){
            if(s->grid[i][j].val==INT_MIN || s->grid[i][j].val==INT_MAX){
                continue;
            }
            sum+=s->grid[i][j].val;
        }
    }
    return sum/num;
}

int get_sum(sheet *s, const Range*  range){
    char *start_cell=range->start_cell;
    char *end_cell=range->end_cell;

    int start_row, start_col, end_row, end_col;
    cell_name_to_index(start_cell, &start_row, &start_col);
    cell_name_to_index(end_cell, &end_row, &end_col);

    int sum=0;
    for(int i=start_row;i<=end_row;i++){
        for(int j=start_col;j<=end_col;j++){
            if(s->grid[i][j].val==INT_MIN || s->grid[i][j].val==INT_MAX){
                continue;
            }
            sum+=s->grid[i][j].val;
        }
    }
    return sum;
}

int get_stdev(sheet *s, const Range*  range){
    char *start_cell=range->start_cell;
    char *end_cell=range->end_cell;

    int start_row, start_col, end_row, end_col;
    cell_name_to_index(start_cell, &start_row, &start_col);
    cell_name_to_index(end_cell, &end_row, &end_col);

    int mean=get_avg(s,range);
    int num=(end_col-start_col+1)*(end_row-start_row+1);

    double variance=0.0;
    for(int i=start_row;i<=end_row;i++){
        for(int j=start_col;j<=end_col;j++){
            if(s->grid[i][j].val==INT_MIN || s->grid[i][j].val==INT_MAX){
                continue;
            }
            variance+=(s->grid[i][j].val-mean)*(s->grid[i][j].val-mean);
        }
    }
    variance/=num;

    return (int)round(sqrt(variance));
}

int give_function_type(const char* fun_name){
    if(strcmp(fun_name, "MIN")==0){
        return 0;
    }
    if(strcmp(fun_name, "MAX")==0){
        return 1;
    }
    if(strcmp(fun_name, "AVG")==0){
        return 2;
    }
    if(strcmp(fun_name, "SUM")==0){
        return 3;
    }
    if(strcmp(fun_name, "STDEV")==0){
        return 4;
    }
    if(strcmp(fun_name, "SLEEP")==0){
        return 5;
    }
    return -1;
}

char *index_to_cell_name(int i, int j){
    char *col_name = col_index_to_name(j);

    char row_str[4];
    int row = i + 1, len = 0;
    while (row > 0) {
        row_str[len++] = '0' + (row % 10);
        row /= 10;
    }
    row_str[len] = '\0';

    for (int k = 0; k < len / 2; k++) {
        char temp = row_str[k];
        row_str[k] = row_str[len - k - 1];
        row_str[len - k - 1] = temp;
    }

    char *name = (char *)malloc(strlen(col_name) + len + 1);
    
    strcpy(name, col_name);
    strcat(name, row_str);

    free(col_name);

    return name;
}

double get_time(void) {
    unsigned int start = sleep(0); // Approximate time marker in seconds
    return (double)start;  // Convert to floating-point for decimal accuracy
}
bool is_valid_cell(int num_rows, int num_cols, const char * cell){
    if(cell==NULL){
        return false;
    }

    int row_idx, col_idx;
    cell_name_to_index(cell, &row_idx, &col_idx);

    if(row_idx<num_rows && col_idx<num_cols && row_idx>=0 && col_idx>=0){
        return true;
    }

    return false;
}

// function which calculate a string of arithmetic expression int int , return INTMIN if error
int calculate_arithmetic_expression(const char * expr){
    if(expr==NULL || strlen(expr)==0){
        return INT_MAX;
    }
    char *s=my_strdup(expr);
    
    int value1;

    char *f=strpbrk(s+1,"+-/*"); 
    if(f==NULL){
        value1=atoi(s);
        free(s);
        return value1;
    }

    char op=*f;
    *f='\0';
    value1=atoi(s);

    int value2;
    char *c=f+1;
    value2=atoi(c);

    free(s);

    switch(op){
        case '+':
            return value1+value2; // Addition
        case '-':
            return value1-value2; // Subtraction
        case '*':
            return value1*value2; // Multiplication
        case '/':
            if(value2==0){
                // fprintf(stderr, "Division by zero error\n"); 
                return INT_MIN;
            }
            return value1/value2; // Division
        
        default:
            // fprintf(stderr, "Invalid operator: %c\n", op);
            return INT_MAX;
    }

    return INT_MAX;
}

char *my_strdup(const char *s) {
    if (s == NULL) return NULL;
    char *copy =(char *)malloc(strlen(s) + 1);
    if (copy) strcpy(copy, s);
    return copy;
}

// function to extract row index and col index from cell name string
void cell_name_to_index(const char* str, int *rowIndex, int* colIndex){
    //seprating the uppercase col name and integer row name

    char * colname=str;
    *rowIndex=0;
    *colIndex=0;
    
    for(int i=1; i<=3;i++){
        if(isupper(*str)){
            str++;
        }else{
            break;
        }
    }
    
    char * number = str;
    *rowIndex= atoi(number)-1;
    
    str--;
    // setting col index from col name
    int i=0;
    while(str >= colname){
        *colIndex+=pow(26,i) * ( *str -'A' +1);
        i++;
        str--;
    }
    
    *colIndex-=1;
    return;
}

// Function to convert column index to name
char *col_index_to_name(int col){
    
    // Allocate memory for the name
    char *name=(char *)malloc(4*sizeof(char));
    
    // Convert the column index to name
    int i=0;
    while(col>=0){
        name[i++]='A'+((col)%26);
        col=(col/26)-1;
    }
    name[i]='\0';
    
    // Reverse the name
    int j=0;
    i--;
    while(j<i){
        char temp=name[i];
        name[i]=name[j];
        name[j]=temp;
        j++;i--;
    }
    
    return name;
}

int max ( int a , int b ) {
    return (a>=b) ? a : b;
}

int min ( int a , int b ) {
    return (a>=b) ? b : a;
}

// Function to remove spaces from a string
void remove_space(char *c){
    char *i = c, *j = c;
    while (*j != '\0') {
        if (*j != ' ') {
            *i++ = *j;
        }
        j++;
    }
    *i = '\0';
    return;
}

// Function to check if a string is an integer
bool is_integer(char *s){
    if(s==NULL || strlen(s)==0){
        return false; // Handle null input safely
    }
    
    char *endp;
    strtol(s,&endp,10); // Convert the string to a long integer
    if(*endp=='\0'){
        return true; // If the conversion is successful, the string is an integer
    }
    return false;
}

void parse_range(const char *s,Range *r){
    char *copy=my_strdup(s);
    char *colon=strchr(copy,':');
    *colon='\0';
    r->start_cell=my_strdup(copy);
    r->end_cell=my_strdup(colon+1);
    
    free(copy);
}

bool is_operator(char s){
    return (s=='+' || s=='-' || s=='*' || s=='/')?true:false;
}

// Function to check if a string is an explicit assignment or a mathematical expression
bool is_arithmetic_expression(char *s){
    if(s==NULL || strlen(s)==0){
        return false; // Handle null input safely
    }
    
    if(*s=='+' || *s=='-'){
        s++;
    }
    
    if(is_integer(s)){
        return true;
    }
    
    char *op=strpbrk(s,"+-/*");
    if(!op || op==s){
        return false;
    }
    
    *op='\0';
    char *value1=s;
    char *value2=(op+1);
    if(*value2=='+' || *value2=='-'){
        value2++;
    }
    
    if(is_integer(value1) && is_integer(value2)){
        return true;
    }
    
    return false;
}

// Function to check if a string is a valid cell name
bool is_cell_name(char *s){
    if(s==NULL || strlen(s)>6 || strlen(s)<2){
        return false; // Handle null input safely and check if the length of the string is greater than 6 (Maximum cell name 'ZZZ999')
    }
    
    bool flag_num=false, flag_alpha=false; // Flags to check if a number or alphabet has been seen
    int alpha_count=0, num_count=0; // Count of alphabets and numbers
    while(*s!='\0'){
        if(alpha_count>3 || num_count>3){ // If the count of alphabets or numbers exceeds 3
            return false;
        }
        if(!flag_num && isupper(*s) && *s>='A'){ // Checks for a continuous stream of alphabets
            s++;
            alpha_count++;
            flag_alpha=true;
        }
        else if(flag_alpha && isdigit(*s) && *s>='0'){ // Checks for a continuous stream of digits
            if(*s=='0' && num_count==0){ // If the first digit after alphabets is 0
                return false;
            }
            s++;
            num_count++;
            flag_num=true;
        }
        else{
            return false;
        }
    }
    return (alpha_count>0 && num_count>0)?true:false; // If both alphabets and numbers are present
}

// Function to check if a string is a valid function name
int is_function_name(char *s){
    if(s==NULL){
        return 0; // Handle null input safely
    }
    if(strcmp(s,"SUM")==0 || strcmp(s,"AVG")==0 || strcmp(s,"STDEV")==0 || strcmp(s,"MAX")==0 || strcmp(s,"MIN")==0){
        return 1;
    }
    else if(strcmp(s,"SLEEP")==0){
        return 2;
    }
    else{
        return 0;
    }
}

// Function to check if string is cell dependent expression like A1=B1 or A1 = 2*-B1
bool is_cell_expression(char *s){
    if(s==NULL || strlen(s)==0){
        return false;
    }
    
    if(*s == '+' || *s == '-'){
        s++;
    }
    
    char *op = strpbrk(s, "+-*/");
    if (!op || op == s) return false;
    
    char *value1;
    char *value2;
    *op = '\0';
    value1 = s;
    value2 = (op + 1);
    if(*value2=='+' || *value2=='-'){
        value2++;
    }
    
    if(is_cell_name(value1) && is_integer(value2)){
        return true;
    }
    if(is_cell_name(value2) && is_integer(value1)){
        return true;
    }
    if(is_cell_name(value1) && is_cell_name(value2)){
        return true;
    }
    
    return false;
}
