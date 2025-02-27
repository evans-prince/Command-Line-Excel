#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/spreadsheet.h"
#include "../include/utils.h"

void test_create_sheet(void) {
    printf("Running test: create_sheet...\n");
    
    sheet *s = create_sheet(5, 5);
    if (s == NULL) {
        printf("FAIL: Sheet creation failed.\n");
        return;
    }

    if (s->num_rows == 5 && s->num_cols == 5 && s->grid != NULL) {
        printf("PASS: Sheet created with correct dimensions.\n");
    } else {
        printf("FAIL: Incorrect sheet dimensions or grid allocation.\n");
    }

    free_sheet(s);
}

void test_set_cell_value(void) {
    printf("Running test: set_cell_value...\n");

    sheet *s = create_sheet(5, 5);
    if (s == NULL) {
        printf("FAIL: Sheet creation failed.\n");
        return;
    }

    set_cell_value(s, "A1", 42);
    
    int row, col;
    cell_name_to_index("A1", &row, &col);
    if (s->grid[row][col].val == 42) {
        printf("PASS: Cell value set correctly.\n");
    } else {
        printf("FAIL: Cell value not set properly.\n");
    }

    free_sheet(s);
}

void test_add_dependency(void) {
    printf("Running test: add_dependency...\n");

    sheet *s = create_sheet(5, 5);
    if (s == NULL) {
        printf("FAIL: Sheet creation failed.\n");
        return;
    }

    char message[100];
    add_dependency(&s->grid[0][0], &s->grid[1][1], message);

    if (s->grid[0][0].num_children == 1 && s->grid[1][1].num_parents == 1) {
        printf("PASS: Dependency added correctly.\n");
    } else {
        printf("FAIL: Dependency not added properly.\n");
    }

    free_sheet(s);
}

void test_remove_dependencies(void) {
    printf("Running test: remove_dependencies...\n");

    sheet *s = create_sheet(5, 5);
    if (s == NULL) {
        printf("FAIL: Sheet creation failed.\n");
        return;
    }

    char message[100];
    add_dependency(&s->grid[0][0], &s->grid[1][1], message);
    remove_dependencies(&s->grid[0][0]);

    if (s->grid[0][0].num_children == 0 && s->grid[1][1].num_parents == 0) {
        printf("PASS: Dependencies removed correctly.\n");
    } else {
        printf("FAIL: Dependencies not removed properly.\n");
    }

    free_sheet(s);
}

void test_has_cycle(void) {
    printf("Running test: has_cycle...\n");

    sheet *s = create_sheet(5, 5);
    if (s == NULL) {
        printf("FAIL: Sheet creation failed.\n");
        return;
    }

    char message[100];
    add_dependency(&s->grid[0][0], &s->grid[1][1], message);
    add_dependency(&s->grid[1][1], &s->grid[0][0], message); // Circular dependency

    if (has_cycle(&s->grid[0][0], &s->grid[1][1])) {
        printf("PASS: Cycle detected correctly.\n");
    } else {
        printf("FAIL: Cycle detection failed.\n");
    }

    free_sheet(s);
}

int main(void) {
    printf("Running tests for spreadsheet module...\n");
    test_create_sheet();
    test_set_cell_value();
    test_add_dependency();
    test_remove_dependencies();
    test_has_cycle();
    printf("All spreadsheet tests completed!\n");
    return 0;
}
