#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    
    // 重複產生p、q質數，直到p*q位數為nbits
    do {
        srand(time(NULL));
        uint64_t pbits = (nbits / 4) + (rand() % (((3 * nbits) / 4) - (nbits / 4) + 1)); // nbits/4 ~ (3 * nbits)/4
        uint64_t qbits = nbits - pbits;

        // printf("%lu \n", pbits);
        // printf("%lu \n", qbits);

        // create prime numbers
        make_prime(p, pbits, iters);
        make_prime(q, qbits, iters);
        mpz_mul(n, p, q);

    } while (!(mpz_sizeinbase(n, 2) == nbits));

    mpz_t pSubOne, qSubOne, gcd_e, pqSubMul;
    mpz_inits(pSubOne, qSubOne, gcd_e, pqSubMul, NULL);

    // (p-1)(q-1)
    mpz_sub_ui(pSubOne, p, 1);
    mpz_sub_ui(qSubOne, q, 1);
    mpz_mul(pqSubMul, pSubOne, qSubOne);

    // find gcd == 1
    do {
        mpz_urandomb(e, state, nbits);
        gcd(gcd_e, e, pqSubMul);
    } while (mpz_cmp_ui(gcd_e, 1) != 0);

    mpz_clears(pSubOne, qSubOne, gcd_e, pqSubMul, NULL);
}

// make the private key
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    mpz_t pSubOne, qSubOne, pqSubMul;
    mpz_inits(pSubOne, qSubOne, pqSubMul, NULL);

    // (p-1)(q-1)
    mpz_sub_ui(pSubOne, p, 1);
    mpz_sub_ui(qSubOne, q, 1);
    mpz_mul(pqSubMul, pSubOne, qSubOne);

    // compute d using inverse of e mod phi(n)
    mod_inverse(d, e, pqSubMul);

    mpz_clears(pSubOne, qSubOne, pqSubMul, NULL);
    return;
}

// s = m^d(mod n)
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char *username, FILE *pbfile) {

    // n, e, s, then user name. With each with a trailing newline
    gmp_fprintf(pbfile,"%Zx\n%Zx\n%Zx\n%s\n", n, e, s, username);
    return;
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {

    // n, d, with trailing newline and in hexstring
    gmp_fprintf(pvfile, "%Zx\n%Zx\n", n, d);
    return;
}