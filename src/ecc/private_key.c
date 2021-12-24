#include <E:/Bitcoin/bitcoinclib/includes/ecc.h>

/* Initiliazer */

void private_key_init(PrivateKey *self)
{
    mpz_init_set_ui(&self->secret, 0);
}
void private_key_init_set(PrivateKey *self, const char *secret, const int base)
{
    mpz_init_set_str(&self->secret, secret, base);
    secp256k1_point_init(&self->publicPoint);
    secp256k1_point_mul(&self->publicPoint, &G, &self->secret);
}

/* Print */

void private_key_print(const PrivateKey *self, const int base);

/* Signing */

void private_key_sign(EcdsaSignature *signature, const PrivateKey *self, const MP_INT *z)
{
    char *stream, *secret, *z_str, *hash;
    int lenght = mpz_sizeinbase(&self->secret, 16)+mpz_sizeinbase(z, 16);
    stream = malloc(lenght+1);
    stream[lenght] = '\0';

    secret = mpz_get_str(NULL, 16, &self->secret);
    z_str =  mpz_get_str(NULL, 16, z);


    strcpy(stream, secret);
    strcat(stream, z_str);

    hash256(&hash, stream, 16, false);
    MP_INT k, r, s, k_inv, N2, temp; 
    mpz_inits(&k,&r,&s,&k_inv,&N2,&temp, NULL);
    //mpz_init_set_str(&k, hash, 16);
    mpz_init_set_ui(&k, 456);
    Secp256k1Point pk;
    secp256k1_point_init(&pk);
    secp256k1_point_mul(&pk, &G, &k);
    mpz_set(&r, &pk.x.num);


    mpz_sub_ui(&N2, &N, 2);
    mpz_powm(&k_inv, &k, &N2, &N);
    mpz_mul(&temp, &r, &self->secret);
    mpz_add(&temp, &temp, z);
    mpz_mul(&temp, &temp, &k_inv);

    mpz_mod(&s, &temp, &N);

    mpz_div_ui(&temp, &N, 2);
    if (mpz_cmp(&s, &temp) > 0)
    {
        mpz_sub(&s, &N, &s);
    }
    ecdsa_signature_init_set(signature, &r, &s);

    /* Free Memory */
    mpz_clears(&k,&r,&s,&k_inv,&N2,&temp);
    free(stream);
}   