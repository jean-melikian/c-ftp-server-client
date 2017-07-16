//
// Created by Jean-Christophe MELIKIAN on 16/07/2017.
//

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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