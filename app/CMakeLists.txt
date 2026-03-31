add_executable(SolarSystem main.cpp)

target_link_libraries(SolarSystem game)

# Copy assets

add_custom_command(TARGET SolarSystem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets/shaders
    $<TARGET_FILE_DIR:SolarSystem>/assets/shaders
)
