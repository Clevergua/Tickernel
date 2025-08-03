cmake_minimum_required(VERSION 3.26)
project(Tickernel VERSION 1.1 LANGUAGES C)

# Project configuration
# Because of this use c99: https://stackoverflow.com/questions/40829032/how-to-install-c11-compiler-on-mac-os-with-optional-string-functions-included
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Output configuration information
message(STATUS "Configuring ${PROJECT_NAME} v${PROJECT_VERSION}")


# Source and header files
file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/*.c
)

set(PUBLIC_INCLUDE_DIRS
    ${Vulkan_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/external/cglm
    ${CMAKE_CURRENT_SOURCE_DIR}/external/spirv_reflect
    # ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Libraries
set(LINK_LIBRARIES
    ${Vulkan_LIBRARIES}
)

# Print configuration
message(Sources: ${SOURCE_FILES})
message(Include directories: ${PRIVATE_INCLUDE_DIRS} ${PRIVATE_INCLUDE_DIRS})
message(Link libraries: ${LINK_LIBRARIES})


# Build shared library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PRIVATE_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${PUBLIC_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LINK_LIBRARIES})

# Testing
enable_testing()
file(GLOB TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.c)

foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} ${PROJECT_NAME})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    message(STATUS "Test added: ${TEST_NAME}")
endforeach()

message(STATUS "Configuration completed for ${PROJECT_NAME}")
