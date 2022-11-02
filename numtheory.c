#include "numtheory.h"
#include "randstate.h"

// 產生一個隨機大數，並檢查是不適質數，重複直到產生質數
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mpz_urandomb(p, state, bits); // 0 ~ (2^bits - 1)
    gmp_printf ("%Zd\n", p);
    while (!is_prime(p, iters)){
        mpz_urandomb(p, state, bits);
    }
}

// 用Miller Rabin法檢查是否為質數
bool is_prime(mpz_t p, uint64_t iters) {

    // No need to consider too small numbers, even if it is prime
    if (mpz_cmp_ui(p, 12) <= 0) return false;

    mpz_t two, pSubOne, pSubThree;
    mpz_inits(two, pSubOne, pSubThree, NULL);

    mpz_set_ui(two, 2); // two = 2
    mpz_sub_ui(pSubOne, p, 1); // p - 1
    mpz_sub_ui(pSubThree, p, 3); // p - 3 bound

    mp_bitcnt_t s = 0; // init s for power 2 ^ s
    mpz_t reminder;
    mpz_inits(reminder, NULL);
    // pSubOne一定是偶數
    do {
        mpz_tdiv_q_2exp(reminder, pSubOne, s);
        mpz_fdiv_q_ui(reminder, reminder, 2);
        s += 1;
    } while (mpz_even_p(reminder));

    mp_bitcnt_t sminus = s - 1; // s - 1 for the comparison

    mpz_t a, y, j;
    mpz_inits(a, y, j, NULL);
    // 疊代幾次
    for (uint64_t i = 0; i < iters; i++) {
        
        // a: 隨機挑選 2 ~ (p-1)
        mpz_urandomm(a, state, pSubThree); // 0 ~ (n-1)
        mpz_add_ui(a, a, 2);
        pow_mod(y, a, reminder, p);
        if ((mpz_cmp_ui(y, 1) != 0) && (mpz_cmp(y, pSubOne) != 0)) {
            mpz_set_ui(j, 1);
            while ((mpz_cmp_ui(j, sminus) <= 0) && (mpz_cmp(y, pSubOne) != 0)) {
                pow_mod(y, y, two, p);
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(two, pSubOne, pSubThree, reminder, a, y, j, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }
            if (mpz_cmp(y, pSubOne) != 0) {
                mpz_clears(two, pSubOne, pSubThree, reminder, a, y, j, NULL);
                return false;
            }
        }
    }
    mpz_clears(two, pSubOne, pSubThree, reminder, a, y, j, NULL);
    // print prime
    gmp_printf ("%Zd\n", p);
    return true;
}

// ex: b = 5, e = 3, m = 13, and then 5^3 = 125 which /13 = 8. The final 8 is calculated by the algorithm  
void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus){

    mpz_t b, e, o;
    mpz_inits(b, e, o, NULL);
    mpz_set(b, base);
    mpz_set(e, exponent);
    mpz_set_ui(o, 1);

    while (mpz_cmp_ui(e, 0) > 0) {
        if (mpz_odd_p(e)) {
            mpz_mul(o, o, b);
            mpz_mod(o, o, modulus);
        }
        mpz_mul(b, b, b);
        mpz_mod(b, b, modulus);
        mpz_fdiv_q_ui(e, e, 2);
    }
    mpz_set(out, o);

    mpz_clears(b, e, o, NULL);
}
