#include <E:/Bitcoin/bitcoinclib/includes/ecc.h>

/* Initialization of the bitcoin's elliptic curve */

void init_bitcoin_curve()
{
    secp256k1_field_init_set_str(&A, "0");
    secp256k1_field_init_set_str(&B, "7");
    secp256k1_field_init_set_str(&GX, "79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798");
    secp256k1_field_init_set_str(&GY, "483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8");
    secp256k1_point_init_set(&G, &GX, &GY);
}

/* S256 Point Definitions */

/* Initializers */

void secp256k1_point_init(Secp256k1Point *self)
{
    secp256k1_field_init(&self->x);
    secp256k1_field_init(&self->y);
}   

void secp256k1_point_init_set(Secp256k1Point *self, const Secp256k1Element *x, const Secp256k1Element *y)
{
    secp256k1_field_init_set(&self->x, x);
    secp256k1_field_init_set(&self->y, y);
}

void secp256k1_point_set(Secp256k1Point *self, const Secp256k1Point *other)
{
    if (self == other) return;
    secp256k1_field_init_set(&self->x, &other->x);
    secp256k1_field_init_set(&self->y, &other->y);
}

/* Check if the Point is on the Curve */

bool secp256k1_point_isvalid(Secp256k1Point *self)
{
    if (secp256k1_field_int_cmp(&self->x,0) == true && secp256k1_field_int_cmp(&self->y,0) == true) return true;
    MP_INT two, three;
    mpz_init_set_ui(&two, 2);
    mpz_init_set_ui(&three, 3);

    Secp256k1Element y2, x3, i, j;
    secp256k1_field_init(&y2);
    secp256k1_field_init(&x3);
    secp256k1_field_init(&i);
    secp256k1_field_init(&j);

    secp256k1_field_pow(&y2, &self->y, &two);
    secp256k1_field_mul(&i, &A, &self->x);
    secp256k1_field_add(&i, &i, &B);
    secp256k1_field_pow(&x3, &self->x, &three);
    secp256k1_field_add(&x3, &x3, &i);

    if (secp256k1_field_cmp(&y2, &x3) == false)
    {
        printf("Error Initiliazing Point\n");
        return false;
    }
    else 
    {
        return true;
    }
}

/* Print */

void secp256k1_point_print(const Secp256k1Point *self, const int base)
{
    char *x_str;
    char *y_str;
    char *a_str;
    char *b_str;
    x_str = malloc(mpz_sizeinbase(&self->x.num, base)+2);
    y_str = malloc(mpz_sizeinbase(&self->y.num, base)+2);
    a_str = malloc(mpz_sizeinbase(&A.num, base)+2);
    b_str = malloc(mpz_sizeinbase(&B.num, base)+2);
    mpz_get_str(x_str, base, &self->x.num);
    mpz_get_str(y_str, base, &self->y.num);
    mpz_get_str(a_str, base, &A.num);
    mpz_get_str(b_str, base, &B.num);
    
    printf("(%s, %s) on curve %s and %s\n", x_str, y_str, a_str, b_str);

    /* Free Memory */
    free(x_str);
    free(y_str);
    free(a_str);
    free(b_str);
}

/* Arithmetic Operations */

void secp256k1_point_add(Secp256k1Point *self, const Secp256k1Point *p1, const Secp256k1Point *p2)
{
    /* Useful temp variables */
    MP_INT two, three;
    mpz_init_set_ui(&two, 2);
    mpz_init_set_ui(&three, 3);
    Secp256k1Element i, j, s, x, y;
    secp256k1_field_init(&i);
    secp256k1_field_init(&j);
    secp256k1_field_init(&s);
    secp256k1_field_init(&x);
    secp256k1_field_init(&y);

    if (secp256k1_field_int_cmp(&p1->x, 0) == true)
    {
        secp256k1_point_set(self, p2);
    }
    else if (secp256k1_field_int_cmp(&p2->x, 0) == true)
    {
        secp256k1_point_set(self, p1);
    }
    else if (secp256k1_field_cmp(&p1->x, &p2->x) == true && secp256k1_field_cmp(&p1->y, &p2->y) == false)
    {
        secp256k1_field_init(&self->x);
        secp256k1_field_init(&self->y);
    }
    else if (secp256k1_field_cmp(&p1->x, &p2->x) == false)
    {
        /* s = (p2.y - p1.y) / (p2.x - p1.x) */
        secp256k1_field_sub(&i, &p2->x, &p1->x);
        secp256k1_field_sub(&j, &p2->y, &p1->y);
        secp256k1_field_div(&s, &j, &i);
        /* x = s.pow(2) - p1.x - p2.x; */
        secp256k1_field_pow(&j, &s, &two);
        secp256k1_field_sub(&i, &j, &p1->x);
        secp256k1_field_sub(&x, &i, &p2->x);
        /* y = s * (p1.x - x) - p1.y; */
        secp256k1_field_sub(&i, &p1->x, &x);
        secp256k1_field_mul(&j, &s, &i);
        secp256k1_field_sub(&y, &j, &p1->y);

        secp256k1_field_init_set(&self->x, &x);
        secp256k1_field_init_set(&self->y, &y);

        secp256k1_field_clear(5, &i, &j, &s, &x, &y);
    }
    else if (secp256k1_point_cmp(p1, p2) == true && secp256k1_field_int_cmp(&p2->y, 0))
    {
        secp256k1_field_init(&self->x);
        secp256k1_field_init(&self->y);
    }
    else if (secp256k1_point_cmp(p1, p2) == true)
    {
        /* s = (&p1->x.pow(2) * 3 + p1.a) / (p1.y * 2); */
        secp256k1_field_int_mul(&i, &p1->y, &two);
        secp256k1_field_pow(&j, &p1->x, &two);
        secp256k1_field_int_mul(&j, &j, &three);
        secp256k1_field_add(&j, &j, &A);
        secp256k1_field_div(&s, &j, &i);
        /* x = s.pow(2) - (&p1->x * 2); */
        secp256k1_field_int_mul(&i, &p1->x, &two);
        secp256k1_field_pow(&j, &s, &two);
        secp256k1_field_sub(&x, &j, &i);
        /* y = s * (&self->x - x) - &self->y; */
        secp256k1_field_sub(&i, &p1->x, &x);
        secp256k1_field_mul(&j, &s, &i);
        secp256k1_field_sub(&y, &j, &p1->y);

        secp256k1_field_init_set(&self->x, &x);
        secp256k1_field_init_set(&self->y, &y);

        secp256k1_field_clear(5, &i, &j, &s, &x, &y);
    }
    else
    {
        printf("Something went Wrong while adding points");
    }
}

