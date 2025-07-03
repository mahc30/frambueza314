#include <stdio.h>
#include "client_handler.h"
#include "../util/log.h"

#define BUF_SIZE 128

void *readThread(void *arg)
{
	Log_msg("Create Pthread for reading\n");
	struct client_t *client = ((struct client_t *)arg);
	char buf[BUF_SIZE];
	int status;

	while (1)
	{
		Log_msg("LOOP\n");
        if(ValidateClient(client) == 1) break;
		if(ValidateBufferInput(buf) == 1) break;
		if(ReadBuffer(client, buf) == 1) break;	
		ExecuteBufferInput(buf);
	}

	Log_msg("Terminate Pthread for reading\n");
	client->rxState = 0;
	return NULL;
}

int ReadBuffer(struct client_t *client, char *buf)
{
	ssize_t numOfBytes;
	numOfBytes = read(client->socket, buf, BUF_SIZE);
	if (0 == numOfBytes)
	{
		Log_msg("client closed the socket end\n");
		return 1;
	}
	else if (-1 == numOfBytes)
	{
		perror("ReadThread read() fails: ");
		return 1;
	}

	Log_msg("TODO SANITIZE BUFFER INPUT\n");
	return 0;
}

int ValidateBufferInput(char* buf){
	Log_msg("TODO Validate BUFFER INPUT\n");
	return 0;
}

int ExecuteBufferInput(char* buf){
		if (0 == strncmp(buf, ":exit", strlen(":exit")))
		{
			Log_msg("Client exit\n");
			return 1;
		}
		if(0 == strncmp(buf, ":pene", strlen(":pene"))){
    	    Log_msg("PENEPE");
    	} 
		return 0;
}

int ValidateClient(struct client_t* client){
    if (!client) {
        Log_msg("client is NULL\n");
        return 1;
    }
    
    if (client->socket < 0) {
        Log_msg("Invalid socket: %d\n", client->socket);
        return 1;
    }
    return 0;
}