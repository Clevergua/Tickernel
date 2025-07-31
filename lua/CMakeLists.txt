cmake_minimum_required(VERSION 3.26)
project(TickernelLua VERSION 1.1 LANGUAGES C)
set(CMAKE_C_STANDARD 90)

add_definitions(-DLUA_USE_C89 -DLUA_USE_POSIX)

file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

# # === 添加对 TickernelEngine 的依赖 ===
# if(TARGET TickernelGfx)
#     # 链接引擎库
#     target_link_libraries(${PROJECT_NAME} PUBLIC TickernelEngine)
    
#     # 添加引擎的头文件路径
#     target_include_directories(${PROJECT_NAME} PUBLIC 
#         ${CMAKE_SOURCE_DIR}/engine/src
#         ${CMAKE_SOURCE_DIR}/engine/include
#     )
    
#     # 定义宏，让 C 代码知道有图形支持
#     target_compile_definitions(${PROJECT_NAME} PRIVATE TICKERNEL_GFX_ENABLED)
    
#     message(STATUS "TickernelLua will link with TickernelEngine (graphics support enabled)")
# else()
#     message(WARNING "TickernelEngine not found - TickernelLua will build without graphics support")
# endif()
