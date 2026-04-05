add_library(engine)

target_sources(engine PRIVATE
    src/graphics/Camera.cpp
    src/graphics/Renderer.cpp
    src/graphics/Shader.cpp
    src/input/InputHandler.cpp
    src/physics/Body.cpp
    src/physics/QuadTree.cpp
    src/physics/Simulation.cpp
    src/core/Logger.cpp
)

target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/stb
)

target_link_libraries(engine PUBLIC
    glad
    glfw
)

find_package(OpenGL REQUIRED)

if(TARGET OpenGL::GL)
    set(OPENGL_LIB OpenGL::GL)
elseif(OPENGL_gl_LIBRARY)
    set(OPENGL_LIB ${OPENGL_gl_LIBRARY})
else()
    message(FATAL_ERROR "OpenGL library not found")
endif()

target_link_libraries(engine PUBLIC ${OPENGL_LIB})