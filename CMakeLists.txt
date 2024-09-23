cmake_minimum_required(VERSION 3.26)
# set(CMAKE_SYSROOT ON)
project(Tickernel VERSION 1.1 LANGUAGES C)
set(CMAKE_C_STANDARD_REQUIRED ON)
# Because of this use c99: https://stackoverflow.com/questions/40829032/how-to-install-c11-compiler-on-mac-os-with-optional-string-functions-included
set(CMAKE_C_STANDARD 99)
# Debug or Release
message(${CMAKE_BUILD_TYPE})

find_package(Vulkan REQUIRED)
find_library(GLFW3_LIBRARY NAMES "libglfw3.a" "libglfw.3.dylib" "glfw3.dll" "glfw3dll.lib" PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw3)
find_library(LUA_LIBRARY NAMES "liblua.dylib" "lualib53.dylib" "liblua.dll" "liblua.lib" PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib/lua)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

set(HEADER_DIRS
 ${Vulkan_INCLUDE_DIRS}
 ${CMAKE_CURRENT_SOURCE_DIR}/include/cglm
 ${CMAKE_SOURCE_DIR}/include/lua
 ${CMAKE_CURRENT_SOURCE_DIR}/include/glfw3
 ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(LIBRARYS 
 ${Vulkan_LIBRARIES}
 ${GLFW3_LIBRARY}
 ${LUA_LIBRARY}
 ${CGLM_LIBRARY}
)
if(WIN32)
    # Windows paltform
    # Collect library
    add_definitions(-D_CRT_NO_MICROSOFT_JEDI_MIND_CONTROL=1 -D_CRT_SECURE_NO_WARNINGS=1)
elseif(APPLE)
    # Apple paltform
    # Collect library
    # list(APPEND LIBRARYS  "-framework Cocoa" "-framework IOKit")
else()
# Ohter platform
    message("Unknown platform!")
endif()


add_executable(${PROJECT_NAME} ${SOURCES})
message("   HEADER_DIRS:${HEADER_DIRS}")
target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_DIRS})
message("   LIBRARYS:${LIBRARYS}")
target_link_libraries(${PROJECT_NAME} ${LIBRARYS})

set(DLL_LIBS)
FILE(GLOB_RECURSE DLL_LIBS ${CMAKE_SOURCE_DIR}/lib/*.dylib ${CMAKE_SOURCE_DIR}/lib/*.dll)
message(DLL_LIBS: ${DLL_LIBS})

add_custom_target(CopyAssets COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_BINARY_DIR}/assets)
add_dependencies(${PROJECT_NAME}  CopyAssets)
add_custom_target(CopyDLL COMMAND ${CMAKE_COMMAND} -E copy_if_different  ${DLL_LIBS}  ${CMAKE_BINARY_DIR}/${DLL_FILENAME})
add_dependencies(${PROJECT_NAME}  CopyDLL)