#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include "formula_parser.h"
#include "spreadsheet.h"
#include "utils.h"

void test_eval_formula(void) {
    sheet *test_sheet = create_sheet(10, 10);
    
    // Assigning some values to cells for testing
    set_cell_value(test_sheet, "A1", 5);
    set_cell_value(test_sheet, "B2", 3);
    set_cell_value(test_sheet, "C3", 0); // For division by zero test

    assert(eval_formula(test_sheet, "A1", "5", "+") == 10);
    assert(eval_formula(test_sheet, "5", "A1", "+") == 10);
    assert(eval_formula(test_sheet, "B2", "A1", "*") == 15);

    assert(eval_formula(test_sheet, "", "2", "+") == INT_MAX);
    assert(eval_formula(test_sheet, "A1", "Z99", "+") == INT_MAX);
    assert(eval_formula(test_sheet, "5", "C3", "/") == INT_MIN); // Division by zero
    assert(eval_formula(test_sheet, "A1", "C3", "/") == INT_MIN); // Division by zero
    assert(eval_formula(test_sheet, "A1", "0", "/") == INT_MIN); // Division by zero
    assert(eval_formula(test_sheet, "5", "3", "#") == INT_MAX);
    assert(eval_formula(test_sheet, "XYZ", "3", "+") == INT_MAX);

    free_sheet(test_sheet);
    printf("eval_formula() passed all tests!\n");
}

void test_parse_formula(void) {
    int dep_count;

    // ✅ Valid cases
    char **res = parse_formula("A1+5", &dep_count);
    assert(dep_count == 2);
    assert(strcmp(res[0], "A1") == 0);
    assert(strcmp(res[1], "+") == 0);
    assert(strcmp(res[2], "5") == 0);
    free(res[0]); free(res[1]); free(res[2]); free(res);

    res = parse_formula("B2-C3", &dep_count);
    assert(dep_count == 2);
    assert(strcmp(res[0], "B2") == 0);
    assert(strcmp(res[1], "-") == 0);
    assert(strcmp(res[2], "C3") == 0);
    free(res[0]); free(res[1]); free(res[2]); free(res);

    res = parse_formula("B1", &dep_count);
    assert(dep_count == 1);
    assert(strcmp(res[0], "B1") == 0);
    free(res[0]); free(res);

    res = parse_formula("Z99*8", &dep_count);
    assert(dep_count == 2);
    assert(strcmp(res[0], "Z99") == 0);
    assert(strcmp(res[1], "*") == 0);
    assert(strcmp(res[2], "8") == 0);
    free(res[0]); free(res[1]); free(res[2]); free(res);

    printf("✅ parse_formula() passed all tests!\n");
}

int main(void) {
    test_eval_formula();
    test_parse_formula();
    printf("🎉 All formula_parser tests passed successfully!\n");
    return 0;
}
