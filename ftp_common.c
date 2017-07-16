//
// Created by Jean-Christophe MELIKIAN on 16/07/2017.
//

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
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