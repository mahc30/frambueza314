#include <stdio.h>
#include <stdlib.h>
#include "client_handler.h"
#include "client.h"
#include "server_commands.h"
#include "../util/log.h"

#define BUF_SIZE 128

int connected_users = 0;
struct client_t client;
pthread_t clientThread;

void *ReadThread(void *arg)
{
	Log_msg("Create Pthread for reading");
	struct client_t *client = ((struct client_t *)arg);
	char buf[BUF_SIZE];

	while (1)
	{
		if (ValidateClient() == 1)
			break;
		if (ValidateBufferInput(buf) == 1)
			break;
		if (ReadBuffer(buf) == 1)
			break;
		ExecuteBufferInput(buf);
	}

	Log_msg("Terminate Pthread for reading");
	HandleClientExit();
	return NULL;
}

int ReadBuffer(char *buf)
{
	ssize_t numOfBytes;
	numOfBytes = read(client.socket, buf, BUF_SIZE);
	if (0 == numOfBytes)
	{
		Log_msg("Client closed the socket end");
		HandleClientExit();
		return 1;
	}
	else if (-1 == numOfBytes)
	{
		perror("ReadThread read() fails: ");
		return 1;
	}

	// Log_msg("TODO SANITIZE BUFFER INPUT");
	return 0;
}

int ValidateBufferInput(char *buf)
{
	// Log_msg("TODO Validate BUFFER INPUT");
	return 0;
}

void ExecuteBufferInput(char *buf)
{
	if (strncmp(buf, ":exit", strlen(":exit")) == 0)
	{
		HandleClientExit();
	}
	else
	{
		ProcessCommand(buf);
	}
}

int ValidateClient()
{
	if (!&client)
	{
		Log_msg("client is NULL");
		return 1;
	}

	if (client.socket < 0)
	{
		Log_msg("Invalid socket: %d", client.socket);
		return 1;
	}

	return 0;
}

struct client_t AcceptClient(int server_sd)
{
	int client_sd;
	// 6. Accept:
	if (connected_users == 0)
	{
		Log_msg("Waiting for a client");
	}

	client_sd = accept(server_sd, NULL, NULL);

	if (connected_users > 0)
	{
		const char *reject_msg = "Server full. Try again later.\n";
		send(client_sd, reject_msg, strlen(reject_msg), 0);

		close(client_sd);
		return client;
	}

	if (-1 == client_sd)
	{
		perror("Accept fails: ");
		close(server_sd);
		exit(EXIT_FAILURE);
	}

	client.socket = client_sd;
	client.rxState = 1;
	Log_msg("Client Accepted");
	connected_users = 1;

	HandleNewClient(server_sd);

	return client;
}

void HandleNewClient(int server_sd)
{
	int status;

	if (!clientThread)
	{
		Log_msg("Starting new client thread");
		status = pthread_create(&clientThread,
								NULL,
								&ReadThread,
								&client);
	}

	if (status == -1)
	{
		perror("Pthread read fails: ");
		close(server_sd);
		exit(EXIT_FAILURE);
	}
}

void HandleClientExit()
{
	Log_msg("Client Exit");
	pthread_join(clientThread, NULL);
	clientThread = NULL;
	client.socket = -1;
	client.rxState = 0;
	connected_users = 0;
	// PrintClient();
}

void PrintClient()
{
	char clientInfo[1000];
	sprintf(clientInfo, "Client sd:%d rxState: %d", client.socket, client.rxState);
	Log_msg(clientInfo);
}