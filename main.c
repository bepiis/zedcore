#include <stdio.h>
#include <string.h>

#include "lib/header/head.h"
#include "lib/header/node.h"
#include "lib/header/utils.h"
#include "header/client.h"
#include "header/server.h"

#include <errno.h>
#include <unistd.h>

const char info[] = "version:idfk\nhi mom n dad\n";

const char help[] = "ZedCore:\n \
\n \
usage: ZedCore [ntype] [options] \n \
\n \
ntypes: \n \
\t-c (SEND | RECV) {path} {server address}\tclient mode\n \
\t-s\t\t\t\t\t\tserver mode\n \
\n \
notes: \n \
\t- Zed will only look in 'data/' for specified path. You do not need to specify root path \n";


// ZedCore [node type] [options]

int main(int argc, char **argv) {
    if(argc < 2) {
        printf("%s", info);
        return 0;
    }

    if(init() < 0){
        zed_exit("init", EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_D);

    if(strncmp(argv[1], "-s", 2) == 0){

        addr.sin_addr.s_addr = INADDR_ANY;
        serv_main((struct sockaddr *) &addr, (socklen_t)sizeof(addr));

    } else if(strncmp(argv[1], "-c", 2) == 0){

        if(argc - 1 < 4){
            printf("[-] incorrect number of arguments for -c\n");
            return -1;
        }

        if(inet_pton(AF_INET, argv[4], &addr.sin_addr) < 1){
            printf("[-] address format not recognized or not supported\n");
            return -1;
        }

        job *handle = malloc(sizeof(job));
        handle->info = malloc(sizeof(struct job_info));
        handle->obj = malloc(sizeof(struct internal_object));

        handle->obj->abs_path = append_root_path(argv[3]);

        if(access(handle->obj->abs_path, F_OK & R_OK) < 0){
            printf("[-] could not open '%s': %s\n", argv[3], strerror(errno));
            return -1;
        }

        if(strncmp(argv[2], "SEND", 4) == 0){
            handle->job_type = SEND_D;

            if(get_obj_size(handle->obj->abs_path, &handle->obj->size) < 0){
                printf("[-] could not open '%s': %s\n", argv[3], strerror(errno));
                return -1;
            }

            printf("[*] got size: %lld\n", handle->obj->size);

        } else if(strncmp(argv[2], "RECV", 4) == 0){
            handle->job_type = RECV_D;
            handle->obj->size = 0;
        } else {
            printf("[-] unrecognized symbol: %s\n", argv[2]);
            return -1;
        }

        handle->info->addr = malloc(sizeof(addr));
        handle->info->s_ip = malloc(strlen(argv[4]) + 1);
        handle->obj->name = malloc(strlen(argv[3]) + 1);

        memcpy(handle->info->addr, &addr, sizeof(addr));
        handle->info->addr_len = sizeof(addr);

        strncpy(handle->info->s_ip, argv[4], strlen(argv[3]));

        strncpy(handle->obj->name, argv[3], strlen(argv[3]));

        handle->info->port = PORT_D;

        // printf("%s\n", handle->info->s_ip);

        client_main(handle);

        free_job(handle);

    } else if(strncmp(argv[1], "-h", 2) == 0){
        printf("%s", help);
        return 0;
    }
    else {
        printf("[-] unrecognized symbol: %s\n", argv[1]);
        return -1;
    }
    return 0;

}
