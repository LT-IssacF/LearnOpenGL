cmake_minimum_required(VERSION 3.10)

project(LearnOpenGL VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DNOMINMAX)

option(GLFW "glfw" ON)
option(GLAD "glad" ON)
option(GLM "glm" ON)

add_subdirectory(include)

message(STATUS "glfw: " ${GLFW})
message(STATUS "glad: " ${GLAD})
message(STATUS "glm: " ${GLM})

set(01 src/01_hello_window/main.cpp)
set(02 src/02_hello_triangle/main.cpp)
set(03 src/03_shaders/main.cpp)
set(04 src/04_textures/main.cpp)
set(05 src/05_transformations/main.cpp)
set(06 src/06_coordinates_system/main.cpp)
set(07 src/07_camera/main.cpp)

add_executable(main ${07})

target_include_directories(main PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(main PUBLIC glad)
# 链接第三方库时需使用全名
target_link_libraries(main PUBLIC ${CMAKE_SOURCE_DIR}/lib/libglfw3.a shader_s stb_image camera)
