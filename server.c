//
// Created by Ben Westcott on 6/11/21.
//

#include "header/server.h"

int serv_main(struct sockaddr *addr, socklen_t addr_len){
    struct sockaddr_in client_addr;
    int server_fd, client_fd;
    job *handle = malloc(sizeof(job));

    server_fd = spin_sock(SERVER, SOCK_STREAM, addr, addr_len);

    if(listen(server_fd, 3) < 0){
        zed_exit("listen", EXIT_FAILURE);
    }

    printf("[*] server listening on 0.0.0.0:%d\n", PORT_C);

    if((client_fd = accept(server_fd, addr, &addr_len)) < 0){
        zed_exit("accept", EXIT_FAILURE);
    }

    // TODO: malloc
    if(fill_job_info(client_fd, handle) < 0){
        zed_exit("filljobinfo", EXIT_FAILURE);
    }

    handle->sock_fd = client_fd;

    printf("[*] accepted connection from %s:%d\n", handle->info->s_ip, handle->info->port);

    if(handle_conn(handle) < 0){
        zed_exit("handle_conn", EXIT_FAILURE);
    }

    printf("[*] closing connection with: [%s:%d]\n", handle->info->s_ip, handle->info->port);
    close(handle->sock_fd);

    free_job(handle);
    return 0;
}

int handle_conn(job *handle){
    raw_obj_header *raw_response_header;
    char *readable_request, *readable_response;
    int send_size_flag = 0;

    raw_obj_header raw_request;

    if(read(handle->sock_fd, raw_request.raw, OBJ_HEAD_LEN) < 0){
        zed_exit("read", EXIT_FAILURE);
    }

    obj_header * request_header = build_obj_header(&raw_request);

    readable_request = to_readable(request_header);
    printf("[*] [%s:%d]: %s\n", handle->info->s_ip, handle->info->port, readable_request);

    free(readable_request);

    if(fill_job_obj(request_header, handle) < 0){
        zed_exit("fill_job_info", EXIT_FAILURE);
    }

    obj_size_t obj_size;
    obj_header *response_header = malloc(sizeof(obj_header));

    BYTE jtype, ctype;
    switch(request_header->obj.ctype) {
        case SEND_D:
            jtype = RECV_D;
            ctype = UNDEF_D;
            obj_size = request_header->obj.size;
            break;
        case RECV_D:
            jtype = SEND_D;
            if(get_obj_size(handle->obj->abs_path, &obj_size) < 0){
                send_size_flag = 1;

                printf("[-] could not open '%s': %s\n", handle->obj->name, strerror(errno));
                ctype = NO_EXIST_D;
            } else {
                printf("[*] got size: %lld bytes\n", obj_size);
                ctype = UNDEF_D;
            }
            break;
        default:
            printf("[-] unrecognized ctype for this context: 0x%x\n", request_header->obj.ctype);
            return -1;
    }

    handle->job_type = jtype;
    handle->obj->size = obj_size;

    response_header->htype = RESP_D;
    response_header->obj.ctype = ctype;
    response_header->obj.size = obj_size;
    response_header->obj.name = handle->obj->name;

    free(request_header->obj.name);
    free(request_header);

    raw_response_header = build_raw_obj_header(response_header);

    readable_response = to_readable(response_header);

    printf("[*] sending response: %s\n", readable_response);

    free(readable_response);
    free(response_header);

    if(send(handle->sock_fd, raw_response_header->raw, OBJ_HEAD_LEN, 0) < 0){
        zed_exit("send", EXIT_FAILURE);
    }

    if(send_size_flag){
        return -1;
    } else {
        free(raw_response_header);
    }

    if(handle->job_type == SEND_D){

        raw_obj_header OK_HEAD;
        if(read(handle->sock_fd, &OK_HEAD, OBJ_HEAD_LEN) < 0){
            zed_exit("read", EXIT_FAILURE);
        }

        if(is_ok_header(&OK_HEAD) < 0){
            printf("[!] client did not respond with OK\n");
            return -1;
        }

        printf("[*] client responded with OK\n");
    } else {
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
