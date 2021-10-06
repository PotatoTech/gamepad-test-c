// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2021 PotatoTech

#include "SDL.h"

#include <stdbool.h>
#include <stdlib.h>

void load_mappings_file(void) {
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

void handle_gamepad_added(SDL_ControllerDeviceEvent e, bool show_mappings) {
    SDL_GameController* gamepad = SDL_GameControllerOpen(e.which);
    if (gamepad) {
        // The device ID is not necessarily the same as the ID reported in
        // future controller events.
        SDL_Joystick* js = SDL_GameControllerGetJoystick(gamepad);
        printf("Added gamepad %i\n", SDL_JoystickInstanceID(js));
        if (show_mappings) {
            char* mapping = SDL_GameControllerMapping(gamepad);
            if (mapping) {
                printf("mapping: %s\n", mapping);
            }
            SDL_free(mapping);
        }
    } else {
        fprintf(stderr, "Could not open device %i: %s\n", e.which, SDL_GetError());
    }
}

void handle_gamepad_removed(SDL_ControllerDeviceEvent e) {
    SDL_GameController* gamepad = SDL_GameControllerFromInstanceID(e.which);
    if (gamepad) {
        SDL_GameControllerClose(gamepad);
        printf("Removed gamepad %i\n", e.which);
    } else {
        fprintf(stderr, "Could not close handle for gamepad %i: %s\n", e.which, SDL_GetError());
    }
}

const char* get_axis_name(SDL_GameControllerAxis axis) {
    switch (axis) {
    case SDL_CONTROLLER_AXIS_LEFTX: return "LeftX";
    case SDL_CONTROLLER_AXIS_LEFTY: return "LeftY";
    case SDL_CONTROLLER_AXIS_RIGHTX: return "RightX";
    case SDL_CONTROLLER_AXIS_RIGHTY: return "RightY";
    case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return "TriggerLeft";
    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return "TriggerRight";
    default: return "Unknown";
    }
}

const char* get_button_name(SDL_GameControllerButton button) {
    switch (button) {
    case SDL_CONTROLLER_BUTTON_A: return "A";
    case SDL_CONTROLLER_BUTTON_B: return "B";
    case SDL_CONTROLLER_BUTTON_X: return "X";
    case SDL_CONTROLLER_BUTTON_Y: return "Y";
    case SDL_CONTROLLER_BUTTON_BACK: return "Back";
    case SDL_CONTROLLER_BUTTON_GUIDE: return "Guide";
    case SDL_CONTROLLER_BUTTON_START: return "Start";
    case SDL_CONTROLLER_BUTTON_LEFTSTICK: return "LeftStick";
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return "RightStick";
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return "LeftShoulder";
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return "RightShoulder";
    case SDL_CONTROLLER_BUTTON_DPAD_UP: return "DPadUp";
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return "DPadDown";
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return "DPadLeft";
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return "DPadRight";
    case SDL_CONTROLLER_BUTTON_MISC1: return "Misc1";
    case SDL_CONTROLLER_BUTTON_PADDLE1: return "Paddle1";
    case SDL_CONTROLLER_BUTTON_PADDLE2: return "Paddle2";
    case SDL_CONTROLLER_BUTTON_PADDLE3: return "Paddle3";
    case SDL_CONTROLLER_BUTTON_PADDLE4: return "Paddle4";
    case SDL_CONTROLLER_BUTTON_TOUCHPAD: return "Touchpad";
    default: return "Unknown";
    }
}

void handle_axis_motion(SDL_ControllerAxisEvent e) {
    printf("id %i: axis %s = %i\n", e.which, get_axis_name(e.axis), e.value);
}

void handle_button(SDL_ControllerButtonEvent e) {
    printf("id %i: button %s = ", e.which, get_button_name(e.button));
    switch (e.state) {
    case SDL_PRESSED: printf("down\n"); break;
    case SDL_RELEASED: printf("up\n"); break;
    }
}

int main(int argc, char* argv[]) {
    bool show_mappings = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--show-mappings") == 0) {
            show_mappings = true;
        } else {
            fprintf(stderr, "Usage: %s [--show-mappings]\n", argv[0]);
            return 1;
        }
    }

    if (SDL_Init(SDL_INIT_GAMECONTROLLER)) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);

    load_mappings_file();

    while (true) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_CONTROLLERDEVICEADDED: handle_gamepad_added(e.cdevice, show_mappings); break;
            case SDL_CONTROLLERDEVICEREMOVED: handle_gamepad_removed(e.cdevice); break;
            case SDL_CONTROLLERAXISMOTION: handle_axis_motion(e.caxis); break;
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP: handle_button(e.cbutton); break;
            case SDL_QUIT: return 0;
            }
        }
    }
}
