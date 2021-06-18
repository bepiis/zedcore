//
// Created by Ben Westcott on 6/10/21.
//

#ifndef ZEDCORE_HEAD_H
#define ZEDCORE_HEAD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define MAX_OBJ_NAME_LEN 256
#define OBJ_HEAD_LEN 270

#define HEAD_D 0xA0

//htypes
#define REQ_D 0xB0
#define RESP_D 0xB1
#define MSG_D 0xB2

//obj_flags
#define OBJ_SIZE_D 0xC1
#define OBJ_NAME_D 0xC2

//ctypes
#define SEND_D 0xD0
#define RECV_D 0xD1
#define UNDEF_D 0xD2
#define NO_EXIST_D 0xD3

#define NUM_FLAGS 9

#define OK_D 0xE0

//#define merge(a, b) a##b

#define UNREADABLE "unreadable header"

/*const char *nm[] = {
    [REQUEST] = "request",
    [RESPONSE] = "response",
    [MSG] = "msg",
};*/

typedef struct {
    BYTE raw;
    char * rep;
    int rep_len;
} map_entry;

typedef struct {
    BYTE htype;
    struct object obj;
} obj_header;

typedef struct{
    BYTE raw[OBJ_HEAD_LEN];
} raw_obj_header;

const raw_obj_header WRAPPED_OK_HEADER;

// TODO: contains malloc
raw_obj_header * build_raw_obj_header(obj_header * head);

// TODO: contains malloc
obj_header * build_obj_header(raw_obj_header * raw);

const map_entry * get_map_entry_idx(BYTE target);

// TODO: contains malloc
char * to_readable(obj_header * head);

int is_ok_header(raw_obj_header * head);

void print_raw_header(raw_obj_header * raw);

#endif //ZEDCORE_HEAD_H
