//
// Created by Ben Westcott on 6/10/21.
//

#include "header/head.h"

const raw_obj_header WRAPPED_OK_HEADER = {{HEAD_D, MSG_D, OK_D}};

raw_obj_header * build_raw_obj_header(obj_header * head){
    raw_obj_header * ret = malloc(sizeof(raw_obj_header));

    int idx = 0;

    ret->raw[idx] = HEAD_D;

    idx++;
    ret->raw[idx] = head->htype;

    idx++;
    ret->raw[idx] = head->obj.ctype;

    idx++;
    ret->raw[idx] = OBJ_NAME_D;

    char *ptr = head->obj.name;
    for(int i = 0; i < MAX_OBJ_NAME_LEN; i++){
        idx++;
        if(*ptr != '\0'){
            ret->raw[idx] = (BYTE)*ptr;
            ptr++;
        } else {
            ret->raw[idx] = 0x0;
        }
    }

    if(head->obj.size > 0){
        idx++;
        ret->raw[idx] = OBJ_SIZE_D;

        BYTE * raw_size = uval_2_byte_arr(head->obj.size, sizeof(obj_size_t));
        for(int i=0; i < sizeof(obj_size_t); i++){
            idx++;
            ret->raw[idx] = raw_size[i];
        }
        free(raw_size);
    }

    int left = OBJ_HEAD_LEN - idx;
    for(int i=0; i < left; i++){
        idx++;
        ret->raw[idx] = 0x0;
    }

    return ret;
}

obj_header * build_obj_header(raw_obj_header * raw){
    obj_header * ret = malloc(sizeof(obj_header));

    int idx = 0;
    if(raw->raw[idx] != HEAD_D){
        printf("%s\n", UNREADABLE);
        return NULL;
    }

    idx++;
    ret->htype = raw->raw[idx];

    idx++;
    ret->obj.ctype = raw->raw[idx];

    idx++;
    char name_buf[MAX_OBJ_NAME_LEN];

    memcpy(name_buf, &raw->raw[idx + 1], MAX_OBJ_NAME_LEN);

    size_t name_len = strlen(name_buf) + 1;

    ret->obj.name = malloc(name_len);

    memcpy(ret->obj.name, name_buf, name_len);

    idx += MAX_OBJ_NAME_LEN;

    idx++;
    if(raw->raw[idx] == OBJ_SIZE_D){
        memcpy(&ret->obj.size, &raw->raw[idx + 1], sizeof(obj_size_t));
    } else {
        ret->obj.size = 0;
    }

    return ret;
}

const map_entry flag_map[NUM_FLAGS] = {
        {REQ_D, "REQUEST", 7},
        {RESP_D, "RESPONSE", 8},
        {MSG_D, "MSG", 3},
        {SEND_D, "SEND", 4},
        {RECV_D, "RECV", 4},
        {UNDEF_D, "UNDEF", 5},
        {NO_EXIST_D, "NO_EXIST", 8},
        {OBJ_SIZE_D, "OBJ_SIZE", 8},
        {OBJ_NAME_D, "OBJ_NAME", 8}
};

const map_entry * get_map_entry(BYTE target){
    for(int i=0; i < NUM_FLAGS; i++){
        if(flag_map[i].raw == target){
            return &flag_map[i];
        }
    }
    return NULL;
}

char *to_readable(obj_header * head){
    size_t ret_len = 0;
    char *ret, *s_obj_size;

    const map_entry *htype_entry, *ctype_entry;
    map_entry *run, *block;

    const int sep_len = 2;
    int num_seps = 4;
    int idx = 0;
    int include_size;

    if((include_size = head->obj.size > 0) > 0){
        num_seps += 2;
    }

    block = malloc(num_seps * sizeof(map_entry));

    if((run = (map_entry *) get_map_entry(head->htype)) == NULL){
        printf("%s\n", UNREADABLE);
        return NULL;
    }
    block[idx] = *run;
    idx++;

    if((run = (map_entry *) get_map_entry(head->obj.ctype)) == NULL){
        printf("%s\n", UNREADABLE);
        return NULL;
    }
    block[idx] = *run;
    idx++;

    block[idx] = flag_map[8];
    idx++;

    map_entry obj_name_e = {0x0, head->obj.name, (int)strlen(head->obj.name)};
    block[idx] = obj_name_e;
    idx++;

    if(include_size){
        block[idx] = flag_map[7];
        idx++;

        int s_length = snprintf(NULL, 0, "%llx", head->obj.size);
        s_obj_size = malloc(s_length + 1);
        snprintf(s_obj_size, s_length + 1, "%llx", head->obj.size);


        map_entry obj_size_e = {0x0, s_obj_size, s_length};

        block[idx] = obj_size_e;
    }

    for(int i=0; i < num_seps; i++){
        ret_len += block[i].rep_len;
    }

    ret_len += (sep_len * num_seps);
    //ret_len++;
    ret_len += 4;

    ret = malloc(4 + ret_len);
    ret[0] = '\0';

    strcat(ret, "HEAD");
    for(int i=0; i < num_seps; i++){
        strcat(ret, "::");
        strncat(ret, block[i].rep, block[i].rep_len);
    }

    //if(include_size) free(s_obj_size);

    free(block);

    return ret;
}

int is_ok_header(raw_obj_header * head){
    for(int i=0; i < OBJ_HEAD_LEN; i++){
        if(head->raw[i] != WRAPPED_OK_HEADER.raw[i]){
            return -1;
        }
    }
    return 0;
}

void print_raw_header(raw_obj_header * raw){
    for(int i=0; i < OBJ_HEAD_LEN; i++) {
        printf("0x%x, ", raw->raw[i]);
    }
    printf("\n");
}