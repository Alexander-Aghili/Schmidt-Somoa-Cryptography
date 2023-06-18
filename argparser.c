#include "argparser.h"

/*
    Parses and correctly sets arguments for encrypt and decrypt since they share the same command line arguments.
    Returns non-zero argument if failed. 
*/
int argparser(int argc, char **argv, FILE **input_file, FILE **output_file, FILE **pbfile,
    bool *verbose, bool *help) {
    int opt = 0;
    bool is_open = false;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            is_open = open_file(input_file, optarg, "r");
            if (!is_open) {
                return 1;
            }
            break;
        case 'o':
            is_open = open_file(output_file, optarg, "w");
            if (!is_open) {
                return 2;
            }
            break;
        case 'n':
            is_open = open_file(pbfile, optarg, "r");
            if (!is_open) {
                return 3;
            }
            break;
        case 'v': *verbose = true; break;
        case 'h': *help = true; return 4;
        default: *help = true; return 5;
        }
    }
    return 0;
}

/*
    Opens file_name into file in mode.
    Prints error message and returns false if failed.
*/
bool open_file(FILE **file, const char *file_name, const char *mode) {
    *file = fopen(file_name, mode);
    if (*file == NULL) {
        printf("%s: No such file or directory\n", file_name);
        return false;
    }
    return true;
}

/*
    Closes a file is not null
*/
void check_null_and_close(FILE *file) {
    if (file != NULL) {
        fclose(file);
    }
    return;
}
