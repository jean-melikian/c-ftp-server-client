//
// Created by Jean-Christophe MELIKIAN on 16/07/2017.
//

#ifndef PROJET_FTP_COMMON_H
#define PROJET_FTP_COMMON_H

#include <unistd.h>

#define SERVER_CLIENT_PORT 50000
#define SERVER_MASTER_PORT 50001
#define CLIENT_BUFFER_LENGTH 1024

extern char hostname[1024];

void signal_interceptor();

void get_computer_name();

int init_connection(const char *address, int port);

char *console_read(char *input);

void console_write(char *output);

ssize_t read_server(int socket_desc, char *input, size_t nb_bytes);

ssize_t write_server(int socket_desc, char *output, size_t nb_bytes);

int send_request(int socket_desc, char *output, size_t nb_bytes);

void explode(const char *src, const char *tokens, char ***list, size_t *len);

char *strdup(const char *src);

#endif //PROJET_FTP_COMMON_H
