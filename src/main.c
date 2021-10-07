// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2021 PotatoTech

#include "SDL.h"
#include "argparse.h"
#include "gamepad.h"

#include <stdbool.h>
#include <stdlib.h>

int main(int argc, const char* argv[]) {
    bool verbose = false;
    const char* mappings = NULL;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_STRING(
            'm',
            "mappings",
            &mappings,
            "load the mappings from the specified file",
            NULL,
            0,
            0),
        OPT_BOOLEAN(
            'v',
            "verbose",
            &verbose,
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
    argc = argparse_parse(&parser, argc, argv);

    if (SDL_Init(SDL_INIT_GAMECONTROLLER)) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);

    if (mappings) {
        load_mappings(mappings);
    }

    while (true) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_CONTROLLERDEVICEADDED: handle_gamepad_added(e.cdevice, verbose); break;
            case SDL_CONTROLLERDEVICEREMOVED: handle_gamepad_removed(e.cdevice); break;
            case SDL_CONTROLLERAXISMOTION: handle_axis_motion(e.caxis); break;
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP: handle_button(e.cbutton); break;
            case SDL_QUIT: return 0;
            }
        }
    }
}
