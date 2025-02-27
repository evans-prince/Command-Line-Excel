#include <stdio.h>
#include "../include/input_handler.h"  // Correct path

int main(void) {
    printf("Running test for input handler...\n");

    if (is_quit_command("q")) {
        printf("PASS: Quit command recognized.\n");
    } else {
        printf("FAIL: Quit command not recognized.\n");
    }

    if (!is_quit_command("hello")) {
        printf("PASS: Non-quit command correctly rejected.\n");
    } else {
        printf("FAIL: Non-quit command incorrectly accepted.\n");
    }

    return 0;
}
