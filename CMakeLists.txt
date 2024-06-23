cmake_minimum_required(VERSION 3.21)
project(Tickernel VERSION 1.0 LANGUAGES C)
set(C_STANDARD 11)
set(C_STANDARD_REQUIRED True)
find_package(Vulkan REQUIRED)

FILE(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
set(INCLUDE_DIRECTORIES  ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRECTORIES})
target_link_libraries(${PROJECT_NAME} ${LINK_LIBRARIES})
# message(!!${LINK_LIBRARIES})