
#include <C:/msys64/mingw64/include/gmp.h>
#include "C:/msys64/mingw64/include/openssl/sha.h"
#include "C:/msys64/mingw64/include/openssl/ripemd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef HELPER_CPP
#define HELPER_CPP

#define bool int
#define true 1
#define false 0

char BASE58_ALPHABET[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

/* Hashing algorithms */
void hash256(char **output_buffer, const char *src, const int base, const bool is_string);
void hash160(char **output_buffer,const char *src, const int base);

/* Hexadecimal byte encoding/decoding */
void int_to_little_endian(char **output_buffer, const MP_INT *n, const int byte_size);
void int_to_big_endian(char **output_buffer, const MP_INT *n, const int byte_size);
void little_endian_to_int(MP_INT *i, char *src);
static inline char * flip_endianess(char *src);

/* Base 58 encoding */
void encode_base_58(char **output_buffer, char *src, int source_base);
void encode_base_58_checksum(char **output_buffer, char* src, const int source_base);
void decode_base_58(char **output_buffer, char *src);

/* Variable integer encoding/decoding */
void encode_varint(char **output_buffer, MP_INT *n);
void decode_varint(MP_INT *output_buffer, char **src);

/* Reading */
void extract(char **dest, char **src, int bytes);

#endif
