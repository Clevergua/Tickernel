cmake_minimum_required(VERSION 3.26)
project(TickernelLua VERSION 1.1 LANGUAGES C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/*.c
)

set(PUBLIC_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# set(PRIVATE_INCLUDE_DIRS
#     ${CMAKE_CURRENT_SOURCE_DIR}/external/xxx
# )

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC Tickernel)
target_include_directories(${PROJECT_NAME} PUBLIC ${PUBLIC_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${PRIVATE_INCLUDE_DIRS})
