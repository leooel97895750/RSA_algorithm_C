#include "numtheory.h"
#include "randstate.h"

// 產生一個隨機大數，並檢查是不適質數，重複直到產生質數
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mpz_urandomb(p, state, bits); // 0 ~ (2^bits - 1)
    // gmp_printf ("%Zd\n", p);
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
    // gmp_printf ("%Zd\n", p);
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

// 找出最大公因數
void gcd(mpz_t d, mpz_t a, mpz_t b) {
    mpz_t tmp, tmp_a, tmp_b, aModb;
    mpz_inits(tmp, tmp_a, tmp_b, aModb, NULL);
    mpz_set(tmp_b, b);
    mpz_set(tmp_a, a);

    // gcd(a, b) => gcd (b, a%b)
    while (mpz_cmp_ui(tmp_b, 0) != 0) {
        mpz_set(tmp, tmp_b);
        mpz_mod(aModb, tmp_a, tmp_b);
        mpz_set(tmp_b, aModb);
        mpz_set(tmp_a, tmp);
    }
    mpz_set(d, tmp_a);
    mpz_clears(tmp, tmp_a, tmp_b, aModb, NULL);
}

void mod_inverse(mpz_t o, mpz_t a, mpz_t n) {

    // init all the variable
    mpz_t r, rsub, t, tsub, q, temp_r, temp_t;
    mpz_inits(r, rsub, t, tsub, q, temp_r, temp_t, NULL); // you can inits all at once

    // assigning to var
    mpz_set(r, n); // r = n
    mpz_set(rsub, a); // r' = a
    mpz_set_ui(t, 0); // t = 0
    mpz_set_ui(tsub, 1); // t' = 1

    // while loop

    while (mpz_cmp_ui(rsub, 0) != 0) {

        // this is r and r'
        mpz_fdiv_q(q, r, rsub); // q = (r/r')

        // perform swapping (for r and r')

        mpz_set(temp_r, r); // store r in temp var
        mpz_set(r, rsub); // r <- r'
        mpz_mul(rsub, q, rsub); // r' <- q*r'
        mpz_sub(rsub, temp_r, rsub); // r' <- (r - q*r')

        // perform swapping (for t and t')

        mpz_set(temp_t, t); // store t in temp var
        mpz_set(t, tsub); // t <- t'
        mpz_mul(tsub, q, tsub); // t' <- q*t'
        mpz_sub(tsub, temp_t, tsub); // t' <- (t - q*t')
    }

    if (mpz_cmp_ui(r, 1) > 0) { // if r > 1
        mpz_set_ui(o, 0); // set i to 0
        mpz_clears(r, rsub, t, tsub, q, temp_r, temp_t, NULL);
        return;
    }
    if (mpz_cmp_ui(t, 0) < 0) {
        mpz_add(t, t, n); // tadd = t + n
    }
    mpz_set(o, t); // set to outfile

    mpz_clears(r, rsub, t, tsub, q, temp_r, temp_t, NULL); // free memory to prevent leak
}
