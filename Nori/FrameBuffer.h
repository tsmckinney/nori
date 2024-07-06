#pragma once
#include "Camera.h"
#include <glad/gl.h>

namespace nori {
    //Forward declaration
    class Portal;

    class FrameBuffer {
    public:
        FrameBuffer();
        ~FrameBuffer();

        void Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal);
        void Use();

    private:
        GLuint texId;
        GLuint fbo;
        GLuint renderBuf;
    };
};