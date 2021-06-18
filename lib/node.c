//
// Created by Ben Westcott on 6/10/21.
//

#include "header/node.h"

int init(void){

    struct stat st;
    int dir;
    if(stat(ZED_ROOT_PATH, &st) != 0 && !S_ISDIR(st.st_mode)){
        printf("[!] zed root path not found, creating one...\n");
        dir = mkdir(ZED_ROOT_PATH, 0777);
        if(!dir){
            printf("[*] root path created\n");
        } else {
            printf("[x] unable to create root path\n");
            return MK_ROOT_FAIL;
        }
    }
    return 0;
}

int spin_sock(BYTE ntype, int ttype, struct sockaddr * addr, socklen_t addr_len){
    int sock_fd;
    int sock_opt_val = 1;

    if((sock_fd = socket(AF_INET, ttype, 0)) < 0){
        zed_exit("sock", EXIT_FAILURE);
    }

    if(ntype == SERVER){
        if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &sock_opt_val, sizeof(sock_opt_val)) < 0){
            zed_exit("setsockopt", EXIT_FAILURE);
        }

        if(bind(sock_fd, addr, addr_len) < 0){
            zed_exit("bind", EXIT_FAILURE);
        }
    }
    return sock_fd;
}

int do_job(job *handle){
    int (*type)(int, struct internal_object*);
    if(handle->job_type == SEND_D){
        type = &send_obj;
    } else if(handle->job_type == RECV_D){
        type = &get_obj;
    } else {
        printf("[*] cannot do job, type unrecognized\n");
        return -1;
    }
    (*type)(handle->sock_fd, handle->obj);
    return 0;
}

/// @param sock_fd: socket

int send_obj(int sock_fd, struct internal_object *obj){
    char buff[NET_BUFF_SIZE];
    FILE * obj_fd;
    obj_size_t stat, total = 0;

    memset(&buff, 0, NET_BUFF_SIZE);

    if((obj_fd = fopen(obj->abs_path, "rb")) == NULL){
        zed_exit("fopen", EXIT_FAILURE);
    }

    printf("\e[?25l");
    obj_size_t known_size = obj->size;
    while((stat = fread(buff, 1, NET_BUFF_SIZE, obj_fd)) > 0){

        if(send(sock_fd, buff, stat, 0) < 0){
            zed_exit("send", EXIT_FAILURE);
        }

        total += stat;
        bzero(&buff, NET_BUFF_SIZE);

        float percent = 100*(float)total/(float)known_size;
        printf("[*] sent: %llu bytes [%d%%]\r", total, (int)percent);

    }
    printf("\e[?25h");
    printf("\n");

    printf("[*] total promised: %llu\n", known_size);
    printf("[*] total given: %llu\n", total);

    if(total != known_size){
        printf("[!] size mismatch!\n");
    }

    fclose(obj_fd);
    return 0;
}

int get_obj(int sock_fd, struct internal_object *obj){
    ssize_t stat;
    FILE *obj_fd;
    char buff[NET_BUFF_SIZE];
    obj_size_t total = 0;

    memset(&buff, 0, NET_BUFF_SIZE);

    if((obj_fd = fopen(obj->abs_path, "wb")) == NULL){
            zed_exit("fopen", EXIT_FAILURE);
    }

    printf("\e[?25l");
    obj_size_t known_size = obj->size;
    while((stat = recv(sock_fd, buff, NET_BUFF_SIZE, 0)) > 0){

        total += stat;
        fwrite(buff, 1, stat, obj_fd);
        bzero(&buff, NET_BUFF_SIZE);

        float percent = 100*(float)total/(float)known_size;
        printf("[*] received: %llu bytes [%d%%]\r", total, (int)percent);
        fflush(stdout);
    }
    printf("\e[?25h");
    printf("\n");

    printf("[*] total promised: %llu\n", known_size);
    printf("[*] total given: %llu\n", total);

    if(total != known_size){
        printf("[!] size mismatch!\n");
    }

    fclose(obj_fd);
    return 0;
}

// TODO: contains malloc
int fill_job_info(int sock_fd, job *handle){
    handle->info = malloc(sizeof(struct job_info));
    handle->info->s_ip = malloc(16);
    handle->info->addr = malloc(sizeof(struct sockaddr_in));

    bzero(handle->info->addr, sizeof(*handle->info->addr));
    handle->info->addr_len = sizeof(*handle->info->addr);


    if(getsockname(sock_fd, (struct sockaddr *) handle->info->addr, &handle->info->addr_len) < 0){
        return -1;
    }

    memcpy(handle->info->s_ip, inet_ntoa(handle->info->addr->sin_addr), 16);
    handle->info->port = ntohs(handle->info->addr->sin_port);

    return 0;
}

// TODO: contains malloc
int fill_job_obj(obj_header *head, job *handle){
    if(head->obj.ctype == SEND_D && head->obj.size == 0){
        printf("[!] client wants to send data, but has not specified the size");
        return -1;
    }

    handle->obj = malloc(sizeof(struct internal_object));

    handle->obj->size = head->obj.size;

    unsigned long name_len = strlen(head->obj.name);
    handle->obj->name = malloc(name_len + 1);

    snprintf(handle->obj->name, name_len + 1, "%s", head->obj.name);

    unsigned long abs_path_len = strlen(ZED_ROOT_PATH) + 1 + name_len;
    handle->obj->abs_path = malloc(abs_path_len + 1);

    snprintf(handle->obj->abs_path, abs_path_len + 1, "%s/%s", ZED_ROOT_PATH, head->obj.name);

    return 0;
}

void free_job(job *handle){
    free(handle->info->addr);
    free(handle->info->s_ip);
    free(handle->info);
    free(handle->obj->abs_path);
    free(handle->obj->name);
    free(handle->obj);
    free(handle);
}





