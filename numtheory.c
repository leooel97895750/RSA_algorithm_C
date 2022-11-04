#include "numtheory.h"
#include "randstate.h"

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
  mpz_urandomb(p, state, bits); // 0 ~ (2^bits - 1)
  // gmp_printf ("%Zd\n", p);
  while (!is_prime(p, iters)) {
    mpz_urandomb(p, state, bits);
  }
}

bool is_prime(mpz_t p, uint64_t iters) {

  // No need to consider too small numbers, even if it is prime
  // Make it clean and faster
  if (mpz_cmp_ui(p, 12) <= 0)
    return false;

  mpz_t two, pSubOne, pSubThree;
  mpz_inits(two, pSubOne, pSubThree, NULL);

  mpz_set_ui(two, 2);          // two = 2
  mpz_sub_ui(pSubOne, p, 1);   // p - 1
  mpz_sub_ui(pSubThree, p, 3); // p - 3 bound

  mp_bitcnt_t s = 0; // init s for power 2 ^ s
  mpz_t reminder;
  mpz_inits(reminder, NULL);
  // pSubOne is even
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

    // a: random choose 2 ~ (p-1)
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

// ex: b = 5, e = 3, m = 13, and then 5^3 = 125 which %13 = 8. The final 8 is
// calculated by the algorithm
void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n) {

  mpz_t base, e, out;
  mpz_inits(base, e, out, NULL);
  mpz_set(base, a);
  mpz_set(e, d);
  mpz_set_ui(out, 1);

  while (mpz_cmp_ui(e, 0) > 0) {
    if (mpz_odd_p(e)) {
      mpz_mul(out, out, base);
      mpz_mod(out, out, n);
    }
    mpz_mul(base, base, base);
    mpz_mod(base, base, n);
    mpz_fdiv_q_ui(e, e, 2);
  }
  mpz_set(o, out);

  mpz_clears(base, e, out, NULL);
}

// Find greatest common divisor
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

// Computes the inverse i of a modulo n. In the event that a modular inverse
// cannot be found, set i to 0
void mod_inverse(mpz_t o, mpz_t a, mpz_t n) {

  mpz_t r, rsub, t, tsub, q, tmp_r, tmp_t;
  mpz_inits(r, rsub, t, tsub, q, tmp_r, tmp_t, NULL);

  mpz_set(r, n);
  mpz_set(rsub, a);
  mpz_set_ui(t, 0);
  mpz_set_ui(tsub, 1);

  while (mpz_cmp_ui(rsub, 0) != 0) {
    mpz_fdiv_q(q, r, rsub);
    mpz_set(tmp_r, r);
    mpz_set(r, rsub);
    mpz_mul(rsub, q, rsub);
    mpz_sub(rsub, tmp_r, rsub);
    mpz_set(tmp_t, t);
    mpz_set(t, tsub);
    mpz_mul(tsub, q, tsub);
    mpz_sub(tsub, tmp_t, tsub);
  }

  if (mpz_cmp_ui(r, 1) > 0) {
    mpz_set_ui(o, 0);
    mpz_clears(r, rsub, t, tsub, q, tmp_r, tmp_t, NULL);
    return;
  }

  if (mpz_cmp_ui(t, 0) < 0) {
    mpz_add(t, t, n);
  }

  mpz_set(o, t);
  mpz_clears(r, rsub, t, tsub, q, tmp_r, tmp_t, NULL);
}
