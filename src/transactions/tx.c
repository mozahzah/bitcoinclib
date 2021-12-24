#include "E:/Bitcoin/bitcoinclib/includes/tx.h"

void tx_init(Tx *self)
{
    mpz_init_set_ui(&self->version, 0);
    mpz_init_set_ui(&self->txin_size, 0);
    self->tx_ins = calloc(0,0);
    mpz_init_set_ui(&self->txout_size, 0);
    self->tx_outs = calloc(0,0);
    mpz_init_set_ui(&self->locktime, 0);
}

void tx_init_set(Tx *self, MP_INT *version, MP_INT *txin_size, TxIn *tx_ins, MP_INT *txout_size, TxOut *tx_outs, MP_INT *locktime)
{
    mpz_init_set(&self->version, version);
    mpz_init_set(&self->txin_size, txin_size);
    self->tx_ins = tx_ins;//calloc(mpz_get_ui(&self->txin_size), sizeof(TxIn));
    mpz_init_set(&self->txin_size, txout_size);
    self->tx_outs = tx_outs;//calloc(mpz_get_ui(&self->txout_size), sizeof(TxOut));
    mpz_init_set(&self->locktime, locktime);
}

void tx_serialize(char **output_buffer, const Tx *self)
{
    /* Initialization */
    char *version, *txins_lenght, *tx_ins, *txouts_lenght, *tx_outs, *locktime;
    int count = 0;

    /* Serialization */
    int_to_little_endian(&version, &self->version, 4);
    count = strlen(version);
    encode_varint(*txins_lenght, &self->txin_size);
    count += strlen(txins_lenght);
    for (int i = 0; mpz_cmp_ui(&self->txin_size, i-1) == 0; i++)
    {
        char *tx_in;
        txin_serialize(tx_in, &self->tx_ins[i]);
        strcat(tx_ins, tx_in);
        free(tx_in);
    }
    count += strlen(tx_ins);
    encode_varint(*txouts_lenght, &self->txout_size); 
    count += strlen(txouts_lenght);
    for (int i = 0; mpz_cmp_ui(&self->txout_size, i-1) == 0; i++)
    {
        char *tx_out;
        txin_serialize(tx_out, &self->tx_outs[i]);
        strcat(tx_outs, tx_out);
        free(tx_out);
    }
    count += strlen(tx_outs);
    int_to_little_endian(&locktime, &self->locktime, 4);
    count += strlen(locktime);

    /* Memory Allocation and Finalization */
    *output_buffer = calloc(count+1, 1);
    strcat(*output_buffer, version);
    strcat(*output_buffer, txins_lenght);
    strcat(*output_buffer, tx_ins);
    strcat(*output_buffer, txouts_lenght);
    strcat(*output_buffer, tx_outs);
    strcat(*output_buffer, version);

    /* Free Memory */
    free(version);
    free(txins_lenght);
    free(tx_ins);
    free(txouts_lenght);
    free(tx_outs);
    free(version);
}

void tx_parse(Tx *self, char *src, bool testnet)
{
    MP_INT version, txin_size, txout_size, locktime;
    char *version_str, *locktime_str;

    extract(&version_str, &src, 4);
    little_endian_to_int(&version, version_str);

    decode_varint(&txin_size, &src);

    TxIn *tx_ins = calloc(mpz_get_ui(&txin_size), sizeof(TxIn));
    for (int i = 0; mpz_cmp_ui(&txin_size, i-1); i++)
    {
        TxIn tx_in;
        txin_parse(&tx_in, src);
        tx_ins[i] = tx_in;
    }

    decode_varint(&txout_size, &src);

    TxOut *tx_outs = calloc(mpz_get_ui(&txout_size), sizeof(TxOut));
    for (int i = 0; mpz_cmp_ui(&txout_size, i-1); i++)
    {
        TxOut tx_out;
        txin_parse(&tx_out, src);
        tx_outs[i] = tx_out;
    }

    extract(&locktime_str, &src, 4);
    little_endian_to_int(&locktime, locktime_str);
    
    tx_init_set(self, &version, &txin_size, tx_ins, &txout_size, tx_outs, &locktime);
}

