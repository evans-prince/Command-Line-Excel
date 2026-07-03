#include "../include/command_router.h"
#include "../include/display.h"
#include "../include/formula_parser.h"
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

// Emscripten-specific quirk fix, not present natively: for a "CELL=..."
// input whose right-hand side references one or more other cells (the same
// shape command_router's CELL_DEPENDENT_FORMULA case handles), this re-runs
// the exact same public has_cycle() check that update_dependencies() /
// add_range_dependency() would perform, so we know *before* calling
// command_router whether it's about to reject the assignment as a cycle.
//
// This exists because in the emcc-compiled build specifically, when
// command_router does detect a cycle, it correctly leaves the cell's value
// untouched (verified: no data corruption) but the status message ends up
// "ok" instead of "Cycle detected in dependencies" -- confirmed via a native
// gcc build (-O0 and -O2) that the identical scenario reports the message
// correctly there, so the underlying cycle-rejection logic itself is fine;
// only the message as compiled by emcc doesn't surface. Rather than touch
// the original has_cycle/update_dependencies/command_router logic, this
// duplicates just the check (using the same exported has_cycle()) so the
// browser demo's status text is accurate, then lets command_router run as
// normal either way.
static bool wasm_formula_would_cycle(sheet *s, const char *raw_input) {
    char buf[256];
    strncpy(buf, raw_input, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    remove_space(buf);

    char *eq = strchr(buf, '=');
    if (!eq) return false;

    size_t ref_len = (size_t)(eq - buf);
    if (ref_len == 0 || ref_len >= 8) return false;
    char cell_ref[8];
    memcpy(cell_ref, buf, ref_len);
    cell_ref[ref_len] = '\0';
    if (!is_cell_name(cell_ref) || !is_valid_cell(s->num_rows, s->num_cols, cell_ref)) {
        return false;
    }

    int dep_count = 0;
    char **deps = parse_formula(eq + 1, &dep_count);
    if (!deps) return false;

    int target_row, target_col;
    cell_name_to_index(cell_ref, &target_row, &target_col);
    cell *target = &s->grid[target_row][target_col];

    bool cycle = false;
    for (int i = 0; i < 3; i++) {
        if (deps[i] && is_cell_name(deps[i]) && is_valid_cell(s->num_rows, s->num_cols, deps[i])) {
            int dep_row, dep_col;
            cell_name_to_index(deps[i], &dep_row, &dep_col);
            cell *start_cell = &s->grid[dep_row][dep_col];
            if (has_cycle(start_cell, target, s)) {
                cycle = true;
                break;
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        free(deps[i]);
    }
    free(deps);

    return cycle;
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

    // Predict a cycle before calling command_router, purely so the status
    // text is correct afterward -- command_router still runs exactly as it
    // would have anyway (same dependency handling, same formula-string
    // behavior), this only patches up status_message once it returns.
    bool predicted_cycle = wasm_formula_would_cycle(wasm_sheet, input);

    command_router(wasm_sheet, input, false);

    if (predicted_cycle) {
        strcpy(wasm_sheet->status.status_message, "Cycle detected in dependencies");
    }
}

// Temporary diagnostics for tracking down the cycle-detection quirk -- not
// part of the permanent API, safe to remove once the underlying issue is
// understood. wasm_build_marker() lets us confirm the browser is actually
// running this exact build (rules out stale caching); wasm_debug_cycle_check
// calls the same prediction helper wasm_execute() uses, but returns the
// result directly instead of only using it to patch a status message, so it
// can be probed in isolation from the browser console.
EMSCRIPTEN_KEEPALIVE
const char *wasm_build_marker(void) {
    return "cycle-fix-debug-build-1";
}

EMSCRIPTEN_KEEPALIVE
int wasm_debug_cycle_check(char *input) {
    if (!wasm_sheet || !input) return -1;
    return wasm_formula_would_cycle(wasm_sheet, input) ? 1 : 0;
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
