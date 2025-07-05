#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include "client_handler.h"
#include "client.h"
#include "../util/log.h"

#define PORT 666
#define BUF_SIZE 128
#define MAX_CLIENTS 1

pthread_t check_thread;
struct client_t current_client;
int server_sd;
int status;

int main(int argc, char *argv[])
{
	server_sd = Setup();
	Listen();
	exit(EXIT_SUCCESS);
}

int Setup(){
	// 1. Ignore SIGPIPE
	signal(SIGPIPE, SIG_IGN);
	int status;

	// 2. Create socket
	int server_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sd == -1)
	{
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	Log_msg("Socket created");

	// 3. turn off bind address checking
	int enable = 1;
	status = setsockopt(server_sd, SOL_SOCKET, SO_REUSEADDR, (int *)&enable,
						sizeof(enable));
	if (-1 == status)
	{
		perror("setsockopt error: ");
	}

	// 4. BIND the socket to an address
	//  Prepare the address
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT);

	status = bind(server_sd, (struct sockaddr *)&addr, sizeof(addr));
	if (-1 == status)
	{
		perror("Bind fails: ");
		close(server_sd);
		exit(EXIT_FAILURE);
	}
	Log_msg("Socket binded");

	// 5. Set backlog

	status = listen(server_sd, 1);

	if (-1 == status)
	{
		perror("Listen fails: ");
		close(server_sd);
		exit(EXIT_FAILURE);
	}

	Log_msg("Server listening");
	return server_sd;
}

void Listen(){
	while (1)
	{		
		current_client = AcceptClient(server_sd);
	}
}