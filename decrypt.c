#include "argparser.h"
#include "numtheory.h"
#include "randstate.h"
#include "ss.h"

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

void decrypt_file(FILE *input_file, FILE *output_file, FILE *pvfile, bool verbose);

void print_help(void);
void print_verbose(const mpz_t pq, const mpz_t d);
void print_verbose_mpz_var(const mpz_t var, const char *name);

/*
    Main function for execution.
    Arguments are parsed and then the inputs are validated.
    The decryption then occurs.
*/
int main(int argc, char **argv) {
    bool help = false;
    bool verbose = false;
    FILE *input_file = stdin;
    FILE *output_file = stdout;
    FILE *pvfile = NULL;

    int response = argparser(argc, argv, &input_file, &output_file, &pvfile, &verbose, &help);

    if (response != 0) {
        if (help) {
            print_help();
        }

        check_null_and_close(input_file);
        check_null_and_close(output_file);
        check_null_and_close(pvfile);
        return -1;
    }

    if (pvfile == NULL) {
        bool is_open = open_file(&pvfile, "ss.priv", "r");
        if (!is_open) {
            fclose(input_file);
            fclose(output_file);
            return -2; //Fail
        }
    }

    decrypt_file(input_file, output_file, pvfile, verbose);

    fclose(input_file);
    fclose(output_file);
    fclose(pvfile);

    return 0;
}

/*
    Decrypt file function that reads pq, d values from private file and decrypt it with ss_decrypt_file
*/
void decrypt_file(FILE *input_file, FILE *output_file, FILE *pvfile, bool verbose) {
    mpz_t d, pq;
    mpz_inits(d, pq, NULL);

    ss_read_priv(pq, d, pvfile);

    if (verbose) {
        print_verbose(pq, d);
    }

    ss_decrypt_file(input_file, output_file, d, pq);

    mpz_clears(d, pq, NULL);
    return;
}

/*
    Prints pq, d keys
*/
void print_verbose(const mpz_t pq, const mpz_t d) {
    print_verbose_mpz_var(pq, "pq  ");
    print_verbose_mpz_var(d, "d   ");
}

/*
    Prints mpz variable according to verbose outline
*/
void print_verbose_mpz_var(const mpz_t var, const char *name) {
    uint32_t bits = (uint32_t) mpz_sizeinbase(var, 2);
    printf("%s(%u bits) = ", name, bits);
    mpz_out_str(stdout, 10, var);
    printf("\n");
    return;
}

/*
    Help statement
*/
void print_help(void) {
    printf("SYNOPSIS\n"
           "   Decrypts data using SS decryption.\n"
           "   Encrypted data is encrypted by the encrypt program.\n\n"

           "USAGE\n"
           "   ./decrypt [OPTIONS]\n\n"

           "OPTIONS\n"
           "   -h              Display program help and usage.\n"
           "   -v              Display verbose program output.\n"
           "   -i infile       Input file of data to decrypt (default: stdin).\n"
           "   -o outfile      Output file for decrypted data (default: stdout).\n"
           "   -n pvfile       Private key file (default: ss.priv).\n");
}
