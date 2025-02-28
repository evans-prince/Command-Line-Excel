#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/spreadsheet.h"
#include "../include/utils.h"

void test_create_sheet(void){
    printf("Running test: create_sheet\n");
    
    sheet *s=create_sheet(5, 5);
    if(s==NULL){
        printf("FAIL\n");
        return;
    }
    
    if(s->num_rows==5 && s->num_cols==5 && s->grid!=NULL){
        printf("PASS\n");
    }else{
        printf("FAIL\n");
    }
    
    free_sheet(s);
}
void test_set_cell_value(void){
    printf("Running test: set_cell_value\n");
    
    sheet *s=create_sheet(5, 5);
    if(s==NULL){
        printf("FAIL\n");
        return;
    }
    
    set_cell_value(s, "A1", 42);
    
    int row, col;
    cell_name_to_index("A1", &row, &col);
    if(s->grid[row][col].val==42){
        printf("PASS\n");
    }else{
        printf("FAIL\n");
    }
    
    free_sheet(s);
}

void test_add_dependency(void){
    printf("Running test: add_dependency\n");
    
    sheet *s=create_sheet(5, 5);
    if(s==NULL){
        printf("FAIL\n");
        return;
    }
    
    char message[100];
    add_dependency(&s->grid[0][0], &s->grid[1][1], message);
    
    if(s->grid[1][1].num_children==1 && s->grid[0][0].num_parents==1){
        printf("PASS\n");
    }else{
        printf("FAIL\n");
    }
    
    free_sheet(s);
}
void test_remove_dependencies(void){
    printf("Running test: remove_dependencies\n");
    
    sheet *s=create_sheet(5, 5);
    if(s==NULL){
        printf("FAIL\n");
        return;
    }
    
    char message[100];
    add_dependency(&s->grid[0][0], &s->grid[1][1], message);
    remove_dependencies(&s->grid[0][0]);
    
    if(s->grid[1][1].num_children==0 && s->grid[0][0].num_parents==0){
        printf("PASS\n");
    }else{
        printf("FAIL\n");
    }
    
    free_sheet(s);
}

void test_has_cycle(void){
    printf("Running test: has_cycle\n");
    
    sheet *s=create_sheet(5, 5);
    if(s==NULL){
        printf("FAIL\n");
        return;
    }
    
    char message[100];
    add_dependency(&s->grid[0][0], &s->grid[1][1], message);
    add_dependency(&s->grid[1][1], &s->grid[0][0], message); // circular dependency
    
    if(has_cycle(&s->grid[0][0], &s->grid[1][1])){
        printf("PASS\n");
    }else{
        printf("FAIL\n");
    }
    
    free_sheet(s);
}

void test_update_dependencies_valid(){
    printf("Running test: update_dependencies\n");
    
    sheet *s=create_sheet(5, 5);
    if(s==NULL){
        printf("FAIL\n");
        return;
    }
    
    char *deps[]={"B1", "C1"};
    char message[100]="";
    
    update_dependencies(s, "A1", deps, 2, message);
    
    int rowA, colA, rowB, colB, rowC, colC;
    cell_name_to_index("A1", &rowA, &colA);
    cell_name_to_index("B1", &rowB, &colB);
    cell_name_to_index("C1", &rowC, &colC);
    
    if(s->grid[rowA][colA].num_parents==2 &&
        s->grid[rowB][colB].num_children==1 &&
        s->grid[rowC][colC].num_children==1){
        printf("PASS\n");
    }else{
        printf("FAIL\n");
    }
    
    free_sheet(s);
}
void test_update_dependencies_invalid(void){
    printf("Running test: update_dependencies\n");
    
    sheet *s=create_sheet(5, 5);
    if(s==NULL){
        printf("FAIL\n");
        return;
    }
    
    char *deps[]={"Z1"}; // invalid cell reference
    char message[100]="";
    
    update_dependencies(s, "A1", deps, 1, message);
    
    if(strcmp(message, "Invalid cell reference")==0){
        printf("PASS\n");
    }else{
        printf("FAIL\n");
    }
    
    free_sheet(s);
}
void test_update_dependencies_cycle(void){
    printf("Running test: update_dependencies (Cycle Detection)\n");
    
    sheet *s=create_sheet(5, 5);
    if(s==NULL){
        printf("FAIL\n");
        return;
    }
    
    char *deps1[]={"B1"};
    char *deps2[]={"A1"}; // circular dependency
    
    char message[100]="";
    
    update_dependencies(s, "A1", deps1, 1, message);
    update_dependencies(s, "B1", deps2, 1, message);
    
    if(strcmp(message, "Cycle detected in dependencies")==0){
        printf("PASS\n");
    }else{
        printf("FAIL\n");
    }
    
    free_sheet(s);
}

int main(void){
    printf("Running tests for spreadsheet module\n");

    test_create_sheet();
    test_set_cell_value();
    test_add_dependency();
    test_remove_dependencies();
    test_has_cycle();
    printf("Running tests for update_dependencies\n");

    test_update_dependencies_valid();
    test_update_dependencies_invalid();
    test_update_dependencies_cycle();

    printf("All update_dependencies tests completed\n");
    printf("All spreadsheet tests completed\n");
}
