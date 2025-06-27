cmake_minimum_required(VERSION 3.10)
project(GlyphEngine)

set(CMAKE_CXX_STANDARD 17)

# Add GLAD + GLFW
add_subdirectory(external/glfw)
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/external/glfw/include
)


# Add sources
file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/*.c
)

add_executable(GlyphEngine ${SOURCES})

# Link GLFW
target_link_libraries(GlyphEngine glfw)
# Link OpenGL
if (WIN32)
    target_link_libraries(GlyphEngine opengl32)
elseif(APPLE)
    target_link_libraries(GlyphEngine "-framework OpenGL")
else()
    target_link_libraries(GlyphEngine GL)
endif()
