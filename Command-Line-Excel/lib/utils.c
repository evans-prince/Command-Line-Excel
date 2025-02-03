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
