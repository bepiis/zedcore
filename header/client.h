//
// Created by Ben Westcott on 6/11/21.
//

#ifndef ZEDCORE_CLIENT_H
#define ZEDCORE_CLIENT_H

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

int client_main(job *handle);

#endif //ZEDCORE_CLIENT_H
