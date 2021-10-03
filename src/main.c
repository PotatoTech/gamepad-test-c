// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2021 PotatoTech

#include "SDL.h"

#include <stdbool.h>
#include <stdlib.h>

#define MAYBE_UNUSED(x) ((void) x)

int main(int argc, char* argv[]) {
    MAYBE_UNUSED(argc);
    MAYBE_UNUSED(argv);

    if (SDL_Init(SDL_INIT_GAMECONTROLLER)) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);

    {
        const char* controller_config_env_var = "SDL_GAMECONTROLLERCONFIG";
        char* mapping = getenv(controller_config_env_var);
        if (mapping) {
            if (SDL_GameControllerAddMapping(mapping) < 0) {
                fprintf(
                    stderr,
                    "Failed to load %s: %s\n",
                    controller_config_env_var,
                    SDL_GetError());
            }
        }

        char* base_path = SDL_GetBasePath();
        const char* mappings_file = "gamecontrollerdb.txt";
        char* path_to_mappings = malloc(strlen(base_path) + strlen(mappings_file) + 1);
        strcpy(path_to_mappings, base_path);
        strcat(path_to_mappings, mappings_file);
        if (SDL_GameControllerAddMappingsFromFile(path_to_mappings) < 0) {
            fprintf(stderr, "Failed to load mappings file: %s\n", SDL_GetError());
        }
        free(path_to_mappings);
        SDL_free(base_path);
    }

    while (true) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_CONTROLLERDEVICEADDED: {
                SDL_GameController* gamepad = SDL_GameControllerOpen(e.cdevice.which);
                if (gamepad) {
                    // The device ID is not necessarily the same as the ID reported in
                    // future controller events.
                    SDL_Joystick* js = SDL_GameControllerGetJoystick(gamepad);
                    printf("Added gamepad %i\n", SDL_JoystickInstanceID(js));
                } else {
                    fprintf(
                        stderr,
                        "Could not open device %i: %s\n",
                        e.cdevice.which,
                        SDL_GetError());
                }
                break;
            }
            case SDL_CONTROLLERDEVICEREMOVED: {
                SDL_GameController* gamepad = SDL_GameControllerFromInstanceID(e.cdevice.which);
                if (gamepad) {
                    SDL_GameControllerClose(gamepad);
                    printf("Removed gamepad %i\n", e.cdevice.which);
                } else {
                    fprintf(
                        stderr,
                        "Could not close handle for gamepad %i: %s\n",
                        e.cdevice.which,
                        SDL_GetError());
                }
                break;
            }
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
