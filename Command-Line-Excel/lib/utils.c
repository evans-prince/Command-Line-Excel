#include "../include/utils.h"

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<limits.h>
#include<ctype.h>
#include<math.h>


bool is_valid_cell(const char * cell){
    
    return true;
}

// function which calculate a string of arithmetic expression int int , return INTMIN if error
int calculate_arithmetic_expression(char * expr){
    
    return -123;
}

char *my_strdup(const char *s) {
    if (s == NULL) return NULL;
    char *copy = malloc(strlen(s) + 1);
    if (copy) strcpy(copy, s);
    return copy;
}

// function to extract row index and col index from cell name string
void cell_name_to_index(char* str, int *rowIndex, int* colIndex){
    //seprating the uppercase col name and integer row name
    char * colname=  str;
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
