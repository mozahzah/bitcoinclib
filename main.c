
#include "src/ecc/secp256k1_field.c"
#include "src/ecc/secp256k1_point.c"
#include "src/ecc/ecdsa_signature.c"
#include "src/ecc/private_key.c"
#include "src/script.c"
#include "src/transactions/txin.c"
#include "src/transactions/txout.c"
#include "src/helper.c"


#include <stdio.h>
#include <string.h>
#include "src/transactions/tx.c"


int main ()
{
    init_secp256k1();
    init_bitcoin_curve();
    PrivateKey priv;
    private_key_init_set(&priv, "72437122637458162359935176823685902471865219133957958617126458680489894763219", 10);

    char *out;
    Tx tx;
    MP_INT i, one, sequence, amount, zero;
    mpz_init_set_str(&sequence, "ffffffff", 16);
    mpz_init_set_ui(&i, 1);
    mpz_init_set_ui(&zero, 0);
    mpz_init_set_ui(&one, 1);
    Script script;
    script_init(&script);
    TxIn txin;
    txin_init_set(&txin, "00dceb59a75a49c2db133c5b5fbbaa6b1d4a3222aec4ce3c07e3399e9d273d0e", &one, &script, &sequence);
    TxIn *txins = calloc(500, 1);
    txins[0] = txin;
    mpz_init_set_ui(&amount, 2000);
    TxOut txout;
    Script p2pk;
    script_init_set_p2pkh(&p2pk, "n4TrCtCVPQaorW2WdHvJfH2bWh3RvL1StM");

    txout_init_set(&txout, &amount, &p2pk);
    TxOut *txouts = calloc(500, 1);
    txouts[0] = txout;

    tx_init_set(&tx, &i, &one, txins, &one, txouts, &zero);
    tx_sign_input(&tx, &one, &priv, true);
    tx_serialize(&out, &tx);

    printf(out);
    free(out);

    return 0;
}
