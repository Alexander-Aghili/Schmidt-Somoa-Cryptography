#include "numtheory.h"
#include "randstate.h"

//Helper functions not in header file
void mod_inverse_swap(mpz_t, mpz_t, mpz_t);
void set_r_s_values(mpz_t, mpz_t, const mpz_t n);
void create_random_number(mpz_t a, const mpz_t n);
bool witness(mpz_t a, const mpz_t n);

/*
    Function to calculate and set g to the GCD of a & b using Euclid's Method.
*/
void gcd(mpz_t g, const mpz_t a, const mpz_t b) {
    mpz_t temp, c, d;
    mpz_inits(temp, c, d, NULL); //temp = 0

    //Below required such that a, b are maintained
    mpz_set(c, a); //c = a
    mpz_set(d, b); //d = b

    //while d != 0
    while (mpz_cmp_ui(d, 0) != 0) {
        mpz_set(temp, d); //temp = d;
        mpz_mod(d, c, d); //d = c % d;
        mpz_set(c, temp); //c = temp;
    }
    mpz_set(g, c); //g = a;

    mpz_clears(temp, c, d, NULL);
    return;
}

/*
    Swap function for mod_inverse.
    mpz_t variables act as pointers, where values swapped are dereferenced
*/
void mod_inverse_swap(mpz_t x, mpz_t x_prime, mpz_t q) {
    mpz_t temp_swap, temp_q;
    mpz_inits(temp_swap, temp_q, NULL);

    mpz_set(temp_swap, x_prime); //temp = x_prime;

    //Below is x_prime = x - (q * x_prime)
    mpz_mul(temp_q, q, x_prime); //q = q * x_prime
    mpz_sub(x, x, temp_q); //x = x - q
    mpz_set(x_prime, x); //x_prime = x

    mpz_set(x, temp_swap); // x = temp

    mpz_clears(temp_swap, temp_q, NULL);
    return;
}

/*
    Modular inverse function. (a % n )^-1 with output in o.
*/
void mod_inverse(mpz_t o, const mpz_t a, const mpz_t n) {
    mpz_t r, r_prime, t, t_prime;
    mpz_inits(r, r_prime, t, t_prime, NULL);

    mpz_add(r, r, n); //r = n
    mpz_add(r_prime, r_prime, a); //r' = a
    mpz_add_ui(t_prime, t_prime, 1); //t' = 1
        //t = 0

    //while (r_prime != 0)
    while (mpz_cmp_ui(r_prime, 0) != 0) {
        mpz_t quotient;
        mpz_init(quotient);

        mpz_fdiv_q(quotient, r, r_prime); //quotient = r/r_prime;

        mod_inverse_swap(r, r_prime, quotient);
        mod_inverse_swap(t, t_prime, quotient);

        mpz_clear(quotient);
    }

    //if (r > 1)
    if (mpz_cmp_ui(r, 1) > 0) {
        mpz_set_ui(o, 0);
        mpz_clears(r, r_prime, t, t_prime, NULL);
        return;
    }

    //if (t < 0)
    if (mpz_sgn(t) < 0) {
        mpz_add(t, t, n); //t = t + n
    }

    mpz_set(o, t); //o = t
    mpz_clears(r, r_prime, t, t_prime, NULL);
    return;
}

/*
    Performs power mod of a^d % n and outputs in o.
*/
void pow_mod(mpz_t o, const mpz_t a, const mpz_t d, const mpz_t n) {
    mpz_t v, p, e;
    mpz_inits(v, p, e, NULL);

    mpz_set_ui(v, 1); //v = 1
    mpz_set(p, a); //p = a
    mpz_set(e, d); //e = d
    //while e > 0
    while (mpz_cmp_ui(e, 0) > 0) {
        //if e % 2 == 1
        if (mpz_odd_p(e) != 0) {
            //v = (v * p) % n
            mpz_mul(v, v, p); // v = v * p
            mpz_mod(v, v, n); // v = v % n
        }
        //p = (p * p) % n
        mpz_mul(p, p, p); //p = p * p
        mpz_mod(p, p, n); //p = p % n

        mpz_fdiv_q_ui(e, e, 2); //e = e / 2
    }

    mpz_set(o, v); // o = v
    mpz_clears(v, p, e, NULL);
    return;
}

