#include <stdio.h>
#define BUF_SIZE 128

void *readThread(void *arg)
{
	struct client_t *client = ((struct client_t *)arg);
	char buf[BUF_SIZE];
	int status;

	while (1)
	{
		if (returnInputStatus(client, socket) == 1) break;	
		if(readBuffer() == 1) break;
		if(SanitizeBufferInput() == 1) break;
		ExecuteBufferInput();
	}

	printf("Terminate Pthread for reading\n");
	client->rxState = 0;
	return NULL;
}

int returnInputStatus(client, socket)
{

	ssize_t numOfBytes;
	numOfBytes = read(client->socket, buf, BUF_SIZE);
	if (0 == numOfBytes)
	{
		printf("client closed the socket end\n");
		return 1;
	}
	else if (-1 == numOfBytes)
	{
		perror("ReadThread read() fails: ");
		return 1;
	}
	return 0;
}

int SanitizeBufferInput(char* buf){
		printf("TODO SANITIZE BUFFER INPUT")
		
		return 0;
}

int ExecuteBufferInput(char* buf){

		if (0 == strncmp(buf, ":exit", strlen(":exit")))
		{
			printf("Client exit\n");
			return 1;
		}
		
		return 0;
}