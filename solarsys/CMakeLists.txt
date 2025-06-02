cmake_minimum_required(VERSION 3.14)
project(MiniSolarSystem)
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# GLAD
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v0.1.36
)
FetchContent_MakeAvailable(glad)

add_executable(MiniSolarSystem main.cpp)

# GLAD source
# add_subdirectory(${glad_SOURCE_DIR}/cmake ${glad_BINARY_DIR}/cmake-build)
target_link_libraries(MiniSolarSystem PRIVATE glfw glad)
target_include_directories(MiniSolarSystem PRIVATE ${glad_SOURCE_DIR}/include)

if (WIN32)
    target_link_libraries(MiniSolarSystem PRIVATE opengl32)
endif() 