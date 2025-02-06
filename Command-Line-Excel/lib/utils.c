#include "../include/utils.h"

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<limits.h>
#include<ctype.h>

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
    if(s==NULL){
        return false; // Handle null input safely
    }

    char *endp;
    strtol(s,&endp,10); // Convert the string to a long integer
    if(*endp=='\0'){
        return true; // If the conversion is successful, the string is an integer
    }
    return false;
}

bool is_operator(char s){
    return (s=='+' || s=='-' || s=='*' || s=='/')?true:false;
}

// Function to check if a string is an explicit assignment or a mathematical expression
bool is_expression(char *s){
    if(s==NULL){
        return false; // Handle null input safely
    }

    // If the first character is something other than a digit or a '+' or a '-', the string is not a valid expression
    if(!(isdigit(*s) || *s=='+' || *s=='-')){
        return false;
    }
    if(*s=='+' || *s=='-'){
        s++;
    }
    
    bool flag_digit=false; // Flag to check if a digit has been seen
    bool flag_op=false; // Flags to check if operator has been seen
    int op_count=0; // Count of operators
    while(*s!='\0'){
        if(isdigit(*s)){ // If the character is a digit
            flag_digit=true;
            flag_op=false;
            s++;
        }
        else if(flag_digit && is_operator(*s)){ // If the character is an operator and the previous character was a digit
            op_count++;
            if(op_count>1){ // If the count of operators exceeds 1
                return false;
            }
            flag_digit=false;
            flag_op=true;
            s++;
        }
        else if(flag_op && (*s=='+' || *s=='-')){ // If the character is a '+' or '-' and the previous character was an operator
            flag_op=false;
            s++;
        }
        else{
            return false;
        }
    }
    if(flag_op){ // If the last character was an operator
        return false;
    }
    return true;
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
        else if(flag_alpha && isdigit(*s) && *s>'0'){ // Checks for a continuous stream of digits
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

bool is_range(char *s){ // ! To be edited
    return false;
}
