#include "../include/utils.h"

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include <limits.h>

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
    if(endp=='\0'){
        return true; // If the conversion is successful, the string is an integer
    }
    return false;
}

// Function to check if a string is an explicit assignment or a mathematical expression
bool is_expression(char *s){
    if(s==NULL){
        return false; // Handle null input safely
    }

    // If the first character is not a digit, the string is not a valid expression
    if(!isdigit(*s)){
        return false;
    }
    s++;

    bool flag=false; // Flag to check if an operator has been seen
    while(*s!='\0'){
        if(!flag && (*s=='+' || *s=='-' || *s=='*' || *s=='/')){ //If this is the first time an operator is seen
            flag=true;
            s++;
        }
        else if(isdigit(*s)){ // 
            s++;
        }
        else{
            return false;
        }
    }
    return true;
}

bool is_cell_name(char *s){
    if(s==NULL){
        return false; // Handle null input safely
    }

    bool flag_num=false;
    bool flag_alpha=false;
    while(*s!='\0'){
        if(!flag_num && isalpha(*s)){ // Checks for a continuous stream of alphabets
            s++;
            flag_alpha=true;
        } 
        else if(flag_alpha && isdigit(*s)){ // Checks for a continuous stream of digits
            s++;
            flag_num=true;
        }
        else{
            return false;
        }
    }
    return true;
}

