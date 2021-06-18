//
// Created by Ben Westcott on 6/11/21.
//

#include "header/client.h"

int client_main(job *handle){
    char in_buff[OBJ_HEAD_LEN];
    char *readable_request, *readable_response;

    handle->sock_fd = spin_sock(CLIENT, SOCK_STREAM, (struct sockaddr *) handle->info->addr, handle->info->addr_len);

    if(connect(handle->sock_fd, (struct sockaddr *) handle->info->addr, handle->info->addr_len) < 0){
        zed_exit("connect", EXIT_FAILURE);
    }

    printf("[*] connection established.\n");

    obj_header request_head = {REQ_D, {handle->job_type, handle->obj->name, handle->obj->size}};
    raw_obj_header * raw_request_head = build_raw_obj_header(&request_head);

    if(send(handle->sock_fd, raw_request_head->raw, OBJ_HEAD_LEN, 0) < 0){
        zed_exit("send", EXIT_FAILURE);
    }

    readable_request = to_readable(&request_head);
    printf("[*] sent request header: %s\n", readable_request);

    free(readable_request);
    free(raw_request_head);

    bzero(in_buff, OBJ_HEAD_LEN);
    if(read(handle->sock_fd, in_buff, OBJ_HEAD_LEN) < 0){
        zed_exit("read", EXIT_FAILURE);
    }

    raw_obj_header raw_response;
    memcpy(raw_response.raw, in_buff, OBJ_HEAD_LEN);

    obj_header *response_header = build_obj_header(&raw_response);

    readable_response = to_readable(response_header);
    printf("[*] [%s:%d]: %s\n", handle->info->s_ip, handle->info->port, readable_response);

    free(readable_response);

    handle->obj->size = response_header->obj.size;

    printf("[*] got size: %lld bytes\n", handle->obj->size);

    free(response_header);

    if(handle->job_type == SEND_D){

        raw_obj_header OK_HEAD;
        if(read(handle->sock_fd, &OK_HEAD, OBJ_HEAD_LEN) < 0){
            zed_exit("read", EXIT_FAILURE);
        }

        if(is_ok_header(&OK_HEAD) < 0){
            printf("[!] server did not respond with OK\n");
            return -1;
        }

        printf("[*] server responded with OK\n");
    } else if(handle->job_type == RECV_D){

        if(send(handle->sock_fd, WRAPPED_OK_HEADER.raw, OBJ_HEAD_LEN, 0) < 0){
            zed_exit("send", EXIT_FAILURE);
        }

        printf("[*] sent OK\n");
    }

    if(do_job(handle) < 0){
        zed_exit("do_job", EXIT_FAILURE);
    }

    printf("[*] finished job\n");

    return 0;
}

