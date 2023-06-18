#pragma once

#include <stdio.h>
#include <gmp.h>
#include <stdint.h>

extern gmp_randstate_t state;

//
// Initializes the random state needed for SS key generation operations.
// Must be called before any key generation or number theory operations are used.
//
// seed: the seed to seed the random state with.
//
void randstate_init(uint64_t seed);

//
// Frees any memory used by the initialized random state.
// Must be called after all key generation or number theory operations are used.
//
void randstate_clear(void);
