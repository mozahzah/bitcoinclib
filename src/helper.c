
#include "E:/Bitcoin/bitcoinclib/includes/helper.h"

#ifndef HELPER
#define HELPER

/* Hashing algorithms */

void hash256(char **output_buffer, const char *src, const int base, const bool is_string)
{   
    unsigned char hash[SHA256_DIGEST_LENGTH];
    if (is_string)
    {
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, src, strlen(src));
        SHA256_Final(hash, &sha256);
    }
    else
    {
        MP_INT i;
        mpz_init_set_str(&i, src, base);
        unsigned char bytes[mpz_sizeinbase(&i, 16)/2];
        mpz_export(bytes, NULL, 1, 1, 1, 0, &i);
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, bytes, sizeof(bytes));
        SHA256_Final(hash, &sha256);

        /* Free Large Integer Memory */
        mpz_clear(&i);
    }
    unsigned char hash2[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256_2;
    SHA256_Init(&sha256_2);
    SHA256_Update(&sha256_2, hash, SHA256_DIGEST_LENGTH);
    SHA256_Final(hash2, &sha256_2);

    *output_buffer = malloc(SHA256_DIGEST_LENGTH*2 + 1);
    (*output_buffer) [SHA256_DIGEST_LENGTH*2] = '\0';

    for (unsigned int j = 0; j < SHA256_DIGEST_LENGTH; j++)
    {
        sprintf((*output_buffer)+2*j, "%02x", hash2[j]);
    }
}

void hash160(char **output_buffer, const char *src, const int base)
{   
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned char hash2[RIPEMD160_DIGEST_LENGTH];
    MP_INT i;
    mpz_init_set_str(&i, src, base);
    unsigned char bytes[(mpz_sizeinbase(&i, 16)+1)/2];
    mpz_export(bytes, NULL, 1, 1, 1, 0, &i);
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, bytes, sizeof(bytes));
    SHA256_Final(hash, &sha256);
    RIPEMD160_CTX ripemd160;
    RIPEMD160_Init(&ripemd160);
    RIPEMD160_Update(&ripemd160, hash, SHA256_DIGEST_LENGTH);
    RIPEMD160_Final(hash2, &ripemd160);

    *output_buffer = malloc(RIPEMD160_DIGEST_LENGTH*2 + 1);
    (*output_buffer) [RIPEMD160_DIGEST_LENGTH*2] = '\0';

    for (unsigned int j = 0; j < RIPEMD160_DIGEST_LENGTH; j++)
    {
        sprintf((*output_buffer)+2*j, "%02x", hash2[j]);
    }

    /* Free Memory */
    mpz_clear(&i);
}

/* Hexadecimal byte encoding/decoding */

void int_to_little_endian(char **output_buffer, const MP_INT *n, const int byte_size)
{
    *output_buffer = calloc(byte_size*2 + 1, 1);
    unsigned char *bytes = calloc(byte_size, 1);
    memset (bytes, 0, byte_size);

    mpz_export(bytes, NULL, -1, 1, 1, 0, n);
    for (int j = 0; j < byte_size; j++)
    {
        sprintf((*output_buffer)+2*j, "%02x", bytes[j]);
    }

    /* Free Memory */
    free(bytes);
}

void int_to_big_endian(char **output_buffer, const MP_INT *n, const int byte_size)
{
    if (byte_size % 2 != 0 || byte_size > 32) 
    {
        printf("Byte size Error");
        exit(-1);
    }
    *output_buffer = malloc(byte_size*2 + 1);
    (*output_buffer)[byte_size*2] = '\0';
    unsigned char *bytes = malloc(byte_size);
    memset (bytes, 0, byte_size);
    mpz_export(bytes, NULL, 1, 1, 1, 0, n);
    for (int j = 0; j < byte_size; j++)
    {
        sprintf((*output_buffer)+2*j, "%02x",((unsigned char*)(bytes))[j]);
    }

    /* Free Memory */
    free(bytes);
}

void little_endian_to_int(MP_INT *i, char *src)
{
    int l = strlen(src);
    for (int j = 0, k = l; j < l/2; j += 2, k-=2) 
    {   
        char temp = src[k-2];
        src[k-2] = src[j];
        src[j] = temp;

        char temp2 = src[k-1];
        src[k-1] = src[j+1];
        src[j+1] = temp2;
    }
    mpz_init_set_str(i, src, 16);
}

static inline char * flip_endianess(char *src)
{
    int size = strlen(src);
    char *output_buffer = calloc(size+1, 1);

    for (int i = 0,j = size; i < size/2; i += 2, j-=2) 
    {   
        char c1 = src[j-2];
        output_buffer[j-2] = src[i];
        output_buffer[i] = c1;

        char c2 = src[j-1];
        output_buffer[j-1] = src[i+1];
        output_buffer[i+1] = c2;
    }
    return output_buffer;
}

/* Base 58 encoding */

