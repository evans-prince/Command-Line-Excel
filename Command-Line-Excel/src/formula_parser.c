#include "../include/formula_parser.h"
#include "../include/utils.h"

#include<string.h>
#include<ctype.h>

int eval_formula(sheet *s , char *vale1 , char *v2,char * op){
    
    return -123;
}

char **parse_formula(const char *original_formula , int *dep_count){ // char** ans is just a pointer of array of strings
    // this function needs to find and return all dependencies from formula
    // along with opetator or any integer value
    // it just returns Value1 operator Value2 seprately from a single string
    
    if(original_formula==NULL || dep_count==NULL){
        return NULL;
    }
    
    char * formula = my_strdup(original_formula);
    if(!formula){
        fprintf(stderr, "Memory allocation failed in parse_formula in copying formula.\n");
        return NULL;
    }
   
    int capacity =3;//max 2 cell name can come in formula
    // extra 1 is for operator
    
    char **dependencies = (char **) malloc(capacity*sizeof(char *));
    if(!dependencies){
        fprintf(stderr, "memory allocation falied in parse_formula of char **dependencies. \n");
        free(formula);
        return NULL;
    }
    
    *dep_count=0;
    char * op = strpbrk(formula, "+-*/");
    if(op==NULL){
        dependencies[0]=my_strdup(formula);
        if(!dependencies[0]){
            fprintf(stderr, "memory allocation failed in parse_formula.\n");
            free(dependencies);
            free(formula);
            return NULL;
        }
        *dep_count=1;
        free(formula);
        return dependencies;
    }
    
    dependencies[1]= (char *) malloc(2*sizeof(char));
    if(!dependencies[1]){
        fprintf(stderr, "Memory allocation faliled in parse_formula.\n");
        free(dependencies);
        free(formula);
        return NULL;
    }
    
    dependencies[1][0]=*op;
    dependencies[1][1]='\0';
    
    *op='\0';
    
    char * value1 = formula;
    char * value2 = op+1;
    
    dependencies[0]=my_strdup(value1);
    if(!dependencies[0]){
        fprintf(stderr, "memory allocation failed in parse_formula.\n");
        free(dependencies[1]);
        free(dependencies);
        free(formula);
        return NULL;
    }
    
    dependencies[2]=my_strdup(value2);
    if(!dependencies[2]){
        fprintf(stderr, "memory allocation failed in parse_formula.\n");
        free(dependencies[0]);
        free(dependencies[1]);
        free(dependencies);
        free(formula);
        return NULL;
    }
 
    *dep_count=2;
    free(formula);
    
    return dependencies;
}
