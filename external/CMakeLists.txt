# GLFW
add_subdirectory(glfw)

# GLAD
add_library(glad glad/src/gl.c)
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/include
)