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
    
    if (num1 > 999 || num2 > 18278) {
        printf("Error: Number of rows and columns must be smaller then 999 and 18278 respectively.\n");
        return 1;
    }
    sheet *s = create_sheet(num1,num2);
    bool is_output_enabled=true;
    
    if (s == NULL) {
        printf("Error: Failed to create the spreadsheet.\n");
        return 1;
    }

    display_sheet(s);
    
    double end_time=get_time();
    double elapsed_time=end_time-start_time;
    s->status.elapsed_time=elapsed_time;
    
    char *input = NULL;
    size_t len = 0;
    
    while (true) {
        printf("[%0.1f] (%s) > ",s->status.elapsed_time,s->status.status_message);
        getline(&input, &len, stdin);
        
        // Remove newline character
        input[strcspn(input, "\n")] = 0;

        if(strcmp(input, "disable_output")==0){
            is_output_enabled=false;
            strcpy(s->status.status_message, "ok");
            continue;
        }
        
        if(strcmp(input, "enable_output")==0){
            is_output_enabled=true;
            strcpy(s->status.status_message, "ok");
            display_sheet(s);
            continue;
        }
        
        s->status.elapsed_time=0.0;
        strcpy(s->status.status_message, "ok");


        command_router(s, input, is_output_enabled);
        
    }
    
    // code should never be here , hence crash the software if it comes here
    assert(false);
    return 0;
}

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

// The browser demo keeps one sheet alive across many calls from JS, unlike
// the native CLI which owns `s` for the lifetime of a single process.
static sheet *wasm_sheet = NULL;

EMSCRIPTEN_KEEPALIVE
void wasm_create_sheet(int rows, int cols) {
    if (rows <= 0) rows = 1;
    if (cols <= 0) cols = 1;
    if (rows > 999) rows = 999;
    if (cols > 18278) cols = 18278;

    if (wasm_sheet) {
        free_sheet(wasm_sheet);
    }
    wasm_sheet = create_sheet(rows, cols);
}

EMSCRIPTEN_KEEPALIVE
void wasm_execute(char *input) {
    if (!wasm_sheet || !input) return;

    // command_router's QUIT_COMMAND case does free_sheet(s); exit(0);
    // In the native CLI that's fine because the process is ending anyway.
    // In the browser wasm_sheet must stay alive for the next call, so a
    // bare "q" (the only string is_quit_command() recognizes) is
    // intercepted here instead of being forwarded.
    char trimmed[8];
    strncpy(trimmed, input, sizeof(trimmed) - 1);
    trimmed[sizeof(trimmed) - 1] = '\0';
    remove_space(trimmed);
    if (strcmp(trimmed, "q") == 0) {
        strcpy(wasm_sheet->status.status_message, "quit is disabled in the browser demo");
        return;
    }

    command_router(wasm_sheet, input, false);
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_cell(int row, int col) {
    if (!wasm_sheet || row < 0 || row >= wasm_sheet->num_rows || col < 0 || col >= wasm_sheet->num_cols) {
        return 0;
    }
    return wasm_sheet->grid[row][col].val;
}

EMSCRIPTEN_KEEPALIVE
const char *wasm_get_formula(int row, int col) {
    if (!wasm_sheet || row < 0 || row >= wasm_sheet->num_rows || col < 0 || col >= wasm_sheet->num_cols) {
        return "";
    }
    cell *c = &wasm_sheet->grid[row][col];
    return c->formula ? c->formula : "";
}

EMSCRIPTEN_KEEPALIVE
const char *wasm_get_status(void) {
    return wasm_sheet ? wasm_sheet->status.status_message : "no sheet";
}

EMSCRIPTEN_KEEPALIVE
double wasm_get_elapsed(void) {
    return wasm_sheet ? (double)wasm_sheet->status.elapsed_time : 0.0;
}

EMSCRIPTEN_KEEPALIVE
int wasm_num_rows(void) { return wasm_sheet ? wasm_sheet->num_rows : 0; }

EMSCRIPTEN_KEEPALIVE
int wasm_num_cols(void) { return wasm_sheet ? wasm_sheet->num_cols : 0; }

EMSCRIPTEN_KEEPALIVE
int wasm_first_row(void) { return wasm_sheet ? wasm_sheet->bounds.first_row : 0; }

EMSCRIPTEN_KEEPALIVE
int wasm_first_col(void) { return wasm_sheet ? wasm_sheet->bounds.first_col : 0; }

#endif
