cmake_minimum_required(VERSION 3.26)
project(TickernelLua VERSION 1.1 LANGUAGES C)
set(CMAKE_C_STANDARD 99)

add_definitions(-DLUA_USE_C89)

file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_library(${PROJECT_NAME}  SHARED ${SOURCE_FILES})
