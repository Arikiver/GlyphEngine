#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);
    void use() const;

    void setInt(const std::string &name, int value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setMat4(const std::string &name, const float* value) const;  // ✅ Add this
};
