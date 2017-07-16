//
// Created by Jean-Christophe MELIKIAN on 15/07/2017.
//

#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include "ftp_common.h"

const char *server_addr = "127.0.0.1";
char hostname[1024];
static volatile int keepRunning = 1;

void sigint_handler(int dummy) {
	keepRunning = 0;
}

void client_cli(int socket_desc);

void signal_interceptor();

int main(int argc, char **argv) {

	signal_interceptor();
	get_computer_name();

	int socket_desc = init_connection(server_addr, SERVER_CLIENT_PORT);

	client_cli(socket_desc);

	console_write("\n\nEnd Client\n\n");
	close(socket_desc);

	return 0;
}

void client_cli(int socket_desc) {
	while (keepRunning) {
		char console_input[CLIENT_BUFFER_LENGTH];

		console_read(console_input);
		if (strlen(console_input) >  1) {
			send_request(socket_desc, console_input, strlen(console_input));
		}
		//sleep(1);
	}
}