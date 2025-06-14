cmake_minimum_required(VERSION 3.26)
project(Tickernel VERSION 1.1 LANGUAGES C)
set(CMAKE_C_STANDARD_REQUIRED ON)
# Because of this use c99: https://stackoverflow.com/questions/40829032/how-to-install-c11-compiler-on-mac-os-with-optional-string-functions-included
set(CMAKE_C_STANDARD 99)

# message(${CMAKE_BUILD_TYPE})
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

find_library(LUA_LIBRARY NAMES "liblua.dylib" "lualib53.dylib" "liblua.dll" "liblua.lib" PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib/lua)
set(HEADER_DIRS
    ${Vulkan_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cglm
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lua
    ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIBRARYS
    ${Vulkan_LIBRARIES}
    ${LUA_LIBRARY}
)

message("   PROJECT_NAME:${PROJECT_NAME}")
add_definitions(-DSPIRV_REFLECT_USE_SYSTEM_SPIRV_H)
message("   SOURCES:${SOURCES}")
add_library(${PROJECT_NAME} SHARED ${SOURCES})
message("   HEADER_DIRS:${HEADER_DIRS}")
target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_DIRS})
message("   LIBRARYS:${LIBRARYS}")
target_link_libraries(${PROJECT_NAME} ${LIBRARYS})

# # Copy libs
# set(DLL_LIBS)
# file(GLOB_RECURSE DLL_LIBS ${CMAKE_SOURCE_DIR}/lib/*.dylib ${CMAKE_SOURCE_DIR}/lib/*.dll)
# message(DLL_LIBS: ${DLL_LIBS})
# add_custom_target(CopyDLL COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLL_LIBS} ${CMAKE_BINARY_DIR}/${DLL_FILENAME})
# add_dependencies(${PROJECT_NAME} CopyDLL)


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

# ASTC Texture Compression
find_program(ASTCENC_EXECUTABLE astcenc)
if(NOT ASTCENC_EXECUTABLE)
    message(WARNING "astcenc not found - ASTC texture compression will be disabled")
else()
    set(TEXTURE_SOURCES_DIR ${CMAKE_SOURCE_DIR}/res/textures)
    set(TEXTURE_SOURCES)
    file(GLOB_RECURSE TEXTURE_SOURCES 
        ${TEXTURE_SOURCES_DIR}/*.png
        ${TEXTURE_SOURCES_DIR}/*.jpg
        ${TEXTURE_SOURCES_DIR}/*.jpeg
        ${TEXTURE_SOURCES_DIR}/*.tga
        ${TEXTURE_SOURCES_DIR}/*.bmp
    )

    set(TEXTURE_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/assets/textures)
    file(MAKE_DIRECTORY ${TEXTURE_OUTPUT_DIR})

    foreach(TEXTURE ${TEXTURE_SOURCES})
        get_filename_component(TEXTURE_NAME_WE ${TEXTURE} NAME_WE)
        get_filename_component(TEXTURE_DIR ${TEXTURE} DIRECTORY)
        string(REPLACE "${TEXTURE_SOURCES_DIR}" "" RELATIVE_DIR ${TEXTURE_DIR})
        set(OUTPUT_DIR "${TEXTURE_OUTPUT_DIR}${RELATIVE_DIR}")
        file(MAKE_DIRECTORY ${OUTPUT_DIR})
        
        set(ASTC_OUTPUT "${OUTPUT_DIR}/${TEXTURE_NAME_WE}.astc")
        add_custom_command(
            OUTPUT ${ASTC_OUTPUT}
            COMMAND ${ASTCENC_EXECUTABLE} -cl ${TEXTURE} ${ASTC_OUTPUT} 6x6 -medium
            DEPENDS ${TEXTURE}
            COMMENT "Compressing ${TEXTURE} to ASTC format"
        )
        list(APPEND ASTC_TEXTURES ${ASTC_OUTPUT})
    endforeach()

    add_custom_target(CompressTextures DEPENDS ${ASTC_TEXTURES})
    add_dependencies(${PROJECT_NAME} CompressTextures)
endif()

enable_testing() 
set(TEST_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
file(GLOB TEST_SOURCES ${TEST_SOURCES_DIR}/*.c)

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} ${PROJECT_NAME})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()