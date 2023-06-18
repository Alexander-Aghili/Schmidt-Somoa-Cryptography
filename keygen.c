#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <gmp.h>

#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>

#include "randstate.h"
#include "ss.h"
#include "argparser.h"

#define KEYGEN_OPTIONS "b:i:n:d:s:vh"

int keygen_argparser(int argc, char **argv, uint32_t *nbits, uint32_t *iters, FILE **pbfile,
    FILE **pvfile, uint64_t *seed, bool *verbose);
uint32_t get_number_from_command_line_argument(char *);

void generate_keys(
    uint32_t nbits, uint32_t iters, FILE *pbfile, FILE *pvfile, uint64_t seed, bool verbose);

void print_help(void);
void print_verbose(const char *username, const mpz_t p, const mpz_t q, const mpz_t n,
    const mpz_t pq, const mpz_t d);
void print_verbose_mpz_var(const mpz_t var, const char *name);

/*
    Main function that initializes all variables, calls parser, verifies valid inputs, and generates keys.
*/
int main(int argc, char **argv) {
    uint32_t nbits = 256;
    uint32_t iters = 50;
    FILE *pbfile = NULL;
    FILE *pvfile = NULL;
    uint64_t seed = (uint64_t) time(NULL);
    bool verbose = false;

    int response = keygen_argparser(argc, argv, &nbits, &iters, &pbfile, &pvfile, &seed, &verbose);

    //Error
    if (response != 0) {
        if (pbfile != NULL) {
            fclose(pbfile);
        }
        if (pvfile != NULL) {
            fclose(pvfile);
        }
        return -1;
    }

    if (pbfile == NULL) {
        bool is_open = open_file(&pbfile, "ss.pub", "w+");
        if (!is_open) {
            return -2; //Fail
        }
    }

    if (pvfile == NULL) {
        bool is_open = open_file(&pvfile, "ss.priv", "w+");
        if (!is_open) {
            return -3;
        }
    }

    fchmod(fileno(pvfile), S_IRUSR + S_IWUSR); //Set file permissions 600 for private file

    generate_keys(nbits, iters, pbfile, pvfile, seed, verbose);

    return 0;
}

/*
    Parses and sets keygen command line arguments
*/
int keygen_argparser(int argc, char **argv, uint32_t *nbits, uint32_t *iters, FILE **pbfile,
    FILE **pvfile, uint64_t *seed, bool *verbose) {
    int opt = 0;
    bool is_open = false;
    while ((opt = getopt(argc, argv, KEYGEN_OPTIONS)) != -1) {
        switch (opt) {
        case 'b':
            *nbits = get_number_from_command_line_argument(optarg);
            if (*nbits < 5) {
                printf("Please enter the number of bits for the public key (> 4)\n");
                return 4;
            }
            break;
        case 'i': *iters = get_number_from_command_line_argument(optarg); break;
        case 'n':
            is_open = open_file(pbfile, optarg, "w+");
            if (!is_open) {
                return 2;
            }
            break;
        case 'd':
            is_open = open_file(pvfile, optarg, "w+");
            if (!is_open) {
                return 3;
            }
            break;
        case 's': *seed = (uint64_t) strtoul(optarg, NULL, 10); break;
        case 'v': *verbose = true; break;
        case 'h': print_help(); return 1;
        default: print_help(); return 1;
        }
    }
    return 0;
}

/*
    Returns number from inputted string command line argument.
*/
uint32_t get_number_from_command_line_argument(char *opt_argument) {
    return (uint32_t) strtoul(opt_argument, NULL, 10);
}

/*
    Generate keys function:
    - Initializes random states.
    - Makes public and private keys
    - Gets username
    - Writes public key to pbfile
    - Writes private key to pvfile
*/
void generate_keys(
    uint32_t nbits, uint32_t iters, FILE *pbfile, FILE *pvfile, uint64_t seed, bool verbose) {
    randstate_init(seed);
    srandom(seed);

    mpz_t p, q, n, pq, d;
    mpz_inits(p, q, n, pq, d, NULL);

    ss_make_pub(p, q, n, nbits, iters);
    ss_make_priv(d, pq, p, q);

    char *username = getenv("USER");

    ss_write_pub(n, username, pbfile);
    fclose(pbfile);

    ss_write_priv(pq, d, pvfile);
    fclose(pvfile);

    if (verbose) {
        print_verbose(username, p, q, n, pq, d);
    }

    mpz_clears(p, q, n, pq, d, NULL);
    randstate_clear();
    return;
}

/*
    Prints verbose arguements to screen.
*/
void print_verbose(const char *username, const mpz_t p, const mpz_t q, const mpz_t n,
    const mpz_t pq, const mpz_t d) {
    printf("user = %s\n", username);
    print_verbose_mpz_var(p, "p   ");
    print_verbose_mpz_var(q, "q   ");
    print_verbose_mpz_var(n, "n   ");
    print_verbose_mpz_var(pq, "pq  ");
    print_verbose_mpz_var(d, "d   ");
    return;
}

/*
    Formats and prints mpz variable
*/
void print_verbose_mpz_var(const mpz_t var, const char *name) {
    uint32_t bits = (uint32_t) mpz_sizeinbase(var, 2);
    printf("%s(%u bits) = ", name, bits);
    mpz_out_str(stdout, 10, var);
    printf("\n");
    return;
}

/*
    Prints help message
*/
void print_help(void) {
    printf("SYNOPSIS\n"
           "   Generates an SS public/private key pair.\n\n"

           "USAGE\n"
           "   ./keygen [OPTIONS]\n\n"

           "OPTIONS\n"
           "   -h              Display program help and usage.\n"
           "   -v              Display verbose program output.\n"
           "   -b bits         Minimum bits needed for public key n (default: 256).\n"
           "   -i iterations   Miller-Rabin iterations for testing primes (default: 50).\n"
           "   -n pbfile       Public key file (default: ss.pub).\n"
           "   -d pvfile       Private key file (default: ss.priv).\n"
           "   -s seed         Random seed for testing.\n");
}
