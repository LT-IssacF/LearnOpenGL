cmake_minimum_required(VERSION 3.10)
project(LearnOpenGL VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
add_definitions(-DNOMINMAX)

option(GLFW "glfw" ON)
option(GLAD "glad" ON)
option(GLM "glm" ON)
message(STATUS "glfw: " ${GLFW})
message(STATUS "glad: " ${GLAD})
message(STATUS "glm: " ${GLM})

add_subdirectory(include)

set(01 src/01_hello_window/main.cpp)
set(02 src/02_hello_triangle/main.cpp)
set(03 src/03_shaders/main.cpp)
set(04 src/04_textures/main.cpp)
set(05 src/05_transformations/main.cpp)
set(06 src/06_coordinates_system/main.cpp)
set(07 src/07_camera/main.cpp)
set(11 src/11_colors/main.cpp)
set(12 src/12_basic_lighting/main.cpp)
set(13 src/13_materials/main.cpp)
set(14 src/14_lighting_maps/main.cpp)
set(15 src/15_multi_lights/main.cpp)
set(21 src/21_assimp/main.cpp)
set(31 src/31_stencil_test/main.cpp)
set(32 src/32_blending/main.cpp)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_INSTALL_RPATH ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

add_executable(main ${breakout})

target_include_directories(main PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(main PUBLIC glad)
# 链接第三方库时需使用全名
target_link_libraries(main PUBLIC ${CMAKE_SOURCE_DIR}/lib/libglfw3.a)
# target_link_libraries(main PUBLIC ${CMAKE_SOURCE_DIR}/output/libassimp.dll)
# 1 ~ 15
# target_link_libraries(main PUBLIC shader_s stb_image camera)
# 21
# target_link_libraries(main PUBLIC shader stb_image camera model)
# 31 <=
# target_link_libraries(main PUBLIC shader stb_image camera)
