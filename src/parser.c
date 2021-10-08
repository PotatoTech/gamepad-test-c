// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2021 PotatoTech

#include "parser.h"

#include "argparse.h"

#include <stdlib.h>

Options parse_arguments(int argc, const char* argv[]) {
    Options args = {
        NULL,
        0,
        false,
    };
    int32_t deadzone = 0;

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_STRING(
            'm',
            "mappings",
            &args.mappings,
            "load the mappings from the specified file",
            NULL,
            0,
            0),
        OPT_INTEGER(0, "deadzone", &deadzone, "set the deadzone for gamepad axes", NULL, 0, 0),
        OPT_BOOLEAN(
            'v',
            "verbose",
            &args.verbose,
            "show the mapping used by each controller when it is added",
            NULL,
            0,
            0),
        OPT_END(),
    };

    const char* const usage[] = {
        "gamepad-test [options]",
        NULL,
    };

    struct argparse parser;
    argparse_init(&parser, options, usage, 0);
    argparse_parse(&parser, argc, argv);

    if (deadzone < INT16_MIN + 1 || deadzone > INT16_MAX) {
        args.deadzone = INT16_MAX + 1;
    } else {
        args.deadzone = (uint16_t) abs(deadzone);
    }

    return args;
}
