#include "argparser.h"
#include "numtheory.h"
#include "randstate.h"
#include "ss.h"

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

void encrypt_file(FILE *input_file, FILE *output_file, FILE *pbfile, bool verbose);

void print_help(void);
void print_verbose(const char username[], const mpz_t n);

/*
    Main function for execution.
    Arguments are parsed and then the inputs are validated.
    The encryption then occurs.
*/
int main(int argc, char **argv) {
    bool help = false;
    bool verbose = false;
    FILE *input_file = stdin;
    FILE *output_file = stdout;
    FILE *pbfile = NULL;

    int response = argparser(argc, argv, &input_file, &output_file, &pbfile, &verbose, &help);

    if (response != 0) {
        if (help) {
            print_help();
        }

        check_null_and_close(input_file);
        check_null_and_close(output_file);
        check_null_and_close(pbfile);
        return -1;
    }

    if (pbfile == NULL) {
        bool is_open = open_file(&pbfile, "ss.pub", "r");
        if (!is_open) {
            fclose(input_file);
            fclose(output_file);
            return -2; //Fail
        }
    }

    encrypt_file(input_file, output_file, pbfile, verbose);

    fclose(pbfile);
    fclose(input_file);
    fclose(output_file);

    return 0;
}

/*
    Encrypt file function that reads n, username values from private file and encrypt it with ss_encrypt_file
*/
void encrypt_file(FILE *input_file, FILE *output_file, FILE *pbfile, bool verbose) {
    char username[_POSIX_LOGIN_NAME_MAX];
    memset(username, 0, _POSIX_LOGIN_NAME_MAX); //Clear username buffer

    mpz_t n;
    mpz_init(n);

    ss_read_pub(n, username, pbfile);

    if (verbose) {
        print_verbose(username, n);
    }

    ss_encrypt_file(input_file, output_file, n);

    mpz_clear(n);
    return;
}

/*
    Prints n, username
*/
void print_verbose(const char username[], const mpz_t n) {
    printf("user = %s\n", username);
    printf("n (%u bits) = ", (uint32_t) mpz_sizeinbase(n, 2));
    mpz_out_str(stdout, 10, n);
    printf("\n");
    return;
}

/*
    Help statement
*/
void print_help(void) {
    printf("SYNOPSIS\n"
           "  Encrypts data using SS encryption.\n"
           "  Encrypted data is decrypted by the decrypt program.\n\n"

           "USAGE\n"
           "   ./encrypt [OPTIONS]\n\n"

           "OPTIONS\n"
           "   -h              Display program help and usage.\n"
           "   -v              Display verbose program output.\n"
           "   -i infile       Input file of data to encrypt (default: stdin).\n"
           "   -o outfile      Output file for encrypted data (default: stdout).\n"
           "   -n pbfile       Public key file (default: ss.pub).\n");
}
