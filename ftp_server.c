//
// Created by Jean-Christophe MELIKIAN on 15/07/2017.
//

#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "ftp_common.h"

char hostname[1024];
static volatile int keepRunning = 1;

void sigint_handler(int dummy) {
	keepRunning = 0;
}

int init_server();

void ftp_service_listen(int socket_desc);

void listen_client_cli(int socket_desc, char *client_ip_addr, char *buffer);

void stop_server(int socket_desc);

void *client_handler(void *arguments);

// -- Commands coming from clients
void want(int socket_desc, char **params, size_t count);

void files(int socket_desc, char **params, size_t count);

void need(int socket_desc, char *param);

// -- Commands coming from slaves
void slave(int socket_desc, char **params, size_t count);

// Responses from the server
void content(int socket_desc, char* file_path);

void no_such_file(int socket_desc);

void ok(int socket_desc);

void bad(int socket_desc);

//

void interpretor(int socket_desc, char *str);


typedef struct {
	int client_sock;
	struct sockaddr_in pt_client;
} client_handler_args;

int main(int argc, char **argv) {

	signal_interceptor();

	int socket_desc = init_server();

	ftp_service_listen(socket_desc);

	stop_server(socket_desc);

	return 0;
}

int init_server() {

	int socket_desc = socket(PF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1) {
		perror("Unable to create socket");
		exit(1);
	}
	puts("Socket created");

	// Prepare the sockaddr_in structure
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_CLIENT_PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_desc, (struct sockaddr *) &server, sizeof(server)) != 0) {
        perror("Unable to bind port");
        exit(1);
    }
    puts("Bind done");

    if (listen(socket_desc, 10) == -1) {
        perror("Unable to listen");
        exit(1);
    }


    return socket_desc;
}

void ftp_service_listen(int socket_desc) {
    struct sockaddr_in pt_client; // the client's endpoint information
    socklen_t len;
    int sclient; // the client's pipe descriptor


    printf("Server listening on port %d\n", SERVER_CLIENT_PORT);
    while (keepRunning) {

        sclient = accept(socket_desc, (struct sockaddr *) &pt_client, &len);
        if (sclient < 0) {
            perror("Connection failed");
        } else {
            puts("Connection accepted");

            pthread_t thread_id;
            client_handler_args args = {sclient, pt_client};

            if (pthread_create(&thread_id, NULL, client_handler, (void *) &args) < 0) {
                perror("Could not create a new thread !");
                //exit(1);
            } else {
                puts("A new thread has been created for the client !");

            }

        }
    }
}

/**
 *
 * @param arguments A client_handler_args structure
 * @return
 */
void *client_handler(void *arguments) {
    // Get the socket descriptor
    client_handler_args *args = arguments;


    char buffer[CLIENT_BUFFER_LENGTH];
    //close(sock);
    char *client_ip_addr = inet_ntoa(args->pt_client.sin_addr);
    printf("New client - %s\n", client_ip_addr);
    memset(buffer, 0, sizeof(buffer));

    listen_client_cli(args->client_sock, client_ip_addr, buffer);

    printf("Closed client - %s\n", client_ip_addr);
    close(args->client_sock);
    pthread_exit(NULL);

}

/**
 * Going to search some files listed by the client
 * @param params
 */
void want(int socket_desc, char **params, size_t count) {
    printf("FUNCTION: WANT\n");

	for(int i = 0; i < count; i++) {
		printf("%s ", params[i]);
	}
}

/**
 *
 * @param params
 */
void files(int socket_desc, char **params, size_t count) {
	printf("FUNCTION: FILES\n");


}

/**
 * Going to search one file
 * @param param
 */
void need(int socket_desc, char *param) {
	printf("FUNCTION: NEED\n");
	content(socket_desc, param);
}

void content(int socket_desc, char* file_path) {
	char *output = malloc(sizeof(char) * 1000);

	if(file_path != NULL) {

		FILE *f = fopen(file_path, "rb");
		fseek(f, 0, SEEK_END);
		long fsize = ftell(f);
		fseek(f, 0, SEEK_SET);  //same as rewind(f);

		char *buffer = malloc(fsize + 1);
		fread(buffer, fsize, 1, f);
		fclose(f);

		buffer[fsize] = 0;

		output = malloc(fsize + sizeof(char) * 11);

		sprintf(output, "%010ld\n%s", fsize, buffer);
		write(socket_desc, output, strlen(output));
	} else {

		strcpy(output, "NEED requires a file path argument !");
		write(socket_desc, output, strlen(output));
	}
	free(output);
}

/**
 * Function which interprets the command sent by the client
 * @param str
 */
void interpretor(int socket_desc, char *str) {
    char **list;
    char **params;
    size_t len, lenparams;

    explode(str, " ", &list, &len);

    if (strcmp(list[0], "WANT") == 0 || strcmp(list[0], "want") == 0) {
        // Launch CMD WANT
        explode(list[1], ",", &params, &lenparams);
        want(socket_desc, params, lenparams);

    } else if (strcmp(list[0], "FILES") == 0 || strcmp(list[0], "files") == 0) {
        // Launch CMD FILES
        explode(list[1], ",", &params, &lenparams);
        files(socket_desc, params, lenparams);

    } else if (strcmp(list[0], "NEED") == 0 || strcmp(list[0], "need") == 0) {
        // Launch CMD NEED
        need(socket_desc, list[1]);

    }
}


void listen_client_cli(int socket_desc, char *client_ip_addr, char *buffer) {
	while (keepRunning && read(socket_desc, buffer, sizeof(char) * CLIENT_BUFFER_LENGTH) > 0) {
		printf("From %s: [%s]\n", client_ip_addr, buffer);
		if (strcmp(buffer, "exit") == 0) {
			printf("%s asked to close the connection\n", client_ip_addr);
			break;
		}

		interpretor(socket_desc, buffer);

		//write(socket_desc, buffer, CLIENT_BUFFER_LENGTH);
		memset(buffer, 0, CLIENT_BUFFER_LENGTH);
	}
}

void stop_server(int socket_desc) {
    close(socket_desc);
    printf("Server killed\n");
}


