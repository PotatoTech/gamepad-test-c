// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2021 PotatoTech

#include "SDL.h"

#include <stdbool.h>
#include <stdlib.h>

int main(void) {
    if (SDL_Init(SDL_INIT_GAMECONTROLLER)) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);

    int num_joysticks = SDL_NumJoysticks();
    if (num_joysticks < 0) {
        fprintf(stderr, "Could not detect number of joysticks: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GameController** gamepads = malloc(sizeof gamepads * (size_t) num_joysticks);
    if (!gamepads) {
        fprintf(stderr, "Could not allocate memory for game controller handles\n");
        return 1;
    }

    // Gamepads which are already connected when the program starts will not produce
    // the SDL_CONTROLLERDEVICEADDED event, so we must iterate over them.
    for (int i = 0; i < num_joysticks; ++i) {
        gamepads[i] = SDL_GameControllerOpen(i);
        if (gamepads[i]) {
            printf("Opened game controller %i\n", i);
        } else {
            fprintf(stderr, "Could not open game controller %i: %s\n", i, SDL_GetError());
        }
    }

    while (true) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_CONTROLLERAXISMOTION:
                printf("id %i: axis %u = %i\n", e.caxis.which, e.caxis.axis, e.caxis.value);
                break;
            case SDL_CONTROLLERBUTTONDOWN:
                printf("id %i: button %u = down\n", e.cbutton.which, e.cbutton.button);
                break;
            case SDL_CONTROLLERBUTTONUP:
                printf("id %i: button %u = up\n", e.cbutton.which, e.cbutton.button);
                break;
            case SDL_QUIT: return 0;
            }
        }
    }
}
