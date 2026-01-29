cmake_minimum_required(VERSION 4.0)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 26)

add_executable(LearnOpenGL WIN32
        main.cpp
        third_party/glad/src/glad.c
)

target_link_options(LearnOpenGL PRIVATE
        -Wl,/SUBSYSTEM:WINDOWS
        -Wl,/ENTRY:mainCRTStartup
)

target_include_directories(LearnOpenGL PRIVATE
        third_party/glad/include
        third_party/glfw/include
        third_party/glm
)

target_link_directories(LearnOpenGL PRIVATE
        third_party/glfw/lib
)

target_link_libraries(LearnOpenGL PRIVATE
        glfw3
        opengl32
)
