//
// Created by Ben Westcott on 6/11/21.
//

#ifndef ZEDCORE_SERVER_H
#define ZEDCORE_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "../lib/header/node.h"
#include "../lib/header/head.h"
#include "../lib/header/utils.h"
#include "../lib/header/err.h"

int serv_main(struct sockaddr *addr, socklen_t addr_len);

int handle_conn(job *handle);

#endif //ZEDCORE_SERVER_H
