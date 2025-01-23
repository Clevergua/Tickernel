cmake_minimum_required(VERSION 3.26)
project(Tickernel VERSION 1.1 LANGUAGES C)
set(CMAKE_C_STANDARD_REQUIRED ON)
# Because of this use c99: https://stackoverflow.com/questions/40829032/how-to-install-c11-compiler-on-mac-os-with-optional-string-functions-included
set(CMAKE_C_STANDARD 99)
# Debug or Release
# message(${CMAKE_BUILD_TYPE})

find_package(Vulkan REQUIRED)
# find_library(GLFW3_LIBRARY NAMES "libglfw3.a" "libglfw.3.dylib" "glfw3.dll" "glfw3dll.lib" PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw3)
find_library(LUA_LIBRARY NAMES "liblua.dylib" "lualib53.dylib" "liblua.dll" "liblua.lib" PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib/lua)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

set(HEADER_DIRS
    ${Vulkan_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cglm
    ${CMAKE_SOURCE_DIR}/include/lua
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(LIBRARYS
    ${Vulkan_LIBRARIES}
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
    enable_language(OBJC)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(QUARTZCORE_LIBRARY QuartzCore)
    # find_library(MOLTENVK_LIBRARY MoltenVK)

    list(APPEND LIBRARYS ${COCOA_LIBRARY})
    list(APPEND LIBRARYS ${QUARTZCORE_LIBRARY})
    # list(APPEND LIBRARYS ${MOLTENVK_LIBRARY})

    file(GLOB_RECURSE OCSOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.m)
    list(APPEND SOURCES ${OCSOURCES})

    
else()
    # Ohter platform
    message("Unknown platform!")
endif()

message("   SOURCES:${SOURCES}")
add_library(${PROJECT_NAME} SHARED ${SOURCES})
message("   HEADER_DIRS:${HEADER_DIRS}")
target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_DIRS})
message("   LIBRARYS:${LIBRARYS}")
target_link_libraries(${PROJECT_NAME} ${LIBRARYS})

# Copy libs
set(DLL_LIBS)
file(GLOB_RECURSE DLL_LIBS ${CMAKE_SOURCE_DIR}/lib/*.dylib ${CMAKE_SOURCE_DIR}/lib/*.dll)
message(DLL_LIBS: ${DLL_LIBS})
add_custom_target(CopyDLL COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLL_LIBS} ${CMAKE_BINARY_DIR}/${DLL_FILENAME})
add_dependencies(${PROJECT_NAME} CopyDLL)

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
# Copy assets
add_custom_target(CopyAssets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/assets
)
add_dependencies(${PROJECT_NAME} CopyAssets)

