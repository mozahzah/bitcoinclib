#include <C:/msys64/mingw64/include/gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "C:/msys64/mingw64/include/curl/curl.h"
#include "E:/Bitcoin/bitcoinclib/includes/helper.h"
#include "E:/Bitcoin/bitcoinclib/includes/script.h"
#include "E:/Bitcoin/bitcoinclib/includes/ecc.h"

#ifndef TX
#define TX

typedef struct
{
    char *prev_tx;
    MP_INT prev_index; 
    Script script_sig;
    MP_INT sequence;

} TxIn;

void txin_init(TxIn *self);
void txin_init_set(TxIn *self, char *prev_tx, MP_INT *prev_index, Script *script, MP_INT *sequence);

void txin_serialize(char **output_buffer, TxIn *self);
void txin_get_previous_scriptpubKey(Script *script, const TxIn *self);
void txin_parse(TxIn *self, char *src);

typedef struct
{
    MP_INT amount;
    Script script_pub_key;
} TxOut;

void txout_init(TxOut *self);
void txout_init_set(TxOut *self, MP_INT *amount, Script *script_pub_key);

void txout_serialize(char **output_buffer, const TxOut *self);
void txout_parse(TxOut *self, char *src);

typedef struct
{
    MP_INT version;
    MP_INT txin_size;
    TxIn *tx_ins;
    MP_INT txout_size;
    TxOut *tx_outs;
    MP_INT locktime;
} Tx;

void tx_init(Tx *self);
void tx_init_set(Tx *self, MP_INT *version, MP_INT *txin_size, TxIn *tx_ins, MP_INT *txout_size, TxOut *tx_outs, MP_INT *locktime);

void tx_hash_to_sign(Tx *self, MP_INT *output, const MP_INT *input_index);

void tx_sign_input(Tx *self, const MP_INT *input_index, const PrivateKey *private_key, const bool Compressed);
bool tx_verify_input(Tx *self, const MP_INT *input_index);

void tx_serialize(char **output_buffer, const Tx *self);
void tx_parse(Tx *self, char *src, bool testnet);

void tx_fetcher(Tx *self, char *prev_tx_hex);
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

#endif