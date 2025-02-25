#include "../include/command_router.h"
#include "../include/display.h"
#include "../include/spreadsheet.h"
#include "../include/utils.h"

#include<stdlib.h>
#include<assert.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include <limits.h>
#include<unistd.h>


// insert code here...
int main(int argc, char *argv[]) {
    double start_time=get_time();

    if (argc != 3) {
        printf("Usage: ./sheet <num1> <num2>\n");
        return 1;
    }

    if(!is_integer(argv[1]) || !is_integer(argv[2])){
        printf("Error: Incorrectly running the program.\n");
        return 1;
    }

    int num1 = atoi(argv[1]);
    int num2 = atoi(argv[2]);
    
    if (num1 <= 0 || num2 <= 0) {
        printf("Error: Number of rows and columns must be positive integers.\n");
        return 1;
    }
    
    sheet *s = create_sheet(num1,num2);
    bool is_output_enabled=true;
    
    if (s == NULL) {
        printf("Error: Failed to create the spreadsheet.\n");
        return 1;
    }

    display_sheet(s);
    
    // double end_time=get_time();
    // double elapsed_time=end_time-start_time;
    // s->status.elapsed_time+=elapsed_time;

    // CommandStatus status;
    // status.elapsed_time=elapsed_time;
    // strcpy(status.status_message,"ok");
    
    char *input = NULL;
    size_t len = 0;
    
    while (true) {
        printf("[%0.1f] (%s) > ",s->status.elapsed_time,s->status.status_message);
        getline(&input, &len, stdin);
        
        // Remove newline character
        input[strcspn(input, "\n")] = 0;

        if(strcmp(input, "disable_output")==0){
            is_output_enabled=false;
            continue;
        }
        
        if(strcmp(input, "enable_output")==0){
            is_output_enabled=true;
            display_sheet(s);
            continue;
        }
        
        s->status.elapsed_time=0.0;
        command_router(s, input, is_output_enabled);
        
    }
    
    // code should never be here , hence crash the software if it comes here
    assert(false);
    return 0;
}
