// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2021 PotatoTech

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct Options {
    const char* mappings;
    int16_t deadzone;
    bool verbose;
} Options;

Options parse_arguments(int argc, const char* argv[]);

#endif
