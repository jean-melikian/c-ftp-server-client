//
// Created by Jean-Christophe MELIKIAN on 16/07/2017.
//

#ifndef PROJET_FTP_COMMON_H
#define PROJET_FTP_COMMON_H

#define SERVER_MASTER_PORT 50000
#define CLIENT_BUFFER_LENGTH 1024

void signal_interceptor();

void explode(const char *src, const char *tokens, char ***list, size_t *len);

char *strdup(const char *src);

#endif //PROJET_FTP_COMMON_H
