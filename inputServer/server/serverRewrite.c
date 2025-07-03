#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#define PORT 666
#define BUF_SIZE 128
#define MAX_CLIENTS 2

struct client_t
{
    int id;
    int socket;
    int rxState;
};

int connected_users = 0;
int client_threads[MAX_CLIENTS];
struct client_t clients[MAX_CLIENTS];
pthread_t read_threads[MAX_CLIENTS];
pthread_t check_threads[MAX_CLIENTS];

void *readThread(void *arg)
{
    printf("Initializing Reader Thread");
	struct client_t *client = ((struct client_t *)arg);
	char buf[BUF_SIZE];

	while (1)
	{
        if(ValidateClient(client) == 1) break;
		if(SanitizeBufferInput(buf) == 1) break;
		if(ReadBuffer(client, buf) == 1) break;	
		ExecuteBufferInput(buf);
	}

	printf("Terminate Pthread for reading\n");
	client->rxState = 0;
	return NULL;
}

int ReadBuffer(struct client_t* client, char* buf)
{
    ssize_t numOfBytes = read(client->socket, buf, BUF_SIZE);
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

int ValidateClient(struct client_t* client){
    if (!client) {
        fprintf(stderr, "client is NULL\n");
        return 1;
    }
    
    if (client->socket < 0) {
        fprintf(stderr, "Invalid socket: %d\n", client->socket);
        return 1;
    }
    return 0;
}

int SanitizeBufferInput(char* buf){
    if (!buf) {
        fprintf(stderr, "buf is NULL\n");
        return 1;
    }
    return 0;
}

void ExecuteBufferInput(char* buf){

    if (0 == strncmp(buf, ":exit", strlen(":exit")))
    {
        printf("Client exit\n");
    }
    if(0 == strncmp(buf, ":pene", strlen(":pene"))){
        printf("PENEPE");
    }    
}

void *check_clients(void *arg)
{
	struct client_t *client = arg;

	while (1)
	{
		if (0 == client->rxState)
		{
			printf("Client closed the socket\n");
			clients[client->id] = clients[client->id + 1];
			read_threads[client->id] = NULL;
			connected_users--;
			break;
		}
	}

	close(client->socket);
	return NULL;
}

int main(int argc, char *argv[])
{

	int status;
	int enable = 1;
	int server_sd;
	int client_sd;

	struct client_t client;

	// 1. Ignore SIGPIPE
	signal(SIGPIPE, SIG_IGN);

	// 2. Create socket
	server_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sd == -1)
	{
		perror("Socket creation failed\n");
		exit(EXIT_FAILURE);
	}
	printf("Socket created\n");

	// 3. turn off bind address checking
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
	printf("Socket binded\n");

	// 5. Set backlog

	status = listen(server_sd, 1);

	if (-1 == status)
	{
		perror("Listen fails: ");
		close(server_sd);
		exit(EXIT_FAILURE);
	}

	printf("Server listening\n");

	while (1)
	{
		// 6. Accept:
		printf("Waiting for a client\n");
		client_sd = accept(server_sd, NULL, NULL);

		if (-1 == client_sd)
		{
			perror("Accept fails: ");
			close(server_sd);
			exit(EXIT_FAILURE);
		}

		// 7. Create a thread for receiving messages from client

		client.id = connected_users;
		client.socket = client_sd;
		client.rxState = 1;
		printf("Client %d connected\n", connected_users);
		clients[connected_users] = client;
		printf("Added Client:\n ID: %d in position %d\n",
			   clients[connected_users].id, connected_users);

		printf("Create Pthread for reading\n");
		status = pthread_create(&read_threads[connected_users], NULL, &readThread,
								&clients[connected_users]);

		if (-1 == status)
		{
			perror("Pthread read fails: ");
			close(server_sd);
			exit(EXIT_FAILURE);
		}

		status = pthread_create(&check_threads[connected_users], NULL, &check_clients, &clients[connected_users]);
		if (-1 == status)
		{
			perror("Pthread Check Clients Fails: ");
			close(server_sd);
			exit(EXIT_FAILURE);
		}

		connected_users++;
	}

	exit(EXIT_SUCCESS);
}