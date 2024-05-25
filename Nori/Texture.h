#pragma once
#include <glad/gl.h>

class Texture {
public:
  Texture(const char* fname, int rows, int cols);
  ~Texture();

  void Use();

private:
  GLuint texId;
  bool is3D;
};
