#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <C:/msys64/mingw64/include/gmp.h>
#include "E:/Bitcoin/bitcoinclib/includes/helper.h"

#ifndef SCRIPT
#define SCRIPT

typedef struct
{
    int size;
    char **commands; //Array of Strings
} Script;

void script_init(Script *self);
void script_init_set(Script *self, int size, char **commands);

void script_init_set_p2pkh(Script *self, char *h160);
void script_serialize(char **output_buffer, Script *self);

void script_parse(Script *self, char **src);

#endif