#pragma once

struct tagRAWINPUT;
class Input {
public:
  Input();

  void EndFrame();
  void UpdateRaw(unsigned int state, float x, float y);

  //Keyboard
  bool key[2048];
  bool key_press[2048];

  //Mouse
  bool mouse_button[3];
  bool mouse_button_press[3];
  float mouse_dx;
  float mouse_dy;
  float mouse_ddx;
  float mouse_ddy;

  //Joystick
  //TODO:

  //Bindings
  //TODO:

  //Calibration
  //TODO:
};
