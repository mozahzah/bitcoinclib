#include "E:/Bitcoin/bitcoinclib/includes/tx.h"

void txout_init(TxOut *self)
{
    mpz_init_set_ui(&self->amount, 0);
    script_init(&self->script_pub_key);
}

void txout_init_set(TxOut *self, MP_INT *amount, Script *script_pub_key)
{
    mpz_init_set(&self->amount, amount);
    self->script_pub_key = *script_pub_key;
}

void txout_serialize(char **output_buffer, const TxOut *self)
{
    /* Initialization */
    char *amount, *script_pub_key;

    /* Serialization */
    int_to_little_endian(&amount , &self->amount, 8);
    script_serialize(&script_pub_key, &self->script_pub_key);
    
    /* Memory Allocation and Finalization */
    *output_buffer = calloc(strlen(amount)+strlen(script_pub_key), 1);
    strcat(*output_buffer, amount);
    strcat(*output_buffer, script_pub_key);

    /* Free Memory */
    free(amount);
    free(script_pub_key);
}

void txout_parse(TxOut *self, char *src)
{
    char *amount_str;
    MP_INT amount;
    Script script_pub_key;

    extract(&amount_str, &src, 8);
    little_endian_to_int(&amount, amount_str);

    script_parse(&script_pub_key, &src);

    txout_init_set(self, &amount, &script_pub_key);
}