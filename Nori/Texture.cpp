#include "Texture.h"
#include <fstream>
#include <cassert>
#include <vector>
#include "stb_image.h"

namespace nori {
    Texture::Texture(const char* fname, int rows, int cols) {
        //Check if this is a 3D texture
        assert(rows >= 1 && cols >= 1);
        isTextureArray = (rows > 1 || cols > 1);

        //Open the file
        std::string name = fname;
        if (name.substr(name.find_last_of(".") + 1) == "bmp") {
            //Open the bitmap
            std::ifstream fin(std::string("Textures/") + fname, std::ios::in | std::ios::binary);
            if (!fin) {
                texId = 0;
                return;
            }

            //Read the bitmap
            char input[54];
            fin.read(input, 54);
            const GLsizei width = *reinterpret_cast<int32_t*>(&input[18]);
            const GLsizei height = *reinterpret_cast<int32_t*>(&input[22]);
            assert(width % cols == 0);
            assert(height % rows == 0);
            const int block_w = width / cols;
            const int block_h = height / rows;
            uint8_t* img = new uint8_t[width * height * 3];
            for (int y = height; y-- > 0;) {
                const int row = y / block_h;
                const int ty = y % block_h;
                for (int x = 0; x < width; x++) {
                    const int col = x / block_w;
                    const int tx = x % block_w;
                    uint8_t* ptr = img + ((row * cols + col) * (block_w * block_h) + ty * block_w + tx) * 3;
                    fin.read(reinterpret_cast<char*>(ptr), 3);
                }
                const int padding = (width * 3) % 4;
                if (padding) {
                    char junk[3];
                    fin.read(junk, 4 - padding);
                }
            }

            //Load texture into video memory
            glGenTextures(1, &texId);
            if (isTextureArray) {
                glBindTexture(GL_TEXTURE_2D_ARRAY, texId);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glGenerateMipmap(GL_TEXTURE_2D);
                glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB8, width / rows, height / cols, rows * cols, 0, GL_BGR, GL_UNSIGNED_BYTE, img);
            }
            else {
                glBindTexture(GL_TEXTURE_2D, texId);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, img);
            }

            //Clenup
            delete[] img;
        }
        else
        {
            stbi_set_flip_vertically_on_load(1);
            int x, y, n;
            const char* path1 = "Textures/";
            std::string path2 = path1;
            path2 += fname;
            const char* path = path2.c_str();

            stbi_uc* data = stbi_load(path, &x, &y, &n, STBI_rgb_alpha);
            if (!data) {
                texId = 0;
                return;
            }

            //Load texture into video memory
            glGenTextures(1, &texId);
            if (isTextureArray) {
                glBindTexture(GL_TEXTURE_2D_ARRAY, texId);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glGenerateMipmap(GL_TEXTURE_2D);
                glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB8, x / rows, y / cols, rows * cols, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
            }
            else {
                glBindTexture(GL_TEXTURE_2D, texId);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, x, y, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
            }
            stbi_image_free(data);

            //Clenup
            delete[] data;
        }
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &texId);
    }

    void Texture::Use() {
        if (isTextureArray) {
            glBindTexture(GL_TEXTURE_2D_ARRAY, texId);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, texId);
        }
    }
};