void tx_hash_to_sign(Tx *self, MP_INT *output, const MP_INT *input_index)
{
    /* Initialization */
    char *version, *txins_lenght, *tx_ins, *txouts_lenght, *tx_outs, *locktime, *result;
    int count = 0;

    /* Serialization */
    int_to_little_endian(&version, &self->version, 4);
    count = strlen(version);
    encode_varint(*txins_lenght, &self->txin_size);
    count += strlen(txins_lenght);
    for (int i = 0; mpz_cmp_ui(&self->txin_size, i-1) == 0; i++)
    {
        char *tx_in_str;
        TxIn tx_in;
        Script script_pub_key;
        if (mpz_cmp_ui(input_index, i) == 0)
        {
            txin_get_previous_scriptpubKey(&script_pub_key, &self->tx_ins[i]);
            txin_init_set(&tx_in, &self->tx_ins[i].prev_tx,  &self->tx_ins[i].prev_index, &script_pub_key,  &self->tx_ins[i].sequence);
            txin_serialize(tx_in_str, &tx_in);
            strcat(tx_ins, tx_in_str);
        }
        else 
        {
            script_init(&script_pub_key);
            txin_init_set(&tx_in, &self->tx_ins[i].prev_tx,  &self->tx_ins[i].prev_index, &script_pub_key,  &self->tx_ins[i].sequence);
            txin_serialize(tx_in_str, &tx_in);
            strcat(tx_ins, tx_in_str);
        }
        free(tx_in_str);
    }
    count += strlen(tx_ins);
    encode_varint(*txouts_lenght, &self->txout_size); 
    count += strlen(txouts_lenght);
    for (int i = 0; mpz_cmp_ui(&self->txout_size, i-1) == 0; i++)
    {
        char *tx_out;
        txin_serialize(tx_out, &self->tx_outs[i]);
        strcat(tx_outs, tx_out);
        free(tx_out);
    }
    count += strlen(tx_outs);
    int_to_little_endian(&locktime, &self->locktime, 4);
    count += strlen(locktime);

    /* Memory Allocation and Finalization */
    result = calloc(count+1, 1);
    strcat(result, version);
    strcat(result, txins_lenght);
    strcat(result, tx_ins);
    strcat(result, txouts_lenght);
    strcat(result, tx_outs);
    strcat(result, version);

    /* Get final Hash Integer */
    char *h256;
    hash256(&h256, result, 16, false);
    mpz_set_str(output, h256, 16);

    /* Free Memory */
    free(version);
    free(txins_lenght);
    free(tx_ins);
    free(txouts_lenght);
    free(tx_outs);
    free(version);
}

void tx_sign_input(Tx *self, const MP_INT *input_index, const PrivateKey *private_key, const bool compressed)
{
    char *der, *sec;
    EcdsaSignature signature;
    MP_INT z;
    Script script_sig;

    tx_hash_to_sign(self, &z, input_index);
    private_key_sign(&signature, private_key, &z);
    ecdsa_signature_to_der(&der, &signature);
    secp256k1_point_sec(&sec, &private_key->publicPoint, compressed);
    char **s = {der, sec};
    script_init_set(&script_sig, 2, s);
    self->tx_ins[mpz_get_ui(input_index)].script_sig = script_sig;

    /* Free Memory */
    free(der);
    free(sec);
    mpz_clear(&z);
    return true;
}

void tx_fetcher(Tx *self, char *prev_tx_hex)
{
        CURL* curl;
        CURLcode res;
        char *readBuffer;

        curl = curl_easy_init();
        if(curl) 
        {
            char *url = calloc(100,1);
            sprintf(url, "https://blockstream.info/testnet/api/tx/%s/hex", prev_tx_hex);
     
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            if (readBuffer[9] == '0')
            {
                char *temp;
                readBuffer;
                memmove(readBuffer[8], readBuffer[12], strlen(readBuffer)-4);
                tx_parse(self, readBuffer, true);
                MP_INT locktime;
                char *locktime_str = calloc(1000, 1);
                memccpy(locktime_str, readBuffer[strlen(readBuffer)-8], 0, 8);
                little_endian_to_int(&locktime, locktime_str);
                self->locktime = locktime;
            }
            tx_parse(self, readBuffer, true);
        }
        else 
        {
            printf("INVALID URL");
            exit(-1);
        }
}

struct memory {
   char *response;
   size_t size;
 };

size_t write_callback(char *data, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct memory *mem = (struct memory *)userp;
 
    char *ptr = realloc(mem->response, mem->size + realsize + 1);
    if(ptr == NULL)
        return 0;  /* out of memory! */
 
    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;
 
    return realsize;
}