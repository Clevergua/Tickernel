cmake_minimum_required(VERSION 3.26)
project(TickernelLua VERSION 1.1 LANGUAGES C)
set(CMAKE_C_STANDARD 99)

add_definitions(-DLUA_USE_C89)

file(GLOB SRC_FILES "src/*.c")
list(REMOVE_ITEM SRC_FILES "${CMAKE_SOURCE_DIR}/src/lua.c" "${CMAKE_SOURCE_DIR}/src/luac.c")

add_library(lua SHARED ${SRC_FILES})
set_target_properties(lua PROPERTIES OUTPUT_NAME "lua")
