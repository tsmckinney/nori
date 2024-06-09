#include "Input.h"
#include "GameHeader.h"
#include <SDL3/SDL.h>
#include <memory>

Input::Input() {
  memset(this, 0, sizeof(Input));
}

void Input::EndFrame() {
  memset(key_press, 0, sizeof(key_press));
  memset(mouse_button_press, 0, sizeof(mouse_button_press));
  mouse_dx = mouse_dx * GH_MOUSE_SMOOTH + mouse_ddx * (1.0f - GH_MOUSE_SMOOTH);
  mouse_dy = mouse_dy * GH_MOUSE_SMOOTH + mouse_ddy * (1.0f - GH_MOUSE_SMOOTH);
  mouse_ddx = 0.0f;
  mouse_ddy = 0.0f;
}

void Input::UpdateRaw(unsigned int state, float x, float y) {
    if (GH_HIDE_MOUSE)
    {
        mouse_dx = x;
        mouse_dy = y;
        mouse_ddx += mouse_dx;
        mouse_ddy += mouse_dy;
    }
    else {
        mouse_dx = 0;
        mouse_dy = 0;
        mouse_ddx += 0;
        mouse_ddy += 0;
    }
    if (state == SDL_BUTTON_LEFT)
    {
        mouse_button[0] = true;
        mouse_button_press[0] = true;
    }
    if (state == SDL_BUTTON_MIDDLE) {
        mouse_button[1] = true;
        mouse_button_press[1] = true;
    }
    if (state == SDL_BUTTON_RIGHT)
    {
        mouse_button[2] = true;
        mouse_button_press[2] = true;
    }
    if (state != SDL_BUTTON_LEFT)
    {
        mouse_button[0] = false;
    }
    if (state != SDL_BUTTON_MIDDLE)
    {
        mouse_button[1] = false;
    }
    if (state != SDL_BUTTON_RIGHT)
    {
        mouse_button[2] = false;
    }
}
