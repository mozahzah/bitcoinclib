#include <E:/Bitcoin/bitcoinclib/includes/ecc.h>

/* Initialization of secp256k1 constants */

void init_secp256k1()
{
    mpz_init_set_str(&P, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F", 16);
    mpz_init_set_str(&N, "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141", 16);
}

/* secp256k1 Finite Field Element */

/* Initializers */

void secp256k1_field_init(Secp256k1Element *self)
{
    mpz_init_set_ui(&self->num, 0);
}

void secp256k1_field_init_set_str(Secp256k1Element *self, const char *num_hex)
{
    mpz_init_set_str(&self->num, num_hex, 16);
}

void secp256k1_field_init_set(Secp256k1Element *self, const Secp256k1Element *other)
{
    if (self == other) return;
    mpz_init_set(&self->num, &other->num);
}

void secp256k1_field_clear(int n, Secp256k1Element *elements,...)
{
    va_list valist;
    va_start(valist, elements);
    for (int i = 0; i < n -1; i++)
    { 
        mpz_clear(&(va_arg(valist, Secp256k1Element *)->num));
    }

    /* Free Memory */
    mpz_clear(&elements->num);
    va_end(valist);
}

/* Print */

void secp256k1_field_print(const Secp256k1Element *self, const int base)
{
    char *num_str;
    switch (base)
    {
        case 16:
            num_str = malloc(sizeof(char)*65);
            mpz_get_str(num_str, base, &self->num);
            break;
        case 2:
            num_str = malloc(sizeof(char)*257);
            mpz_get_str(num_str, base, &self->num);
            break;
        default:
            num_str = malloc(sizeof(char)*79);
            mpz_get_str(num_str, base, &self->num);
            break;
    }
    printf("%s in secp256k1 Prime Field\n", num_str);
    free(num_str);
}

/* Arithmetic Operations */

void secp256k1_field_add(Secp256k1Element *self, const Secp256k1Element *b, const Secp256k1Element *c)
{
    mpz_add(&self->num, &b->num, &c->num);
    mpz_mod(&self->num, &self->num, &P);
}
void secp256k1_field_sub(Secp256k1Element *self, const Secp256k1Element *b, const Secp256k1Element *c)
{
    mpz_sub(&self->num, &b->num, &c->num);
    mpz_mod(&self->num, &self->num, &P);
}
void secp256k1_field_mul(Secp256k1Element *self, const Secp256k1Element *b, const Secp256k1Element *c)
{
    mpz_mul(&self->num, &b->num, &c->num);
    mpz_mod(&self->num, &self->num, &P);
}
void secp256k1_field_div(Secp256k1Element *self, const Secp256k1Element *b, const Secp256k1Element *c)
{
    /* Initialize */
    MP_INT p, t;
    mpz_inits(&p, &t, NULL);

    mpz_set(&t, &b->num);
    mpz_sub_ui(&p, &P, 2);
    mpz_powm(&t, &c->num, &p, &P);
    if(mpz_cmp_ui(&t, 0) < 0) mpz_add(&self->num, &self->num, &P);
    mpz_mul(&t, &b->num, &t);
    mpz_mod(&self->num, &t, &P);

    /* Free Memory */
    mpz_clear(&p);
    mpz_clear(&t);
}
void secp256k1_field_int_mul(Secp256k1Element *self, const Secp256k1Element *base, const MP_INT *coeff)
{
    mpz_mul(&self->num, &base->num, coeff);
    mpz_mod(&self->num, &self->num, &P);
}
void secp256k1_field_pow(Secp256k1Element *self, const Secp256k1Element *base ,const MP_INT *exponent)
{
    /* Initialize */
    MP_INT p, e;
    mpz_inits(&p, &e, NULL);

    mpz_sub_ui(&p, &P, 1);
    mpz_mod(&e, exponent, &p);
    mpz_powm(&self->num, &base->num, exponent, &P);

    /* Free Memory */
    mpz_clear(&p);
    mpz_clear(&e);
}

/* Comparaison Operations */

bool secp256k1_field_cmp(const Secp256k1Element *a, const Secp256k1Element *b)
{
    if (mpz_cmp(&a->num, &b->num) == 0) return true;
    else return false;
}
bool secp256k1_field_not(const Secp256k1Element *a, const Secp256k1Element *b)
{
    if (secp256k1_field_cmp(a, b) == true) return false;
    else return true;
}

bool secp256k1_field_int_cmp(const Secp256k1Element *a, const unsigned int n)
{
    if (mpz_cmp_ui(&a->num, n) == 0) return true;
    else return false;
}

bool secp256k1_field_int_not(const Secp256k1Element *a, const unsigned int n) 
{
    if (secp256k1_field_int_cmp(a, n) == true) return false;
    else return true;
}




