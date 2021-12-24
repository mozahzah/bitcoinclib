#include <E:/Bitcoin/bitcoinclib/includes/ecc.h>

/* Ecdsa Signature */

/* Initializers */

void ecdsa_signature_init(EcdsaSignature *self)
{
    mpz_init_set_ui(&self->r, 0);
    mpz_init_set_ui(&self->s, 0);
}

void ecdsa_signature_init_set(EcdsaSignature *self, const MP_INT* r, const MP_INT* s)
{
    mpz_init_set(&self->r, r);
    mpz_init_set(&self->s, s);
}

/* Print */

void ecdsa_signature_print(const EcdsaSignature *self, const int base)
{
    char *r_str = malloc(mpz_sizeinbase(&self->r, base)+2);
    char *s_str = malloc(mpz_sizeinbase(&self->s, base)+2);
    mpz_get_str(r_str, base, &self->r);
    mpz_get_str(s_str, base, &self->s);
    printf("r: %s, s: %s \n",r_str, s_str);

    /* Free Memory */
    free(r_str);
    free(s_str);
}

/* Formating and Parsing */

void ecdsa_signature_to_der(char **der, const EcdsaSignature *self)
{
    /* Allocate Memory For Byte Array*/
    int r_lenght = ((mpz_sizeinbase(&self->r, 16)+1)/2);
    int s_lenght = ((mpz_sizeinbase(&self->s, 16)+1)/2);
    unsigned char *r_bytes = malloc(r_lenght);
    unsigned char *s_bytes = malloc(s_lenght);

    /* Extract Memory */
    mpz_export(r_bytes, NULL, 1,1,1,0, &self->r);
    mpz_export(s_bytes, NULL, 1,1,1,0, &self->s);

    
    /* Format R Bytes */
    if ((int)r_bytes[0] & 0x80) 
    {
        r_lenght += 3;
        r_bytes = realloc(r_bytes, r_lenght);
        memmove(r_bytes, r_bytes+3, r_lenght);

        r_bytes[0] = '\02';
        r_bytes[1] = r_lenght-2;
        r_bytes[2] = '\00';
    }
    else
    {
        r_lenght += 2;
        r_bytes = realloc(r_bytes, r_lenght);
        memmove(r_bytes, r_bytes+2, r_lenght);

        r_bytes[0] = '\02';
        r_bytes[1] = r_lenght-2;
    }

    char *r_bytes_str = calloc(r_lenght*2+1, 1);
    for (int i = 0; i < r_lenght; i++)
    {
        sprintf(r_bytes_str+2*i, "%02x", r_bytes[i]);
    }

    /* Format S Bytes */
    if ((int)r_bytes[0] & 0x80) 
    {
        s_lenght += 3;
        s_bytes = realloc(s_bytes, s_lenght);
        memmove(s_bytes, s_bytes+3, s_lenght);

        s_bytes[0] = '\02';
        s_bytes[1] = s_lenght-2;
        s_bytes[2] = '\00';
    }
    else
    {
        s_lenght += 2;
        s_bytes = realloc(s_bytes, s_lenght);
        memmove(s_bytes, s_bytes+2, s_lenght);

        s_bytes[0] = '\02';
        s_bytes[1] = s_lenght-2;
    }

    char *s_bytes_str = calloc(s_lenght*2+1, 1);
    for (int i = 0; i < s_lenght; i++)
    {
        sprintf(s_bytes_str+2*i, "%02x", s_bytes[i]);
    }

    /* Finalize Hex String */
    *der = calloc((r_lenght+s_lenght+2)*2+1,1);
    sprintf(*der, "30%02x", r_lenght+s_lenght);
    strcat(*der, r_bytes_str);
    strcat(*der, s_bytes_str);

    /* Free Memory */
    free(r_bytes);
    free(s_bytes);
    free(r_bytes_str);
    free(s_bytes_str);
}

// void ecdsa_signature_parse(EcdsaSignature *signature, const char *src)
// {

// }