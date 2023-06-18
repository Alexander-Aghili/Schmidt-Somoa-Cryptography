#include "ss.h"
#include "numtheory.h"
#include "randstate.h"

#include <stdlib.h>
#include <time.h>
#include <string.h>

void get_n_from_p_q(mpz_t n, const mpz_t p, const mpz_t q);
void lcm(mpz_t o, const mpz_t a, const mpz_t b);
void get_k(size_t *k, const mpz_t var);

gmp_randstate_t state;

/*
    Makes public key by:
     - p is randomly generated number with bits in range [nbits/5, 2*nbits/5]
     - q is randomly generated number with remaining bits from nbits - 2*pbits
     - n = p^2*q
*/
void ss_make_pub(mpz_t p, mpz_t q, mpz_t n, uint64_t nbits, uint64_t iters) {
    uint64_t pbits = (uint64_t) (random() % (nbits / 5)) + (nbits / 5); //[nbits/5, 2*nbits/5]
    uint64_t qbits = nbits - (2 * pbits);

    make_prime(p, pbits, iters);

    mpz_t temp_p, temp_q, mod_p, mod_q;
    mpz_inits(temp_p, temp_q, mod_p, mod_q, NULL);

    do {
        make_prime(q, qbits, iters);

        mpz_sub_ui(temp_p, p, 1);
        mpz_sub_ui(temp_q, q, 1);

        mpz_mod(mod_p, p, temp_q);
        mpz_mod(mod_q, q, temp_p);
    } while (mpz_cmp_ui(mod_p, 0) == 0 || mpz_cmp_ui(mod_q, 0) == 0);

    get_n_from_p_q(n, p, q);

    mpz_clears(temp_p, temp_q, mod_p, mod_q, NULL);

    return;
}

/*
    Sets n from p and q
    n = p*p*q
*/
void get_n_from_p_q(mpz_t n, const mpz_t p, const mpz_t q) {
    mpz_t temp_p;
    mpz_init(temp_p);

    mpz_mul(temp_p, p, p); //temp_p = p*p
    mpz_mul(n, temp_p, q); //n = (p*p)*q

    mpz_clear(temp_p);
    return;
}

/*
    Writes n and username to pbfile
*/
void ss_write_pub(const mpz_t n, const char username[], FILE *pbfile) {
    mpz_out_str(pbfile, 16, n);
    fputc('\n', pbfile);
    fputs(username, pbfile);
    fputc('\n', pbfile);
    return;
}

/*
    Reads and places n and username from pbfile
*/
void ss_read_pub(mpz_t n, char username[], FILE *pbfile) {
    mpz_inp_str(n, pbfile, 16);
    fscanf(pbfile, "%s", username);
    return;
}

/*
    Makes private keys d and pq using p and q 
*/
void ss_make_priv(mpz_t d, mpz_t pq, const mpz_t p, const mpz_t q) {
    mpz_t n, temp_p, temp_q, lambda;
    mpz_inits(n, temp_p, temp_q, lambda, NULL);

    mpz_mul(temp_p, p, p); //temp_p = p*p
    mpz_mul(n, temp_p, q); //n = q * (p * p)
    mpz_sub_ui(temp_p, p, 1); //temp_p = p - 1
    mpz_sub_ui(temp_q, q, 1); //temp_q = q - 1

    mpz_mul(pq, p, q); //pq = p * q
    lcm(lambda, temp_p, temp_q); //lambda = lcm(p-1, q-1)

    mod_inverse(d, n, lambda);

    mpz_clears(n, temp_p, temp_q, lambda, NULL);
    return;
}

/*
    Calculate the lowest common multiple of a and b
    o = lcm(a,b)
*/
void lcm(mpz_t o, const mpz_t a, const mpz_t b) {
    mpz_t temp;
    mpz_init(temp);
    mpz_mul(o, a, b); //o = a*b
    gcd(temp, a, b); //temp = gcd(a, b)
    mpz_fdiv_q(o, o, temp); //o = o/temp = (a*b)/gcd(a,b)
    mpz_clear(temp);
    return;
}

/*
    Writes pq and d to pvfile
*/
void ss_write_priv(const mpz_t pq, const mpz_t d, FILE *pvfile) {
    mpz_out_str(pvfile, 16, pq);
    fputc('\n', pvfile);
    mpz_out_str(pvfile, 16, d);
    fputc('\n', pvfile);
    return;
}

/*
    Reads and places pq and d into pvfile
*/
void ss_read_priv(mpz_t pq, mpz_t d, FILE *pvfile) {
    mpz_inp_str(pq, pvfile, 16);
    fscanf(pvfile, "\n");
    mpz_inp_str(d, pvfile, 16);
    return;
}

/*
    Encrypts message m with public key n using powermod, places result in c.
*/
void ss_encrypt(mpz_t c, const mpz_t m, const mpz_t n) {
    pow_mod(c, m, n, n);
    return;
}

/*
    Encrypts contents on infile and outputs that to outfile using public key n.
    Encrypts in blocks of size k.  
*/
void ss_encrypt_file(FILE *infile, FILE *outfile, const mpz_t n) {
    mpz_t root, block_data, encrypted_data;
    mpz_inits(root, block_data, encrypted_data, NULL);

    size_t k;
    mpz_sqrt(root, n);
    get_k(&k, root);

    uint8_t *write_contents = (uint8_t *) calloc(k, sizeof(uint8_t));

    size_t read_bytes;
    do {
        write_contents[0] = 0xFF; //Prepend 0xFF byte
        read_bytes = fread(write_contents + 1, sizeof(uint8_t), k - 1, infile);
        if (read_bytes == 0) {
            break; //Nothing read
        }
        mpz_import(block_data, read_bytes + 1, 1, sizeof(uint8_t), 1, 0, (void *) write_contents);
        ss_encrypt(encrypted_data, block_data, n);
        mpz_out_str(outfile, 16, encrypted_data);
        fputc('\n', outfile);
    } while (read_bytes == (k - 1));

    free(write_contents);

    mpz_clears(root, block_data, encrypted_data, NULL);
    return;
}

/*
    Gets k using lg(k)-1/8
*/
void get_k(size_t *k, const mpz_t var) {
    *k = mpz_sizeinbase(var, 2);
    *k = *k - 1;
    *k = *k / 8;
    return;
}

/*
    Decrypts using power mod with c, d and pq, outputting to m.
*/
void ss_decrypt(mpz_t m, const mpz_t c, const mpz_t d, const mpz_t pq) {
    pow_mod(m, c, d, pq);
    return;
}

/*
    Decrypts infile in blocks of size k using private keys d and pq and outputs message into outfile. 
*/
void ss_decrypt_file(FILE *infile, FILE *outfile, const mpz_t d, const mpz_t pq) {
    mpz_t c, m;
    mpz_inits(c, m, NULL);

    size_t k;
    get_k(&k, pq);

    uint8_t *read_contents = (uint8_t *) calloc(k, sizeof(uint8_t));

    while (mpz_inp_str(c, infile, 16) > 0) {
        ss_decrypt(m, c, d, pq);

        mpz_export((void *) read_contents, &k, 1, sizeof(uint8_t), 1, 0, m);

        for (int i = 1; i < (uint8_t) k; i++) {
            uint8_t read_character = read_contents[i];
            if (read_character == 0x00) {
                break;
            }
            fputc(read_character, outfile);
        }
    }

    free(read_contents);

    if (ferror(infile)) {
        printf("Error parsing input file.\n");
        mpz_clears(c, m, NULL);
        return;
    }

    mpz_clears(c, m, NULL);
    return;
}
