#pragma once
#include "NoriMain.h"

class Pillar : public Object {
public:
  Pillar() {
    mesh = AquireMesh("pillar.obj");
    shader = AquireShader("texture");
    texture = AquireTexture("white.bmp");
    scale = Vector3(0.1f);
  }
  virtual ~Pillar() {}
};
