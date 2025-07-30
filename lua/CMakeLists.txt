cmake_minimum_required(VERSION 3.26)
project(TickernelLua VERSION 1.1 LANGUAGES C)

# Project configuration
# Because of this use c99: https://stackoverflow.com/questions/40829032/how-to-install-c11-compiler-on-mac-os-with-optional-string-functions-included
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Output configuration information
message(STATUS "Configuring ${PROJECT_NAME} v${PROJECT_VERSION}")

# Dependencies
find_library(LUA_LIBRARY 
    NAMES lua lua53 lua5.3
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib/lua
    REQUIRED
)

# Source and header files
file(GLOB_RECURSE SOURCE_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.c
)

set(INCLUDE_DIRS
    ${Vulkan_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lua
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Libraries
set(LINK_LIBRARIES
    ${LUA_LIBRARY}
)

# Print configuration
message(STATUS "Sources: ${SOURCE_FILES}")
message(STATUS "Include directories: ${INCLUDE_DIRS}")
message(STATUS "Link libraries: ${LINK_LIBRARIES}")

# Build shared library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LINK_LIBRARIES})

## Copy DLLs
file(GLOB_RECURSE DLL_FILES ${CMAKE_SOURCE_DIR}/lib/*.dylib ${CMAKE_SOURCE_DIR}/lib/*.dll)
if(DLL_FILES)
    add_custom_target(CopyDLL 
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLL_FILES} ${CMAKE_BINARY_DIR}
        COMMENT "Copying DLLs to ${CMAKE_BINARY_DIR}"
    )
    add_dependencies(${PROJECT_NAME} CopyDLL)
else()
    message(STATUS "No DLL files found to copy")
endif()


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