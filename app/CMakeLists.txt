add_executable(SolarSystem main.cpp)

target_link_libraries(SolarSystem game)

# Copy assets
if(EXISTS ${CMAKE_SOURCE_DIR}/shaders)
    add_custom_command(TARGET SolarSystem POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:SolarSystem>/shaders
    )
endif()