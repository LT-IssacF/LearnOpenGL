cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

set(TARGET_NAME LearnOpenGL)

project(${TARGET_NAME})

set(01 src/01_hello_window)
set(02 src/02_hello_triangle)
set(03 src/03_shaders)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)

# set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

# 头文件路径
include_directories(include)

# 库文件路径
link_directories(lib)

# 添加需要链接的库文件
link_libraries(glad)
link_libraries(glfw3)
link_libraries(glfw3dll)

message(STATUS "This is binary dir " ${${TARGET_NAME}_BINARY_DIR})
message(STATUS "This is source dir " ${${TARGET_NAME}_SOURCE_DIR})

# add_executable(main ${01}/main.cpp)
# add_executable(main ${02}/main.cpp)
add_executable(main ${03}/main.cpp)

# 链接库到.exe上
target_link_libraries(main glad)
# target_link_libraries(main glfw3)
target_link_libraries(main -lglfw3dll)
