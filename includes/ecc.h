#include <C:/msys64/mingw64/include/gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "E:/Bitcoin/bitcoinclib/includes/helper.h"

#ifndef ECC
#define ECC

#define bool int
#define true 1
#define false 0

/* Initialization of secp256k1 constants */

MP_INT P, N;
void init_secp256k1();

/* secp256k1 Finite Field Element */

typedef struct
{
    MP_INT num;
} Secp256k1Element;
 
/* Initializer */
void secp256k1_field_init(Secp256k1Element *self);
void secp256k1_field_init_set_str(Secp256k1Element *self, const char *num_hex);
void secp256k1_field_init_set(Secp256k1Element *self, const Secp256k1Element *other);
void secp256k1_field_clear(int n, Secp256k1Element *self,...);

/* Print */

void secp256k1_field_print(const Secp256k1Element *self, const int base);

/* Arithmetic Operations */

void secp256k1_field_add(Secp256k1Element *self, const  Secp256k1Element *b, const Secp256k1Element *c);
void secp256k1_field_sub(Secp256k1Element *self, const  Secp256k1Element *b, const Secp256k1Element *c);
void secp256k1_field_mul (Secp256k1Element *self, const Secp256k1Element *b, const Secp256k1Element *c);
void secp256k1_field_div (Secp256k1Element *self, const Secp256k1Element *b, const Secp256k1Element *c);
void secp256k1_field_int_mul(Secp256k1Element *self, const Secp256k1Element *base, const MP_INT* coeff);
void secp256k1_field_pow(Secp256k1Element *self, const Secp256k1Element *base, const MP_INT* exponent);

/* Comparaison Operations */

bool secp256k1_field_cmp(const Secp256k1Element *self, const Secp256k1Element *b);
bool secp256k1_field_not(const Secp256k1Element *self, const Secp256k1Element *b);
bool secp256k1_field_int_cmp(const Secp256k1Element *self, const unsigned int n);
bool secp256k1_field_int_not(const Secp256k1Element *self, const unsigned int n);

/* Initialize Bitcoin Curve and Properties */

Secp256k1Element A;
Secp256k1Element B;
Secp256k1Element GX;
Secp256k1Element GY;
void init_bitcoin_curve();

/* Bitcoin Elleptic Curve Point */

typedef struct
{
    Secp256k1Element x;
    Secp256k1Element y;
} Secp256k1Point;
Secp256k1Point G; // Bitcoin Point

/* Initializer */

void secp256k1_point_init(Secp256k1Point *self);
void secp256k1_point_init_set(Secp256k1Point *self, const Secp256k1Element *x, const Secp256k1Element *y);
void secp256k1_point_set(Secp256k1Point *self, const Secp256k1Point *other);
bool secp256k1_point_isvalid(Secp256k1Point *self);

/* Print */

void secp256k1_point_print(const Secp256k1Point *self, const int base);

/* Arithmetic Operations */

void secp256k1_point_add(Secp256k1Point *self, const Secp256k1Point *p1,const Secp256k1Point *p2);
void secp256k1_point_mul(Secp256k1Point *self, const Secp256k1Point *p, const MP_INT* coeff);

/* Comparaison Operations */

bool secp256k1_point_cmp(const Secp256k1Point *self, const Secp256k1Point *p);
bool secp256k1_point_not(const Secp256k1Point *self, const Secp256k1Point *p);

/* Formating and Parsing */

void secp256k1_point_sec(char **output_buffer, const Secp256k1Point *self, bool compressed);
void secp256k1_point_address(char **output_buffer, const Secp256k1Point *self, bool compressed,bool testnet);
//static Secp256k1Point Parse(std::string sec_pubkey);

/* ECDSA Signature */

typedef struct
{
    MP_INT r;
    MP_INT s;
} EcdsaSignature;

/* Initializer */

void ecdsa_signature_init(EcdsaSignature *self);
void ecdsa_signature_init_set(EcdsaSignature *self, const MP_INT* r, const MP_INT* s);

/* Print */

void ecdsa_signature_print(const EcdsaSignature* self, const int base);

/* Formating and Parsing */

void ecdsa_signature_to_der(char **der, const EcdsaSignature *self);
bool secp256k1_point_verify(const Secp256k1Point *self, const MP_INT *z, const EcdsaSignature *signature);
//void ecdsa_signature_parse(EcdsaSignature *signature, const char *src);

/* Private Key */

typedef struct
{
    MP_INT secret;
    Secp256k1Point publicPoint;
} PrivateKey;

/* Initiliazer */

void private_key_init(PrivateKey *self);
void private_key_init_set(PrivateKey *self, const char *secret, const int base);

/* Print */

void private_key_print(const PrivateKey* self, const int base);

/* Signing */

void private_key_sign(EcdsaSignature *signature, const PrivateKey *self, const MP_INT *z);



#endif