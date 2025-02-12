#include "../include/formula_parser.h"
#include "../include/utils.h"
#include "../include/spreadsheet.h"

#include<string.h>
#include<ctype.h>

// Evaluating the formula
int eval_formula(sheet *s, char *value1, char *value2, char *op){ 
    // value1=cell, value2=number
    // value1=number, value2=cell
    // value1=cell, value2=cell
    // value1=cell ,value2=NULL, op=NULL
    if(value1==NULL || strlen(value1)==0){
        return INT_MIN; 
    }

    int a,b;

    // Getting the value for a
    if(is_cell_name(value1)){
        if(!is_valid_cell(s->num_rows,s->num_cols,value1)){
            fprintf(stderr, "Invalid cell reference: %s\n", value1);
            return INT_MIN;
        }
        int row_idx, col_idx;
        cell_name_to_index(value1, &row_idx, &col_idx);

        a=s->grid[row_idx][col_idx].val; // Get value from cell
        
    }
    else if(is_integer(value1)){
        a=atoi(value1); // Convert string to integer
    }
    else{
        fprintf(stderr, "Invalid value1: %s\n", value1);
        return INT_MIN;
    }

    // Getting the value for b
    if(is_cell_name(value2)){
        if(is_valid_cell(s->num_rows,s->num_cols,value2)){
            fprintf(stderr, "Invalid cell reference: %s\n", value2);
            return INT_MIN;
        }
        int row_idx, col_idx;
        cell_name_to_index(value2, &row_idx, &col_idx);

        b=s->grid[row_idx][col_idx].val; // Get value from cell
    }
    else if(is_integer(value2)){
        b=atoi(value2); // Convert string to integer
    }
    else{
        fprintf(stderr, "Invalid value1: %s\n", value2);
        return INT_MIN;
    }

    if(op!=NULL){
        switch(*op){
            case '+':
                return a+b; // Addition
            case '-':
                return a-b; // Subtraction
            case '*':
                return a*b; // Multiplication
            case '/':
                if(b==0){
                    fprintf(stderr, "Division by zero error\n"); 
                    return INT_MIN;
                }
                return a/b; // Division
            
            default:
                fprintf(stderr, "Invalid operator: %c\n", *op);
                return INT_MIN; 
        }
    }
    
    return a; // Return the value1 if no operator
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