void encode_base_58(char **output_buffer, char *src, int source_base)
{   
    /* Create and count prefix, replace zero's by 1 */
    int prefix_count = 0;
    for (unsigned int j = 0; j < strlen(src); j++)
    {
        if (src[j] == '0')
        {
            prefix_count += 1;
        }
        else 
        {
            break;
        }
    }
    char prefix[prefix_count];
    prefix[prefix_count] = '\0';
    for (int j = 0; j < prefix_count; j++){
        prefix[j] = '1';
    }

    /* Get source number and start encoding */
    MP_INT n, mod;
    mpz_init(&mod); 
    mpz_init_set_str(&n, src, source_base);
    char result[mpz_sizeinbase(&n, 58)];
    int k = 0;
    while (true)
    {
        mpz_divmod_ui(&n,&mod,&n, 58);
        strncat(result, &BASE58_ALPHABET[mpz_get_ui(&mod)], 1);
        k++;
        if (mpz_cmp_ui(&n, 0) == 0) break;
    }

    /* Allocate memory for final buffer */
    *output_buffer = malloc(k + prefix_count + 1);
    memset(*output_buffer, 0, k + prefix_count + 1);
    (*output_buffer)[k + prefix_count] = '\0';
    
    /* Stream prefix and result to output buffer */
    strncpy(*output_buffer, prefix, prefix_count);
    for(unsigned int g = sizeof(result), i = 0; g > sizeof(result) - k - prefix_count; g--, i++) 
    {
        strncat(*output_buffer, result+g, 1);
    }

    /* Free Memory */
    mpz_clears(&n, &mod);
}

void encode_base_58_checksum(char **output_buffer, char* src, const int source_base)
{
    char *temp, *res, *x;
    res = malloc(strlen(src)+9);
    res[strlen(src)+8] = '\0';
    hash256(&temp , src, source_base, false);
    strcpy(res, src);
    extract(&x ,&temp, 4);
    strcat(res , x);
    encode_base_58(output_buffer, res, source_base);

    /* Free Memory */
    free(temp);
    free(res);
    free(x);
}

void decode_base_58(char **output_buffer, char *src)
{
    MP_INT n;
    mpz_init_set_ui(&n, 0);
    for (unsigned int i = 0; i < strlen(src); i++)
    {
        mpz_mul_ui(&n, &n, 58);
        int index= strchr(BASE58_ALPHABET, src[i])-BASE58_ALPHABET;
        mpz_add_ui(&n, &n, index);
    }

    unsigned int size = mpz_sizeinbase(&n, 16);
    *output_buffer = malloc(size+1);
    (*output_buffer)[size] = '\0';

    mpz_get_str(*output_buffer, 16, &n);

    /* Checksum */ 
    char checksum[9];
    memcpy(checksum, *output_buffer+strlen(*output_buffer)-8, 8);

    int lenght = strlen(*output_buffer) - 8;
    char temp[lenght+1];
    memcpy(temp, *output_buffer, lenght+1);
    temp[lenght] = '\0';

    char *hash;
    hash256(&hash, temp, 16, false);

    hash[8] = '\0';
    if (strcmp(hash, checksum) != 0)
    {
        printf("Bad Adress");
        exit(-1);
    }

    /* Free Memory */
    free(hash);
    mpz_clear(&n);
}

/* Variable integer encoding/decoding */

void encode_varint(char **output_buffer, MP_INT *n)
{
    char* hex;
    if (mpz_cmp_ui(n, 0xfd) < 0)
    {
        *output_buffer = calloc(3, 1);
        int_to_little_endian(&hex, n, 1);
        sprintf(*output_buffer, "%s", hex);

        /* Free Memory */
        free(hex);
        return;
    }
    else if (mpz_cmp_ui(n, 0x10000) < 0)
    {
        *output_buffer = calloc(7,1);
        int_to_little_endian(&hex, n, 2);
        sprintf(*output_buffer, "%s", "fd");
        strcat(*output_buffer, hex);

        /* Free Memory */
        free(hex);
        return;
    }
    MP_INT temp;
    mpz_init_set_str(&temp, "100000000", 16);
    if (mpz_cmp(n, &temp) < 0)
    {
        *output_buffer = calloc(11,1);
        int_to_little_endian(&hex, n, 4);
        sprintf(*output_buffer, "%s", "fe");
        strcat(*output_buffer, hex);

        /* Free Memory */
        mpz_clear(&temp);
        free(hex);
        return;
    }
    mpz_init_set_str(&temp, "1000000000000000", 16);
    if (mpz_cmp(n, &temp) < 0)
    {
        *output_buffer = calloc(19,1);
        int_to_little_endian(&hex, n, 8);
        sprintf(*output_buffer, "%s", "ff");
        strcat(*output_buffer, hex);

        /* Free Memory */
        mpz_clear(&temp);
        free(hex);
        return;
    }
    else
    {
        printf("Number too big, can't encode Varint");
        exit(-1);
    } 
}

void decode_varint(MP_INT *output_int, char **src)
{
    char *temp, *x;
    extract(&temp, src, 1);
    if (strcmp(temp, "fd") == 0)
    {
        extract(&x, src, 2);
        little_endian_to_int(output_int, x);
    }
    if (strcmp(temp, "fe") == 0)
    {
        extract(&x, src, 4);
        little_endian_to_int(output_int, x);
    }
    if (strcmp(temp, "ff") == 0)
    {
        extract(&x, src, 8);
        little_endian_to_int(output_int, x);
    }
    else 
    {
        mpz_init_set_str(output_int, *src, 16);
    }

    /* Free Memory */
    free(temp);
    free(x);
}


/* Reading */

void extract(char **dest, char **src, int bytes)
{
    bytes *= 2;
    *dest = malloc(bytes+1);  
    (*dest)[bytes] = '\0';
    memcpy(*dest, *src, bytes);
    memmove(*src, (*src)+bytes, strlen(*src)-bytes);
}
    
#endif