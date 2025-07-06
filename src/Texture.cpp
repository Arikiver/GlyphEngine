#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Texture.h"
#include <glad/glad.h>
#include <iostream>

Texture::Texture(const std::string& path) {
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return;
    }

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  

    stbi_image_free(data);
}

void Texture::bind() const {
    glBindTexture(GL_TEXTURE_2D, ID);
}