/*
    Sets r and s values for is_prime.
*/
void set_r_s_values(mpz_t r, mpz_t s, const mpz_t n) {
    //s = n - 1
    mpz_set(s, n);
    mpz_sub_ui(s, s, 1);
    //r = 0 *Should already be 0*

    //while (is_even(s))
    while (mpz_even_p(s) != 0) {
        mpz_fdiv_q_ui(s, s, 2); //s = s / 2;
        mpz_add_ui(r, r, 1); //r++
    }

    return;
}

/*
    Creates random number in range [2, n - 2] and puts the random value in a.
    Seed is based off of state from randstate.h
*/
void create_random_number(mpz_t a, const mpz_t n) {
    mpz_t range;
    mpz_init(range);
    mpz_set(range, n); //range = n
    mpz_sub_ui(range, range, 4); //range = range - 4

    mpz_urandomm(a, state, range); //get_random_num(a); [0, range]
    mpz_add_ui(a, a, 2); //a += 2

    mpz_clear(range);
    return;
}

/*
    Witness function for Miller-Rabin Test
*/
bool witness(mpz_t a, const mpz_t n) {
    mpz_t r, s, x, y, two, temp;
    mpz_inits(r, s, x, y, two, temp, NULL);
    mpz_set_ui(two, 2); //two = 2

    //temp = n - 1
    mpz_sub_ui(temp, n, 1);

    set_r_s_values(r, s, n);

    pow_mod(x, a, s, n);

    mpz_t i;
    //for (int i = 0; i < r; i++)
    for (mpz_init(i); mpz_cmp(i, r) < 0; mpz_add_ui(i, i, 1)) {
        pow_mod(y, x, two, n);
        //if (y == 1 && x != 1 && x != n - 1)
        if (mpz_cmp_ui(y, 1) == 0 && mpz_cmp_ui(x, 1) != 0 && mpz_cmp(x, temp) != 0) {
            mpz_clears(i, r, s, x, y, two, temp, NULL);
            return true;
        }
        mpz_set(x, y); //x = y
    }
    mpz_clear(i);
    //return x != 1
    bool return_value = mpz_cmp_ui(x, 1) != 0;
    mpz_clears(r, s, x, y, two, temp, NULL);
    return return_value;
}

/*
    Uses Miller-Rabin test to determine if number is prime.
    No use of init_set functions to avoid accidental allocation.
*/
bool is_prime(const mpz_t n, uint64_t iters) {
    mpz_t temp_mod;
    mpz_init(temp_mod);
    mpz_mod_ui(temp_mod, n, 2); //temp_mod = n % 2

    //if n < 2 || (n != 2 && n % 2 == 0)
    if (mpz_cmp_ui(n, 2) < 0 || (mpz_cmp_ui(n, 2) != 0 && mpz_cmp_ui(temp_mod, 0) == 0)) {
        mpz_clear(temp_mod);
        return false;
    }
    //if (n == 2 || n == 3)
    if (mpz_cmp_ui(n, 2) == 0 || mpz_cmp_ui(n, 3) == 0) {
        mpz_clear(temp_mod);
        return true;
    }

    mpz_clear(temp_mod);

    mpz_t i, a;
    mpz_inits(i, a, NULL);

    //for (int i = 0; i < iters; i++)
    for (; mpz_cmp_ui(i, iters) < 0; mpz_add_ui(i, i, 1)) {
        create_random_number(a, n);
        if (witness(a, n)) {
            mpz_clears(i, a, NULL);
            return false;
        }
    }

    mpz_clears(i, a, NULL);
    return true;
}

/*
    Puts a random prime *bits* bits long into *p* using *iters* number of iterations to 
    check for primality using the Miller-Rabin test.
*/
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mpz_t temp;
    mpz_init(temp);
    //temp = 2^bits;
    mpz_ui_pow_ui(temp, 2, bits);

    do {
        mpz_urandomb(p, state, bits);
        mpz_add(p, p, temp);
    } while (!is_prime(p, iters));
    mpz_clear(temp);
    return;
}
