//
// Created by Ben Westcott on 6/10/21.
//

#include "header/utils.h"

BYTE * uval_2_byte_arr(unsigned int val, size_t val_size){
    BYTE * ret = malloc(val_size * sizeof(BYTE));

    for(int i=0; i < val_size; i++){
        if(i != 0){
            val >>= 8;
        }
        ret[i] = val & 0xFF;
    }
    return ret;
}

char * append_root_path(char * path){
    size_t ret_len = 5 + strlen(path) + 1;
    char * ret = malloc(ret_len);

    //printf("[DBG] ret_len: %zu\n", ret_len);

    snprintf(ret, ret_len, "%s/%s", ZED_ROOT_PATH, path);
    return ret;
}

int get_obj_size(char * path, obj_size_t * size){
    if(access(path, F_OK & R_OK) == 0){
        FILE *obj_fd;
        if((obj_fd = fopen(path, "r")) != NULL){
            fseek(obj_fd, 0, SEEK_END);
            *size = ftell(obj_fd);
            fseek(obj_fd, 0, SEEK_SET);
            fclose(obj_fd);
        } else {
            return -1;
        }
    }
    return 0;
}

void zed_exit(char * msg, int e){
    perror(msg);
    exit(e);
}

