#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#define PORT 6666
#define BUF_SIZE 128
#define NUM_CLIENTS 11

struct client_t
{
	int id;
	int socket;
	int rxState;
};

int num_connected = 0;
int client_threads[NUM_CLIENTS];
struct client_t clients[NUM_CLIENTS];
pthread_t read_threads[NUM_CLIENTS];
pthread_t check_threads[NUM_CLIENTS];

void *readThread(void *arg)
{
	struct client_t *client = ((struct client_t *)arg);
	ssize_t numOfBytes;
	char buf[BUF_SIZE];
	int status;

	while (1)
	{
		numOfBytes = read(client->socket, buf, BUF_SIZE);
		if (0 == numOfBytes)
		{
			printf("client closed the socket end\n");
			break;
		}
		else if (-1 == numOfBytes)
		{
			perror("ReadThread read() fails: ");
			break;
		}
		else
		{
			struct client_t temp_client;
			if (0 == strncmp(buf, ":exit", strlen(":exit")))
			{
				printf("Client exit\n");
				break;
			}
			if (0 == strncmp(buf, ":list", strlen(":list")))
			{
				char *list = "Clientes Conectados:  ";
				list[strlen(list) - 2] = num_connected;
				for (int i = 0; i < num_connected; i++)
				{

					temp_client = clients[i];

					status = write(temp_client.socket, list, strlen(list) + 1);
					if (-1 == status)
					{
						perror("Server write to client fails: ");
						num_connected--;
						break;
					}
				}
				printf("Client exit\n");
				break;
			}

			// Responder a todos los clientes

			for (int i = 0; i < num_connected; i++)
			{

				temp_client = clients[i];

				if (temp_client.rxState == 0)
					break;
				printf("Replying to Client: %d\n", temp_client.id);

				if (buf[strlen(buf) - 1] == '\n')
					buf[strlen(buf) - 1] = 0;

				status = write(temp_client.socket, buf, strlen(buf) + 1);
				if (-1 == status)
				{
					perror("Server write to client fails: ");
					num_connected--;
					break;
				}
			}
		}
	}
	printf("Terminate Pthread for reading\n");
	client->rxState = 0;
	return NULL;
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
			num_connected--;
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

		client.id = num_connected;
		client.socket = client_sd;
		client.rxState = 1;
		printf("Client %d connected\n", num_connected);
		clients[num_connected] = client;
		printf("Added Client:\n ID: %d in position %d\n",
			   clients[num_connected].id, num_connected);

		// printf("Create Pthread for reading\n");
		status = pthread_create(&read_threads[num_connected], NULL, &readThread,
								&clients[num_connected]);

		if (-1 == status)
		{
			perror("Pthread read fails: ");
			close(server_sd);
			exit(EXIT_FAILURE);
		}

		status = pthread_create(&check_threads[num_connected], NULL,
								&check_clients, &clients[num_connected]);
		if (-1 == status)
		{
			perror("Pthread Check Clients Fails: ");
			close(server_sd);
			exit(EXIT_FAILURE);
		}

		num_connected++;
	}

	exit(EXIT_SUCCESS);
}