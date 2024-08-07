#pragma once
#include <glad/gl.h>
#include <string>
#include <vector>
namespace nori {

    class Shader {
    public:
        Shader(const char* name);
        ~Shader();

        void Use();
        void SetMVP(const float* mvp, const float* mv);

    private:
        GLuint LoadShader(const char* fname, GLenum type);

        std::vector<std::string> attribs;
        GLuint vertId;
        GLuint fragId;
        GLuint progId;
        GLuint mvpId;
        GLuint mvId;
    };
};