void secp256k1_point_mul(Secp256k1Point *self, const Secp256k1Point *p, const MP_INT *coeff)
{
    MP_INT c, one, i;
    mpz_inits(&c, &one, &i, NULL);
    mpz_set_ui(&one, 1);
    mpz_mod(&c, coeff, &N);

    Secp256k1Point result, current;
    secp256k1_point_init(&result);
    secp256k1_point_set(&current, p);
    while (mpz_cmp_ui(&c, 0) != 0)
    {
        mpz_and(&i, &c, &one);
        if (mpz_cmp_ui(&i, 1) == 0)
        {
            secp256k1_point_add(&result, &result, &current);
        }
        secp256k1_point_add(&current, &current, &current);
        mpz_fdiv_q_2exp(&c, &c, 1);
    }
    secp256k1_point_set(self, &result);
}

/* Comparaison Operations */

bool secp256k1_point_cmp(const Secp256k1Point *self, const Secp256k1Point *p)
{
    if (secp256k1_field_cmp(&self->x, &p->x) == true && secp256k1_field_cmp(&self->y, &p->y) == true) return true;
    else return false;
}

bool secp256k1_point_not(const Secp256k1Point *self, const Secp256k1Point *p)
{
    if (secp256k1_point_cmp(self, p) == true) return false;
    else return true;
}

/* Signature Verification */

bool secp256k1_point_verify(const Secp256k1Point *self, const MP_INT *z, const EcdsaSignature *signature)
{
    MP_INT s_inv, u, v, N2;
    mpz_inits(&s_inv, &u, &v, &N2, NULL);
    Secp256k1Point total, temp;
    mpz_sub_ui(&N2, &N, 2);
    //s_inv = powm(sig.s, N-2, N);
    mpz_powm(&s_inv, &signature->s, &N2, &N);
    //u = z * s_inv % N;
    mpz_mul(&u, z, &s_inv);
    mpz_mod(&u, &u, &N);
    //v = sig.r * s_inv % N;
    mpz_mul(&v, &signature->r, &s_inv);
    mpz_mod(&v, &v, &N);
    //point total = G * u + *this * v;
    secp256k1_point_mul(&temp, self, &v);
    secp256k1_point_mul(&total, &G, &u);
    secp256k1_point_add(&total, &total, &temp);
    if (mpz_cmp(&total.x.num, &signature->r) == 0) return true;
    else return false;
}

void secp256k1_point_sec(char **output_buffer, const Secp256k1Point *self, bool compressed)
{
    
    if (compressed)
    {
        *output_buffer = calloc(67, 1);

        MP_INT temp;
        mpz_init(&temp);
        mpz_mod_ui(&temp, &self->y.num, 2);
        if (mpz_cmp_ui(&temp, 0) == 0)
        {
            sprintf(*output_buffer, "%s", "02");
            strcat(*output_buffer, mpz_get_str(NULL, 16, &self->x.num));
        }
        else
        {
            sprintf(*output_buffer, "%s", "03");
            strcat(*output_buffer, mpz_get_str(NULL, 16, &self->x.num));
        }
    }
    else
    {  
        *output_buffer = calloc(133,1);

        (*output_buffer)[0] = '0';
        (*output_buffer)[1] = '4';
        strcat(*output_buffer, mpz_get_str(NULL, 16, &self->x.num));
        strcat(*output_buffer, mpz_get_str(NULL, 16, &self->y.num));
    }
}

void secp256k1_point_address(char **output_buffer, const Secp256k1Point *self, bool compressed,bool testnet)
{
    char *h160, *sec, *address;
    address = calloc(60, 1);

    secp256k1_point_sec(&sec, self, compressed);
    hash160(&h160, sec, 16);
    if (testnet)
    {
        sprintf(address, "%s", "6f");
        strcat(address, h160);
    }
    else{
        sprintf(address, "%s", "00");
        strcat(address, h160);
    }
    encode_base_58_checksum(output_buffer, address, 16);

    free(address);
    free(sec);
    free(h160);
}