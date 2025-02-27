#include <stdio.h>
#include <assert.h>
#include "../include/utils.h"  // Include your utils header

void test_my_strdup(void) {
    char *original = "Hello, Spreadsheet!";
    char *duplicate = my_strdup(original);

    assert(duplicate != NULL);
    assert(strcmp(original, duplicate) == 0);

    printf("PASS: my_strdup works correctly.\n");

    free(duplicate);  // Don't forget to free memory
}

void test_max_min(void) {
    assert(max(5, 10) == 10);
    assert(min(5, 10) == 5);
    printf("PASS: max and min functions work correctly.\n");
}

void test_remove_space(void) {
    char str[] = "  He llo W orl d  ";
    remove_space(str);
    assert(strcmp(str, "HelloWorld") == 0);
    printf("PASS: remove_space works correctly.\n");
}

void test_is_integer(void) {
    assert(is_integer("123") == true);
    assert(is_integer("-456") == true);
    assert(is_integer("12.3") == false);
    assert(is_integer("abc") == false);
    printf("PASS: is_integer correctly identifies integers.\n");
}

void test_is_operator(void) {
    assert(is_operator('+') == true);
    assert(is_operator('-') == true);
    assert(is_operator('*') == true);
    assert(is_operator('/') == true);
    assert(is_operator('%') == false);
    assert(is_operator('a') == false);
    printf("PASS: is_operator correctly identifies operators.\n");
}

void test_is_valid_cell(void) {
    assert(is_valid_cell(10, 10, "A1") == true);
    assert(is_valid_cell(10, 10, "Z10") == false);
    printf("PASS: is_valid_cell checks correctly.\n");
}

int main(void) {
    printf("Running tests for utils module...\n");

    test_my_strdup();
    test_max_min();
    test_remove_space();
    test_is_integer();
    test_is_operator();
    test_is_valid_cell();

    printf("All tests passed for utils module! 🎉\n");
    return 0;
}
