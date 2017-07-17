//
// Created by Jean-Christophe MELIKIAN on 16/07/2017.
//

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory.h>
#include <unistd.h>
#include "ftp_common.h"

void signal_interceptor() {
	void sigint_handler(int sig);
	struct sigaction sa;

	sa.sa_handler = sigint_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}
}

/**
 *
 * @param address
 * @param port
 * @return The socket descriptor
 */
int init_connection(const char *address, int port) {

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		perror("Unable to create socket");
		exit(1);
	}
	struct sockaddr_in server_endpoint;
	server_endpoint.sin_family = AF_INET;
	server_endpoint.sin_port = htons(port);
	server_endpoint.sin_addr.s_addr = inet_addr(address);

	if (connect(sock, (struct sockaddr *) &server_endpoint, sizeof(server_endpoint)) == -1) {
		perror("Unable to connect to server");
		exit(1);
	}
	return sock;
}

char *console_read(char *input) {
	printf("> %s $ ", hostname);
	return fgets(input, CLIENT_BUFFER_LENGTH, stdin);
}


void console_write(char *output) {
	printf("\n%s\n", output);
}

ssize_t read_server(int socket_desc, char *input, size_t nb_bytes) {

	ssize_t size;
	if ((size = read(socket_desc, input, nb_bytes)) < 0) {
		console_write("Server closed\n");
	}
	return size;
}

ssize_t write_server(int socket_desc, char *output, size_t nb_bytes) {
	size_t ln = strlen(output) - 1;
	if (*output && output[ln] == '\n')
		output[ln] = '\0';

	ssize_t size;
	if ((size = write(socket_desc, output, nb_bytes)) < 0) {
		console_write("Server closed\n");
	}
	return size;
}

int send_request(int socket_desc, char *output, size_t nb_bytes) {

	char socket_input[CLIENT_BUFFER_LENGTH];

	if (output == NULL)
		return -1; //If Interrupt

	if (write_server(socket_desc, output, nb_bytes) == -1) {
		return -1;
	}

	if (read_server(socket_desc, socket_input, CLIENT_BUFFER_LENGTH) == -1) {
		return -1;
	}

	console_write(socket_input);
	return 0;
}

void get_computer_name() {
	gethostname(hostname, 1024);
}


char *strdup(const char *src) {
	char *tmp = malloc(strlen(src) + 1);
	if (tmp)
		strcpy(tmp, src);
	return tmp;
}

void explode(const char *src, const char *tokens, char ***list, size_t *len) {
	if (src == NULL || list == NULL || len == NULL)
		return;

	char *str, *copy, **_list = NULL, **tmp;
	*list = NULL;
	*len = 0;

	copy = strdup(src);
	if (copy == NULL)
		return;

	str = strtok(copy, tokens);
	if (str == NULL)
		goto free_and_exit;

	_list = realloc(NULL, sizeof *_list);
	if (_list == NULL)
		goto free_and_exit;

	_list[*len] = strdup(str);
	if (_list[*len] == NULL)
		goto free_and_exit;
	(*len)++;


	while ((str = strtok(NULL, tokens))) {
		tmp = realloc(_list, (sizeof *_list) * (*len + 1));
		if (tmp == NULL)
			goto free_and_exit;

		_list = tmp;

		_list[*len] = strdup(str);
		if (_list[*len] == NULL)
			goto free_and_exit;
		(*len)++;
	}


	free_and_exit:
	*list = _list;
	free(copy);
}