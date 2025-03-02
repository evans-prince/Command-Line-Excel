#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "spreadsheet.h"
#include "recalculation.h"

void test_recalculation() {
    printf("Running recalculation tests...\n");

    // Step 1: Create a 5x5 sheet
    sheet *s = create_sheet(5, 5);
    assert(s != NULL);

    // Step 2: Set values and dependencies
    set_cell_value(s, "A1", 10);
    set_cell_value(s, "B1", 5);

    // Add dependency: C1 = A1 + B1
    CellRange range1 = {0, 0, 0, 1}; // A1 to B1
    char msg1[100];
    add_range_dependency(s, 0, 2, &range1, msg1); // C1 depends on A1 and B1
    assert(s->grid[0][2].num_parent_ranges == 1);

    // Step 3: Modify A1 and trigger recalculation
    set_cell_value(s, "A1", 20);
    trigger_recalculation(s);

    // Step 4: Verify updated values
    assert(s->grid[0][2].dirty == false);
    printf("PASS\n");

    // Step 5: Cycle Detection Test
    bool cycle_detected = has_cycle(&s->grid[0][2], &s->grid[0][2], s);
    assert(cycle_detected == true);
    printf(" PASS\n");

    // Step 6: Free memory
    free_sheet(s);
    printf("PASS\n");
}

int main() {
    test_recalculation();
    return 0;
}
