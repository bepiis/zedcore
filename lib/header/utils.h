//
// Created by Ben Westcott on 6/10/21.
//

#ifndef ZEDCORE_UTILS_H
#define ZEDCORE_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


#define ZED_ROOT_PATH "data"

typedef unsigned char BYTE;
typedef unsigned long long obj_size_t;

struct object {
    BYTE ctype;
    char * name;
    obj_size_t size;
};

struct internal_object {
    char * name;
    char * abs_path;
    obj_size_t size;
};

// TODO: contains malloc
BYTE * uval_2_byte_arr(unsigned int val, size_t val_size);

// TODO: contains malloc
char * append_root_path(char * path);

int get_obj_size(char * path, obj_size_t * size);

void zed_exit(char * msg, int e);


#endif //ZEDCORE_UTILS_H
