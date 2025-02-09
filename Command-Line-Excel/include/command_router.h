#ifndef COMMAND_ROUTER_H
#define COMMAND_ROUTER_H

#include"display.h"
#include"formula_parser.h"
#include"recalculation.h"
#include"scrolling.h"
#include"spreadsheet.h"
#include"utils.h"

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include <limits.h>

void command_router(sheet *s , char* user_input, bool is_output_enabled);

#endif
