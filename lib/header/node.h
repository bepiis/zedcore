//
// Created by Ben Westcott on 6/10/21.
//

#ifndef ZEDCORE_NODE_H
#define ZEDCORE_NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "head.h"
#include "err.h"

#define PORT_C 9001
#define PORT_D 9002

#define NET_BUFF_SIZE 1024

#define CLIENT 0x1
#define SERVER 0x2

struct job_info {
    struct sockaddr_in *addr;
    socklen_t addr_len;
    char *s_ip;
    unsigned int port;
};

typedef struct {
    int sock_fd;
    struct job_info *info;
    BYTE job_type;
    struct internal_object *obj;
} job;


int init(void);

int spin_sock(BYTE ntype, int ttype, struct sockaddr * addr, socklen_t addr_len);

int do_job(job *handle);

int send_obj(int sock_fd, struct internal_object *obj);

int get_obj(int sock_fd, struct internal_object *obj);

int fill_job_info(int sock_fd, job *handle);

int fill_job_obj(obj_header *head, job *handle);

void free_job(job *handle);


#endif //ZEDCORE_NODE_H
