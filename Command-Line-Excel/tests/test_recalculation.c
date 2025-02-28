#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include "../include/spreadsheet.h"
#include "../include/recalculation.h"
#include "../include/utils.h"

void set_formula_directly(sheet *s, const char *cell_name, const char *formula){
    int row, col;
    cell_name_to_index(cell_name, &row, &col);
    s->grid[row][col].formula=my_strdup(formula);
    s->grid[row][col].dirty=true;
    add_to_calculation_chain(s, &s->grid[row][col]);
}
void test_large_dependency_chain(void){
    sheet *test_sheet=create_sheet(50, 50);

    // Creating a dependency chain of 1000 cells
    char cell_name[5];
    for(int i=1; i<=1000; i++){
        sprintf(cell_name, "A%d", i);
        int row, col;
        cell_name_to_index(cell_name, &row, &col);
        
        if(i==1){
            test_sheet->grid[row][col].val=i;
        }
        else{
            char formula[10];
            sprintf(formula, "A%d+1", i-1);
            set_formula_directly(test_sheet, cell_name, formula);
        }
    }

    trigger_recalculation(test_sheet);
    
    int row, col;
    cell_name_to_index("A1000", &row, &col);
    assert(test_sheet->grid[row][col].val==1000);
    printf("test_large_dependency_chain passed!\n");

    free_sheet(test_sheet);

}
void test_circular_dependency(void){
    sheet *test_sheet=create_sheet(5, 5);

    set_formula_directly(test_sheet, "A1", "A2+1");
    set_formula_directly(test_sheet, "A2", "A1+1");

    trigger_recalculation(test_sheet);
    
    assert(strcmp(test_sheet->status.status_message, "Invalid formula")==0);
    printf("test_circular_dependency passed!\n");

    free_sheet(test_sheet);
}

void test_topological_sort_large(void){
    sheet *test_sheet=create_sheet(10, 10);

    set_formula_directly(test_sheet, "A1", "A2+1");
    set_formula_directly(test_sheet, "A2", "A3+1");
    set_formula_directly(test_sheet, "A3", "A4+1");
    set_formula_directly(test_sheet, "A4", "A5+1");

    int row, col;
    cell_name_to_index("A1", &row, &col);
    update_topological_ranks(&test_sheet->grid[row][col]);
    cell_name_to_index("A5", &row, &col);
    assert(test_sheet->grid[row][col].topological_rank==0);
    
    cell_name_to_index("A4", &row, &col);
    assert(test_sheet->grid[row][col].topological_rank==1);
    
    cell_name_to_index("A3", &row, &col);
    assert(test_sheet->grid[row][col].topological_rank==2);
    cell_name_to_index("A2", &row, &col);
    assert(test_sheet->grid[row][col].topological_rank==3);
    
    cell_name_to_index("A1", &row, &col);
    assert(test_sheet->grid[row][col].topological_rank==4);

    printf("test_topological_sort_large passed!\n");

    free_sheet(test_sheet);
}

void test_memory_allocation(void){
    sheet *test_sheet=create_sheet(100, 100);

    for(int i=0; i<100; i++){
        for(int j=0; j<100; j++){
            test_sheet->grid[i][j].val=i+j;
        }
    }

    free_sheet(test_sheet);
    printf("test_memory_allocation() passed!\n");
}
void test_remove_from_calculation_chain_stress(void){
    sheet *test_sheet=create_sheet(20, 20);

    for(int i=0; i<20; i++){
        test_sheet->grid[0][i].val=i+1;
        add_to_calculation_chain(test_sheet, &test_sheet->grid[0][i]);
    }

    for(int i=0; i<20; i++){
        remove_from_calculation_chain(test_sheet, &test_sheet->grid[0][0]);
    }

    assert(test_sheet->num_dirty_cells==0);
    printf("test_remove_from_calculation_chain_stress passed\n");

    free_sheet(test_sheet);
}

void test_sort_calculation_chain_large(void){
    sheet *test_sheet=create_sheet(10, 10);

    for(int i=1; i<=10; i++){
        test_sheet->grid[0][i-1].val=i;
        test_sheet->grid[0][i-1].topological_rank=10-i;
        add_to_calculation_chain(test_sheet, &test_sheet->grid[0][i-1]);
    }

    sort_calculation_chain(test_sheet);

    for(int i=0; i<10; i++){
        assert(test_sheet->calculation_chain[i]->topological_rank==i);
    }

    printf("test_sort_calculation_chain_large passed\n");

    free_sheet(test_sheet);
}

int main(void){
    printf("Running stress tests...\n");

    test_large_dependency_chain();
    test_circular_dependency();
    test_topological_sort_large();
    test_memory_allocation();
    test_remove_from_calculation_chain_stress();
    test_sort_calculation_chain_large();

    printf("All stress tests passed successfully!\n");
    return 0;
}
