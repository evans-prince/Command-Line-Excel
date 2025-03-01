#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/input_handler.h"
#include "../include/utils.h"

void test_is_quit_command(void){
    printf("Running test: is_quit_command\n");

    if(is_quit_command("q")){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }

    if(!is_quit_command("quit") && !is_quit_command("exit") && !is_quit_command("w")){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }
}
void test_is_scroll_command(void){
    printf("Running test: is_scroll_command\n");

    if(is_scroll_command("w") && is_scroll_command("a") && is_scroll_command("s") && is_scroll_command("d")){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }

    if(!is_scroll_command("x") && !is_scroll_command("q") && !is_scroll_command("move up")){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }
}

void test_is_cell_value_assignment(void){
    printf("Running test: is_cell_value_assignment\n");

    if(is_cell_value_assignment("A1=10") && is_cell_value_assignment("B3=-5") && is_cell_value_assignment("Z99=100")){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }

    if(!is_cell_value_assignment("A1+") && !is_cell_value_assignment("C2*3") && !is_cell_value_assignment("SUM(A1:A3)") && !is_cell_value_assignment("10=A1")){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }
}
void test_is_function_call(void){
    printf("Running test: is_function_call\n");

    if(is_function_call("A1=SUM(A1:A3)") && is_function_call("B22=AVG(B2:B4)") && is_function_call("ZZZ999=STDEV(D1:D10)")){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }

    if(!is_function_call("A1=10") && !is_function_call("B3+4") && !is_function_call("MAXA1:A3") && !is_function_call("SUM(A1)")){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }

}

void test_is_cell_dependent_formula(void){
    printf("Running test: is_cell_dependent_formula\n");

    if(is_cell_dependent_formula("F2=A1+B2") && is_cell_dependent_formula("A1=C3*5") && is_cell_dependent_formula("K11=-2+D4") && is_cell_dependent_formula("Z99=E5/10")){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }

    if(!is_cell_dependent_formula("5+5") && !is_cell_dependent_formula("=A1") && !is_cell_dependent_formula("SUM(A1:A3)") && !is_cell_dependent_formula("A1==B2")){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }
}
void test_find_input_type(void){
    printf("Running test: find_input_type\n");
    char error_message[100]="";

    if(find_input_type("q", error_message)==QUIT_COMMAND){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }

    if(find_input_type("w", error_message)==SCROLL_COMMAND){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }

    if(find_input_type("A1=20", error_message)==CELL_VALUE_ASSIGNMENT){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }

    if(find_input_type("Z9=SUM(A1:A3)", error_message)==FUNCTION_CALL){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }

    if(find_input_type("A1=B1+B2", error_message)==CELL_DEPENDENT_FORMULA){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }

    if(find_input_type("A2(=:B3)", error_message)==INVALID_INPUT){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }
}

void test_create_and_free_input(void){
    printf("Running test: create_input & free_input\n");

    struct input *in=create_input();
    if(in!=NULL){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }

    free_input(in);
    printf("PASS\n");
}

void test_parse_input(void){
    printf("Running test: parse_input\n");
    struct input *in=create_input();
    char error_message[100]="";
    in->raw_input=my_strdup("A1=10");

    parse_input(in, error_message);
    if(in->input_type==CELL_VALUE_ASSIGNMENT && strcmp(in->cell_reference, "A1")==0 && strcmp(in->value, "10")==0){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }

    in->raw_input=my_strdup("A1=SUM(A2:A3)");
    parse_input(in, error_message);
    if(in->input_type==FUNCTION_CALL && strcmp(in->function_name, "SUM")==0 && strcmp(in->cell_reference, "A1")==0){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }

    in->raw_input=my_strdup("A1=SUM(s");
    parse_input(in, error_message);
    if(in->input_type==INVALID_INPUT){
        printf("PASS\n");
    }
    else{
        printf("FAIL\n");
    }

    free_input(in);
}

int main(void) {
    printf("\nRunning tests for input_handler\n");
    
    test_is_quit_command();
    test_is_cell_dependent_formula();
    test_find_input_type();
    test_parse_input();
    test_create_and_free_input();
    test_is_scroll_command();
    test_is_cell_value_assignment();
    test_is_function_call();

    printf("All input_handler tests completed!\n");
    return 0;
}
