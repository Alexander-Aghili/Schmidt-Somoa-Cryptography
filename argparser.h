#pragma once

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define OPTIONS "i:o:n:vh"

int argparser(int argc, char **argv, FILE **input_file, FILE **output_file, FILE **pbfile,
    bool *verbose, bool *help);
bool open_file(FILE **file, const char *file_name, const char *mode);
void check_null_and_close(FILE *file);
