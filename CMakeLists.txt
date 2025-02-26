cmake_minimum_required(VERSION 3.26)
project(Tickernel VERSION 1.1 LANGUAGES C)
set(CMAKE_C_STANDARD_REQUIRED ON)
# Because of this use c99: https://stackoverflow.com/questions/40829032/how-to-install-c11-compiler-on-mac-os-with-optional-string-functions-included
set(CMAKE_C_STANDARD 99)
message(${CMAKE_BUILD_TYPE})
find_package(Vulkan REQUIRED)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
set(HEADER_DIRS
    ${Vulkan_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cglm
    ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIBRARYS
    ${Vulkan_LIBRARIES}
    ${CGLM_LIBRARY}
)

message("   SOURCES:${SOURCES}")
add_library(${PROJECT_NAME} SHARED ${SOURCES})
message("   HEADER_DIRS:${HEADER_DIRS}")
target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_DIRS})
message("   LIBRARYS:${LIBRARYS}")
target_link_libraries(${PROJECT_NAME} ${LIBRARYS})

# Compile shaders
find_program(GLSLC_EXECUTABLE glslc)
set(SHADER_SOURCES_DIR ${CMAKE_SOURCE_DIR}/res/glsl)
set(SHADER_SOURCES)
file(GLOB_RECURSE SHADER_SOURCES ${SHADER_SOURCES_DIR}/*.vert ${SHADER_SOURCES_DIR}/*.frag)

set(SHADER_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/assets/shaders)
foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SPIRV ${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv)
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${GLSLC_EXECUTABLE} ${SHADER} -o ${SPIRV}
        DEPENDS ${SHADER}
        COMMENT "Compiling ${SHADER} to SPIR-V"
    )
    list(APPEND SPIRV_BINARIES ${SPIRV})
endforeach()
add_custom_target(CompileShaders DEPENDS ${SPIRV_BINARIES})
add_dependencies(${PROJECT_NAME} CompileShaders)
# # Copy assets
# add_custom_target(CopyAssets
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     ${CMAKE_SOURCE_DIR}/assets
#     ${CMAKE_BINARY_DIR}/assets
# )
# add_dependencies(${PROJECT_NAME} CopyAssets)

