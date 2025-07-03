#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <stddef.h>  // for NULL
#include <unistd.h>  // for ssize_t

#define BUF_SIZE 128

// Forward declaration of client struct
struct client_t {
    int id;
    int socket;
    int rxState;
};

void *readThread(void *arg);
void *checkClients(void *arg);

int ReadBuffer(struct client_t *client, char *buf);
int ValidateBufferInput(char *buf);
int ExecuteBufferInput(char *buf);
int ValidateClient(struct client_t *client);

#endif // CLIENT_HANDLER_H
