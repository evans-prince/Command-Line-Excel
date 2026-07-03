#include <stdio.h>
#include <assert.h>
#include "../include/spreadsheet.h"
#include "../include/utils.h"

void test_create_sheet(){
    sheet *s = create_sheet(3, 3);
    
    assert(s != NULL);
    assert(s->num_rows == 3);
    assert(s->num_cols == 3);
    assert(s->grid != NULL);
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            assert(s->grid[i][j].val == 0);
            assert(s->grid[i][j].dirty == false);
            assert(s->grid[i][j].formula == NULL);
        }
    }

    free_sheet(s);
    printf("PASS\n");
}

void test_set_cell_value(){
    sheet *s = create_sheet(3, 3);
    
    set_cell_value(s, "A1", 10);
    set_cell_value(s, "B2", 20);
    
    assert(s->grid[0][0].val == 10);
    assert(s->grid[1][1].val == 20);

    free_sheet(s);
    printf("PASS\n");
}

void test_add_range_dependency(){
    sheet *s = create_sheet(3, 3);
    char message[100];

    CellRange range = {0, 0, 0, 1};  // A1:B1
    add_range_dependency(s, 1, 1, &range, message); // B2 depends on A1:B1
    
    assert(s->grid[1][1].num_parent_ranges == 1);
    assert(s->grid[0][0].num_child_ranges == 1);
    
    free_sheet(s);
    printf("PASS\n");
}

void test_remove_range_dependencies(){
    sheet *s = create_sheet(3, 3);
    char message[100];

    CellRange range = {0, 0, 0, 1};
    add_range_dependency(s, 1, 1, &range, message);
    
    remove_range_dependencies(s, &s->grid[1][1]);

    assert(s->grid[1][1].num_parent_ranges == 0);
    
    free_sheet(s);
    printf("PASS\n");
}

void test_has_cycle(){
    sheet *s = create_sheet(3, 3);
    char message[100];

    CellRange range1 = {0, 0, 0, 0};  // A1
    CellRange range2 = {1, 1, 1, 1};  // B2

    add_range_dependency(s, 1, 1, &range1, message); // B2 depends on A1
    add_range_dependency(s, 0, 0, &range2, message); // A1 depends on B2

    assert(strcmp(message, "ok") != 0);
    
    free_sheet(s);
    printf("PASS\n");
}

int main(){
    test_create_sheet();
    test_set_cell_value();
    test_add_range_dependency();
    test_remove_range_dependencies();
    test_has_cycle();
    
    printf("\nAll tests passed successfully!\n");
    return 0;
}
