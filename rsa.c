#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    
    // 重複產生p、q質數，直到p*q位數為nbits
    do {
        srand(time(NULL));
        uint64_t pbits = lower + (rand() % (((3 * nbits) / 4) - (nbits / 4) + 1)); // nbits/4 ~ (3 * nbits)/4
        uint64_t qbits = nbits - pbits;

        printf("%lu \n", pbits);
        printf("%lu \n", qbits);

        // create prime numbers
        make_prime(p, pbits, iters);
        make_prime(q, qbits, iters);
        mpz_mul(n, p, q);

    } while (!(mpz_sizeinbase(n, 2) == nbits));

    mpz_t p_minus, q_minus, gcd_e, temp_n;
    mpz_inits(p_minus, q_minus, gcd_e, temp_n, NULL);

    // compute Euler totient function
    // toitent(n) = (p-1)(q-1)
    // mpz_sub_ui(p_minus, p, 1); // p - 1
    // mpz_sub_ui(q_minus, q, 1); // q - 1
    // mpz_mul(temp_n, p_minus, q_minus); // totient

    // do {
    //     mpz_urandomb(e, state, nbits); // generate random num in e
    //     gcd(gcd_e, e, temp_n); // store into gcd_e
    // } while (mpz_cmp_ui(gcd_e, 1) != 0); // while the gcd_e is not the greatest common divisor

    // mpz_clears(p_minus, q_minus, gcd_e, temp_n, NULL);
    // return;
}