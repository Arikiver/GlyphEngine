#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include "Shader.h"
#include "Texture.h"
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Settings
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

// Vertices for a quad (2D)
float quadVertices[] = {
    // positions   // tex coords
     0.0f, 1.0f,   0.0f, 1.0f,
     1.0f, 1.0f,   1.0f, 1.0f,
     1.0f, 0.0f,   1.0f, 0.0f,
     0.0f, 0.0f,   0.0f, 0.0f
};

unsigned int quadIndices[] = {
    0, 1, 2,
    2, 3, 0
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Load image and compute brightness
std::vector<unsigned char> loadBrightnessMap(const char* path, int& width, int& height) {
    int channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 3);
    if (!data) {
        std::cerr << "Failed to load image\n";
        exit(1);
    }

    std::vector<unsigned char> brightness(width * height);
    for (int i = 0; i < width * height; ++i) {
        int r = data[i * 3 + 0];
        int g = data[i * 3 + 1];
        int b = data[i * 3 + 2];
        brightness[i] = static_cast<unsigned char>((r + g + b) / 3);
    }

    stbi_image_free(data);
    return brightness;
}

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GlyphEngine ASCII Test", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // Load test image
    int imgW, imgH;
    auto brightnessMap = loadBrightnessMap("assets/test_image.jpg", imgW, imgH);
    std::cout << "Image loaded: " << imgW << "x" << imgH << "\n";

    // Load ASCII font
    Texture asciiFont("assets/fonts/ascii_font.png");

    // Load shaders
    Shader asciiShader("shaders/ascii.vs", "shaders/ascii.fs");
    asciiShader.use();
    asciiShader.setInt("fontAtlas", 0);

    // Setup quad VAO/VBO/EBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    // Positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // TexCoords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Projection matrix for 2D screen space
    glm::mat4 projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, 0.0f, (float)SCREEN_HEIGHT);
    asciiShader.setMat4("projection", glm::value_ptr(projection));

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        asciiShader.use();
        asciiFont.bind();

        int glyphCols = SCREEN_WIDTH / 8;
        int glyphRows = SCREEN_HEIGHT / 12;

        int imgStepX = imgW / glyphCols;
        int imgStepY = imgH / glyphRows;

        for (int row = 0; row < glyphRows; ++row) {
            for (int col = 0; col < glyphCols; ++col) {
                int px = col * imgStepX;
                int py = row * imgStepY;

                if (px >= imgW || py >= imgH) continue;

                int imgIndex = py * imgW + px;
                unsigned char brightness = brightnessMap[imgIndex];

                // Map brightness to a glyph (0 = white, 255 = dark)
                int charIndex = 255 - brightness;

                int atlasCol = charIndex % 16;
                int atlasRow = charIndex / 16;
                float glyphSize = 1.0f / 16.0f;

                glm::vec2 glyphOffset(atlasCol * glyphSize, atlasRow * glyphSize);
                asciiShader.setVec2("glyphOffset", glyphOffset);

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(col * 8.0f, SCREEN_HEIGHT - row * 12.0f - 12.0f, 0.0f));
                model = glm::scale(model, glm::vec3(8.0f, 12.0f, 1.0f));
                asciiShader.setMat4("model", glm::value_ptr(model));

                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
