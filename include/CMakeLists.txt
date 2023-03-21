set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/glad/glad.c PROPERTIES LANGUAGE CXX)
add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# add_library(shader_s STATIC shader_s.cpp)
# target_include_directories(shader_s PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(shader STATIC shader.cpp)
target_include_directories(shader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(shader PUBLIC glad)

add_library(stb_image STATIC stb_image.cpp)
target_include_directories(stb_image PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(camera STATIC camera.cpp)
target_include_directories(camera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(camera PUBLIC glad)
