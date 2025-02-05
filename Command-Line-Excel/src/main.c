#include "../include/display.h"
#include "../include/formula_parser.h"
#include "../include/input_handler.h"
#include "../include/recalculation.h"
#include "../include/scrolling.h"
#include "../include/spreadsheet.h"
#include "../include/utils.h"

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include <limits.h>

// insert code here...
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./sheet <num1> <num2>\n");
        return 1;
    }
    
    int num1 = atoi(argv[1]);
    int num2 = atoi(argv[2]);
    sheet *s = create_sheet(num1,num2);
    display_sheet(s);
    char *input = NULL;
    size_t len = 0;
    char command;
    
    while (true) {
        printf("Enter the command (or 'q' to quit): ");
        getline(&input, &len, stdin);
        
        // Remove newline character
        input[strcspn(input, "\n")] = 0;
        
        // Exit if user enters 'q'
        if (strcmp(input, "q") == 0) {
            printf("Exiting...\n");
            break;
        }
        
        // Parse input and call function
        if (sscanf(input, "%c" ,&command) == 1) {
             scroll(s, command);
//            printf("%d\n",is_cell_value_assignment(command));
             display_sheet(s);
        } else {
            printf("Invalid input! Try again.\n");
        }
    }
    
    free(input);
    
    return 0;
}
