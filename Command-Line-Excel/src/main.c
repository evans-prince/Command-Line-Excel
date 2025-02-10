#include "../include/command_router.h"
#include "../include/display.h"
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
    
    if (num1 <= 0 || num2 <= 0) {
        printf("Error: Number of rows and columns must be positive integers.\n");
        return 1;
    }
    
    sheet *s = create_sheet(num1,num2);
    
    if (s == NULL) {
        printf("Error: Failed to create the spreadsheet.\n");
        return 1;
    }
    
    display_sheet(s);
    
    char *input = NULL;
    size_t len = 0;
    
    while (true) {
        printf("Enter the command (or 'q' to quit): ");
        getline(&input, &len, stdin);
        
        // Remove newline character
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "q") == 0 || strcmp(input, "quit") == 0) {
            printf("Exiting program. Goodbye!\n");
            break;
        }
        
        command_router(s, input, true);
        
    }
    
    free(input);
    
    return 0;
}
