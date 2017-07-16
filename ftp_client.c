#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "ftp_common.h"

const char *server_addr = "127.0.0.1";

static volatile int keepRunning = 1;

void sigint_handler(int dummy) {
	keepRunning = 0;
}

int init_connection();

void client_cli(int socket_desc);

void signal_interceptor();

int main() {

	signal_interceptor();

	int socket_desc = init_connection();

	client_cli(socket_desc);

	printf("\n\nEnd Client\n\n");
	close(socket_desc);

	return 0;
}

int init_connection() {

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		perror("Unable to create socket");
		exit(1);
	}
	struct sockaddr_in server_endpoint;
	server_endpoint.sin_family = AF_INET;
	server_endpoint.sin_port = htons(SERVER_MASTER_PORT);
	server_endpoint.sin_addr.s_addr = inet_addr(server_addr);

	if (connect(sock, (struct sockaddr *) &server_endpoint, sizeof(server_endpoint)) == -1) {
		perror("Unable to connect to server");
		exit(1);
	}
	return sock;
}

void client_cli(int socket_desc) {
	char buffer[CLIENT_BUFFER_LENGTH];
	memset(buffer, 0, sizeof(buffer));
	while (keepRunning) {
		printf("Your message : ");
		char input[CLIENT_BUFFER_LENGTH];

		if (fgets(input, sizeof input, stdin) == NULL)
			break; //If Interrupt

		size_t ln = strlen(input) - 1;
		if (*input && input[ln] == '\n')
			input[ln] = '\0';

		printf("I send: %s\n", input);
		if (write(socket_desc, input, CLIENT_BUFFER_LENGTH) <= 0) {
			printf("Server Closed\n");
			break;
		}

		char receive[CLIENT_BUFFER_LENGTH];
		if (read(socket_desc, receive, CLIENT_BUFFER_LENGTH) <= 0) {
			printf("Server Closed");
			break;
		}
		printf("I read: %s \n", receive);
		memset(buffer, 0, sizeof(buffer));
		sleep(1);
	}
}