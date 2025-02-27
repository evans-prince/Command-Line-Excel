#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/input_handler.h"

void test_is_quit_command(void) {
    printf("Running test: is_quit_command...\n");

    if (is_quit_command("q") && is_quit_command("Q")) {
        printf("PASS: Quit command recognized correctly.\n");
    } else {
        printf("FAIL: Quit command failed.\n");
    }

    if (!is_quit_command("quit") && !is_quit_command("exit") && !is_quit_command("w")) {
        printf("PASS: Non-quit commands correctly rejected.\n");
    } else {
        printf("FAIL: Non-quit commands not handled properly.\n");
    }
}

void test_is_scroll_command(void) {
    printf("Running test: is_scroll_command...\n");

    if (is_scroll_command("w") && is_scroll_command("a") &&
        is_scroll_command("s") && is_scroll_command("d")) {
        printf("PASS: Scroll commands recognized correctly.\n");
    } else {
        printf("FAIL: Scroll commands failed.\n");
    }

    if (!is_scroll_command("x") && !is_scroll_command("q") &&
        !is_scroll_command("move up")) {
        printf("PASS: Non-scroll commands correctly rejected.\n");
    } else {
        printf("FAIL: Non-scroll commands not handled properly.\n");
    }
}

void test_is_cell_value_assignment(void) {
    printf("Running test: is_cell_value_assignment...\n");

    if (is_cell_value_assignment("A1=10") && is_cell_value_assignment("B3=-5") &&
        is_cell_value_assignment("Z99=100")) {
        printf("PASS: Cell value assignment recognized correctly.\n");
    } else {
        printf("FAIL: Cell value assignment failed.\n");
    }

    if (!is_cell_value_assignment("A1+") && !is_cell_value_assignment("C2*3") &&
        !is_cell_value_assignment("SUM(A1:A3)") && !is_cell_value_assignment("10=A1")) {
        printf("PASS: Non-value assignments correctly rejected.\n");
    } else {
        printf("FAIL: Non-value assignments not handled properly.\n");
    }
}

void test_is_function_call(void) {
    printf("Running test: is_function_call...\n");

    if (is_function_call("A1=SUM(A1:A3)") && is_function_call("B22=AVG(B2:B4)") &&
        is_function_call("ZZZ999=STDEV(D1:D10)")) {
        printf("PASS: Function calls recognized correctly.\n");
    } else {
        printf("FAIL: Function calls failed.\n");
    }

    if (!is_function_call("A1=10") && !is_function_call("B3+4") &&
        !is_function_call("MAXA1:A3") && !is_function_call("SUM(A1)")) {
        printf("PASS: Non-function calls correctly rejected.\n");
    } else {
        printf("FAIL: Non-function calls not handled properly.\n");
    }
}

void test_is_cell_dependent_formula(void) {
    printf("Running test: is_cell_dependent_formula...\n");

    if (is_cell_dependent_formula("F2=A1+B2") && is_cell_dependent_formula("A1=C3*5") &&
        is_cell_dependent_formula("K11=-2+D4") && is_cell_dependent_formula("Z99=E5/10")) {
        printf("PASS: Cell dependent formulas recognized correctly.\n");
    } else {
        printf("FAIL: Cell dependent formulas failed.\n");
    }

    if (!is_cell_dependent_formula("5+5") && !is_cell_dependent_formula("=A1") &&
        !is_cell_dependent_formula("SUM(A1:A3)") && !is_cell_dependent_formula("A1==B2")) {
        printf("PASS: Non-formulas correctly rejected.\n");
    } else {
        printf("FAIL: Non-formulas not handled properly.\n");
    }
}

void test_find_input_type(void) {
    printf("Running test: find_input_type...\n");
    char error_message[100] = "";

    if (find_input_type("q", error_message) == QUIT_COMMAND) {
        printf("PASS: Quit command detected correctly.\n");
    } else {
        printf("FAIL: Quit command detection failed.\n");
    }

    if (find_input_type("w", error_message) == SCROLL_COMMAND) {
        printf("PASS: Scroll command detected correctly.\n");
    } else {
        printf("FAIL: Scroll command detection failed.\n");
    }

    if (find_input_type("A1=20", error_message) == CELL_VALUE_ASSIGNMENT) {
        printf("PASS: Cell value assignment detected correctly.\n");
    } else {
        printf("FAIL: Cell value assignment detection failed.\n");
    }

    if (find_input_type("Z9=SUM(A1:A3)", error_message) == FUNCTION_CALL) {
        printf("PASS: Function call detected correctly.\n");
    } else {
        printf("FAIL: Function call detection failed.\n");
    }

    if (find_input_type("A1=B1+B2", error_message) == CELL_DEPENDENT_FORMULA) {
        printf("PASS: Cell dependent formula detected correctly.\n");
    } else {
        printf("FAIL: Cell dependent formula detection failed.\n");
    }

    if (find_input_type("A2(=:B3)", error_message) == INVALID_INPUT) {
        printf("PASS: Invalid input detected correctly.\n");
    } else {
        printf("FAIL: Invalid input detection failed.\n");
    }
}

void test_create_and_free_input(void) {
    printf("Running test: create_input & free_input...\n");

    struct input *in = create_input();
    if (in != NULL) {
        printf("PASS: Input structure created successfully.\n");
    } else {
        printf("FAIL: Input structure creation failed.\n");
    }

    free_input(in);
    printf("PASS: Input structure freed successfully.\n");
}

void test_parse_input(void) { // !Need to be stress fully tested for several more inputs 
    printf("Running test: parse_input...\n");
    struct input *in = create_input();
    char error_message[100] = "";
    in->raw_input="A1=10";

    parse_input(in, error_message);
    if (in->input_type == CELL_VALUE_ASSIGNMENT && strcmp(in->cell_reference, "A1") == 0 &&
        strcmp(in->value, "10") == 0) {
        printf("PASS: parse_input handled cell assignment correctly.\n");
    } else {
        printf("FAIL: parse_input failed for cell assignment.\n");
    }

    in->raw_input="A1=SUM(A2:A3)";
    parse_input(in, error_message);
    if (in->input_type == FUNCTION_CALL && strcmp(in->function_name, "SUM") == 0 && strcmp(in->cell_reference, "A1")) {
        printf("PASS: parse_input handled function call correctly.\n");
    } else {
        printf("FAIL: parse_input failed for function call.\n");
    }

    in->raw_input="A1=SUM(s";
    parse_input(in, error_message);
    if (in->input_type == INVALID_INPUT) {
        printf("PASS: parse_input correctly detected invalid input.\n");
    } else {
        printf("FAIL: parse_input failed to detect invalid input.\n");
    }

    free_input(in);
}

int main(void) {
    printf("\nRunning tests for input_handler module...\n");
    
    test_is_quit_command();
    test_is_scroll_command();
    test_is_cell_value_assignment();
    test_is_function_call();
    test_is_cell_dependent_formula();
    test_find_input_type();
    test_create_and_free_input();
    test_parse_input();

    printf("All input_handler tests completed!\n");
    return 0;
}
