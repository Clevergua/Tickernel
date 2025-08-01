cmake_minimum_required(VERSION 3.26)
project(TickernelLua VERSION 1.1 LANGUAGES C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source files
file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

# Add dependency on TickernelGfx
if(TARGET TickernelGfx)
    # Link to graphics library
    target_link_libraries(${PROJECT_NAME} PUBLIC TickernelGfx)
    
    # Add graphics library include paths
    target_include_directories(${PROJECT_NAME} PUBLIC 
        ${CMAKE_SOURCE_DIR}/gfx/src
        ${CMAKE_SOURCE_DIR}/gfx/include
    )
    
    # Define macro for graphics support
    target_compile_definitions(${PROJECT_NAME} PRIVATE TICKERNEL_GFX_ENABLED)
    
    message(STATUS "TickernelLua linked to TickernelGfx")
else()
    message(WARNING "TickernelGfx target not found")
endif()

message(STATUS "TickernelLua configuration completed")
