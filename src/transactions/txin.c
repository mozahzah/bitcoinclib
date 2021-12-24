#include "E:/Bitcoin/bitcoinclib/includes/tx.h"

void txin_init(TxIn *self)
{
    mpz_init_set_ui(&self->prev_index, 0);
    self->prev_tx = "00";
    script_init(&self->script_sig);
    mpz_init_set(&self->sequence, 0);
}
void txin_init_set(TxIn *self, char *prev_tx, MP_INT *prev_index, Script *script, MP_INT *sequence)
{
    self->prev_tx = prev_tx;
    mpz_init_set(&self->prev_index, prev_index);
    self->script_sig = *script;
    mpz_init_set(&self->sequence, sequence);
}

void txin_serialize(char **output_buffer, TxIn *self)
{
    /* Initialization */
    int count = 0;
    count += strlen(self->prev_tx);
    char *temp, *sequence, *script_sig, *prev_tx;

    /* Serialization */
    prev_tx = flip_endianess(self->prev_tx);
    int_to_little_endian(&temp, &self->prev_index, 4);
    count += strlen(temp);
    script_serialize(&script_sig, &self->script_sig);
    count += strlen(script_sig);
    int_to_little_endian(&sequence, &self->sequence, 4);
    count += strlen(sequence);

    /* Memory Allocation and Finalization */
    *output_buffer = calloc(count+1, 1);
    strcat(*output_buffer, prev_tx);
    strcat(*output_buffer, temp);
    strcat(*output_buffer, script_sig);
    strcat(*output_buffer, sequence);

    /* Free Memory */
    free(temp);
    free(sequence);
    free(script_sig);
    free(prev_tx);
}
void txin_get_previous_scriptpubKey(Script *script, const TxIn *self)
{

}
void txin_parse(TxIn *self, char *src)
{
    char *prev_tx, *prev_index_str,*sequence_str;
    MP_INT sequence, prev_index;
    Script script_sig;

    extract(&prev_tx, &src, 32);

    extract(&prev_index_str, &src, 4);
    little_endian_to_int(&prev_index, prev_index_str);

    script_parse(&script_sig, &src);

    extract(&sequence_str, &src, 4);
    little_endian_to_int(&sequence, sequence_str);

    txin_init_set(self, flip_endianess(prev_tx), &prev_index, &script_sig, &sequence);
}