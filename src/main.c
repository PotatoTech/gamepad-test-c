// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2021 PotatoTech

#include "SDL.h"
#include "gamepad.h"
#include "parser.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, const char* argv[]) {
    Options args = parse_arguments(argc, argv);

    if (SDL_Init(SDL_INIT_GAMECONTROLLER)) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);

    if (args.verbose) {
        printf("Deadzone: %u\n", args.deadzone);
    }

    if (args.mappings) {
        load_mappings(args.mappings);
    }

    while (true) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_CONTROLLERDEVICEADDED: handle_gamepad_added(e.cdevice, args.verbose); break;
            case SDL_CONTROLLERDEVICEREMOVED: handle_gamepad_removed(e.cdevice); break;
            case SDL_CONTROLLERAXISMOTION: handle_axis_motion(e.caxis, args.deadzone); break;
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP: handle_button(e.cbutton); break;
            case SDL_QUIT: return 0;
            }
        }
    }
}
