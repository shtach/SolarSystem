cmake_minimum_required(VERSION 3.15)
project(SolarSystemSimulation VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type (Debug/Release)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Project structure
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

# GLFW - check if we need to use subdirectory or find package
if(EXISTS ${CMAKE_SOURCE_DIR}/GLFW/CMakeLists.txt)
    add_subdirectory(GLFW)
    set(GLFW_LIBRARY glfw)
else()
    find_package(glfw3 REQUIRED)
    set(GLFW_LIBRARY glfw)
endif()

# GLAD configuration
if(EXISTS ${CMAKE_SOURCE_DIR}/GLRW/src/glad.c)
    # Add GLAD as a library
    add_library(glad ${CMAKE_SOURCE_DIR}/GLRW/src/glad.c)
    target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/GLRW/include)
else()
    message(FATAL_ERROR "GLAD not found at GLRW/src/glad.c")
endif()

# Check for GLM
if(EXISTS ${CMAKE_SOURCE_DIR}/glm)
    include_directories(${CMAKE_SOURCE_DIR}/glm)
endif()

# Source files
set(SOURCE_FILES
    Application.cpp
    main.cpp
    Physics/Body.cpp
    Physics/Simulation.cpp
    Rendering/InputHandler.cpp
    Rendering/Renderer.cpp
)

# Create executable - TYLKO pliki źródłowe
add_executable(SolarSystemSimulation
    ${SOURCE_FILES}
)

# Target properties
set_target_properties(SolarSystemSimulation PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Include directories for the target
target_include_directories(SolarSystemSimulation PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Math
    ${CMAKE_SOURCE_DIR}/Physics
    ${CMAKE_SOURCE_DIR}/Rendering
    ${CMAKE_SOURCE_DIR}/GLRW/include
)

# Link libraries
target_link_libraries(SolarSystemSimulation
    glad
    OpenGL::GL
    ${GLFW_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

# Platform-specific configurations
if(WIN32)
    target_link_libraries(SolarSystemSimulation opengl32 gdi32)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(CORE_VIDEO_LIBRARY CoreVideo)
    target_link_libraries(SolarSystemSimulation 
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${CORE_VIDEO_LIBRARY}
    )
else()  # Linux/Unix
    find_package(X11 REQUIRED)
    target_link_libraries(SolarSystemSimulation
        ${X11_X11_LIB}
        ${X11_Xrandr_LIB}
        ${X11_Xinerama_LIB}
        ${X11_Xcursor_LIB}
        ${X11_Xxf86vm_LIB}
        ${X11_Xi_LIB}
    )
endif()

# Copy assets and shaders if they exist
if(EXISTS ${CMAKE_SOURCE_DIR}/assets)
    add_custom_command(TARGET SolarSystemSimulation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:SolarSystemSimulation>/assets
    )
endif()


if(EXISTS ${CMAKE_SOURCE_DIR}/shaders)
    add_custom_command(TARGET SolarSystemSimulation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:SolarSystemSimulation>/shaders
    )
endif()