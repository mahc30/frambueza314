#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <stddef.h>  // for NULL
#include <unistd.h>  // for ssize_t
#include "client.h"

#define BUF_SIZE 128

void *ReadThread(void *arg);
void *CheckClients(void *arg);

int ReadBuffer(char *buf);
int ValidateBufferInput(char *buf);
void ExecuteBufferInput(char *buf);
int ValidateClient();
struct client_t AcceptClient(int server_sd);
void ResetClient();
void HandleNewClient(int server_sd);

#endif // CLIENT_H
