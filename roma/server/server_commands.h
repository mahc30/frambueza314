#ifndef SERVER_COMMANDS_H
#define SERVER_COMMANDS_H
#include "client.h"
#include <stddef.h>  // for NULL

void ProcessCommand(char *command);
void Help();
#endif
