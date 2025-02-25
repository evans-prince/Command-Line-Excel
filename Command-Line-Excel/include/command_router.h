#ifndef COMMAND_ROUTER_H
#define COMMAND_ROUTER_H

typedef struct sheet sheet;
#include<stdbool.h>

// typedef struct{
//     char status_message[100];
//     float elapsed_time;
// }CommandStatus;

void command_router(sheet *s , char* user_input, bool is_output_enabled);

#endif