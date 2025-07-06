#pragma once
#include <string>

class Texture {
public:
    unsigned int ID;
    int width, height, channels;

    Texture(const std::string& path);
    void bind() const;
};
