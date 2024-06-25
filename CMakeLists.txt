cmake_minimum_required(VERSION 3.21)
project(Tickernel VERSION 1.0 LANGUAGES C)
set(C_STANDARD 11)
set(C_STANDARD_REQUIRED True)
# Debug or Release
message("${CMAKE_BUILD_TYPE}")

find_package(Vulkan REQUIRED)

find_library(GLFW3_LIBRARY NAMES "libglfw.3.dylib" "glfw3.lib" PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw3)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

set(HEADER_DIRS
 ${Vulkan_INCLUDE_DIRS}
 ${CMAKE_CURRENT_SOURCE_DIR}/include/cglm
 ${CMAKE_CURRENT_SOURCE_DIR}/include/glfw3
 ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(LIBRARYS 
 ${Vulkan_LIBRARIES}
 ${GLFW3_LIBRARY}
 ${CGLM_LIBRARY})

add_executable(${PROJECT_NAME} ${SOURCES})
message("   HEADER_DIRS:${HEADER_DIRS}")
target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_DIRS})
message("   LIBRARYS:${LIBRARYS}")
target_link_libraries(${PROJECT_NAME} ${LIBRARYS})