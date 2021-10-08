// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2021 PotatoTech

#ifndef GAMEPAD_H
#define GAMEPAD_H

#include "SDL.h"

#include <stdbool.h>
#include <stdint.h>

void handle_gamepad_added(SDL_ControllerDeviceEvent e, bool show_mappings);
void handle_gamepad_removed(SDL_ControllerDeviceEvent e);
const char* get_axis_name(SDL_GameControllerAxis axis);
const char* get_button_name(SDL_GameControllerButton button);
void handle_axis_motion(SDL_ControllerAxisEvent e, int16_t deadzone);
void handle_button(SDL_ControllerButtonEvent e);
void load_mappings(const char* path_to_mappings);

#endif
