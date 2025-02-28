#include <stdio.h>
#include <assert.h>
#include "../include/utils.h"  // include your utils header
#include "../include/input_handler.h"

void test_my_strdup(void){

    char *orig="Hello, Spreadsheet!";
    char *dup=my_strdup(orig);

    // asserting whether the duplicate string is equal to original string
    assert(dup!=NULL);
    assert(strcmp(orig, dup)==0);

    free(dup);  // free allocated memory
    printf("PASS: my_strdup\n");
}
void test_max_min(void){

    assert(max(5, 10)==10);
    assert(max(-1, -10)==-1);
    assert(min(5, 10)==5);
    assert(min(-1, -10)==-10);
    printf("PASS: max & min\n");
}

void test_remove_space(void){
    char str1[]="  He llo W orl d  ";
    remove_space(str1);
    // checking if the string is equal to "HelloWorld"
    assert(strcmp(str1, "HelloWorld")==0);
    
    printf("PASS: remove_space\n");
}

void test_is_integer(void){

    assert(is_integer("12512353")==true);
    assert(is_integer("-45114as6")==false);
    assert(is_integer("12.0133")==false);
    assert(is_integer("fgkjabk")==false);
    printf("PASS: is_integer\n");
}
void test_is_operator(void){

    assert(is_operator('+')==true);
    assert(is_operator('%')==false);
    assert(is_operator('-')==true);
    assert(is_operator('*')==true);
    assert(is_operator('a')==false);
    assert(is_operator('/')==true);

    printf("PASS: is_operator\n");
}

void test_is_cell_name(void){

    assert(is_cell_name("A1")==true);
    assert(is_cell_name("Z99")==true);
    assert(is_cell_name("AA100")==true);
    assert(is_cell_name("ZZZZ9")==false);
    assert(is_cell_name("Z")==false);
    assert(is_cell_name("349")==false);

    printf("PASS: is_cell_name\n");
}
void test_is_function_name(void){

    assert(is_function_name("SUM")==1);
    assert(is_function_name("AVG")==1);
    assert(is_function_name("MAX")==1);
    assert(is_function_name("MIN")==1);
    assert(is_function_name("slfjs")==0);
    assert(is_function_name("Sum")==0);
    assert(is_function_name("KLD")==0);
    assert(is_function_name("SLEEP")==2);
    assert(is_function_name("STDEV")==1);

    printf("PASS: is_function_name\n");
}

void test_cell_name_to_index(void){

    int row, col;
    cell_name_to_index("B2", &row, &col); 
    // B2 is 1,1
    assert(row==1 && col==1);

    cell_name_to_index("ZZZ999", &row, &col);
    // ZZZ999 is 1,1
    assert(row==998 && col==18277);

    printf("PASS: cell_name_to_index\n");
}
void test_col_index_to_name(void) {
    char *col_name=col_index_to_name(1);
    assert(strcmp(col_name, "B")==0);
    free(col_name);
    printf("PASS: col_index_to_name\n");

}

void test_parse_range(void) {
    Range r;  

    // Test single cell range  
    parse_range("A1", &r);  
    assert(strcmp(r.start_cell, "A1") == 0); 
    assert(strcmp(r.end_cell, "A1") == 0);  
    free(r.start_cell);  
    free(r.end_cell);  
    
    // Proper range test  
    parse_range("B2:D5", &r);
    assert(strcmp(r.start_cell, "B2") == 0);
    assert(strcmp(r.end_cell, "D5") == 0);
    
    free(r.start_cell);
    free(r.end_cell);


    // Another valid range  
    parse_range("Z10:AA20", &r);
    assert(strcmp(r.start_cell, "Z10") == 0);  
    assert(strcmp(r.end_cell, "AA20") == 0);
    
    free(r.start_cell);
    free(r.end_cell);


    // Large range test 
    parse_range("A1:ZZZ999", &r);
    assert(strcmp(r.start_cell, "A1") == 0);  
    assert(strcmp(r.end_cell, "ZZZ999") == 0);
    
    free(r.start_cell);
    free(r.end_cell);
    
    printf("PASS: parse_range\n");  
}
void test_is_valid_cell(void){
    assert(is_valid_cell(10, 10, "A1")==true);
    assert(is_valid_cell(10, 10, "Z10")==false);
    printf("PASS: is_valid_cell\n");
}

void test_index_to_cell_name(void) { 
    
    char *cell1 = index_to_cell_name(1, 1);
    assert(strcmp(cell1, "B2") == 0);  
    free(cell1);
    
    char *cell2 = index_to_cell_name(998, 18277);
    assert(strcmp(cell2, "ZZZ999") == 0);
    
    free(cell2);

    printf("PASS: index_to_cell_name\n");  
}


void test_calculate_arithmetic_expression(void){
    assert(calculate_arithmetic_expression("5+5")==10);
    assert(calculate_arithmetic_expression("-10*-3")==30);
    printf("PASS: calculate_arithmetic_expression\n");
}

int main(void){
    printf("Running tests for utils module...\n");

    test_my_strdup();
    test_max_min();
    test_remove_space();
    test_is_integer();
    test_is_operator();
//    test_is_arithmetic_expression();
//    test_is_cell_expression();
    test_is_cell_name();
    test_is_function_name();
    test_cell_name_to_index();
    test_col_index_to_name();
    test_parse_range();
    test_is_valid_cell();
    test_index_to_cell_name();
    test_calculate_arithmetic_expression();

    printf("All tests passed for utils module!\n");
    return 0;
}
