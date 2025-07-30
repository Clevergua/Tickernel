cmake_minimum_required(VERSION 3.26)
project(Tickernel VERSION 1.1 LANGUAGES C)


# Project configuration
# Because of this use c99: https://stackoverflow.com/questions/40829032/how-to-install-c11-compiler-on-mac-os-with-optional-string-functions-included
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Output configuration information
message(STATUS "Configuring ${PROJECT_NAME} v${PROJECT_VERSION}")

# Dependencies
find_package(Vulkan REQUIRED)

# Source and header files
file(GLOB_RECURSE SOURCE_FILES 
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/include/*.c
)

set(INCLUDE_DIRS
    ${Vulkan_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include/cglm
    ${CMAKE_SOURCE_DIR}/include/spirv_reflect
    ${CMAKE_SOURCE_DIR}/src
)

# Libraries
set(LINK_LIBRARIES
    ${Vulkan_LIBRARIES}
)

# Print configuration
message(STATUS "Sources: ${SOURCE_FILES}")
message(STATUS "Include directories: ${INCLUDE_DIRS}")
message(STATUS "Link libraries: ${LINK_LIBRARIES}")

set(CMAKE_OUTPUR)

# Build shared library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LINK_LIBRARIES})
# Resource handling
## Copy DLLs
file(GLOB_RECURSE DLL_FILES ${CMAKE_SOURCE_DIR}/lib/*.dylib ${CMAKE_SOURCE_DIR}/lib/*.dll)
if(DLL_FILES)
    add_custom_target(CopyDLL 
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLL_FILES} ${CMAKE_BINARY_DIR}
        COMMENT "Copying DLLs to ${CMAKE_BINARY_DIR}"
    )
    add_dependencies(${PROJECT_NAME} CopyDLL)
else()
    message(STATUS "No DLL files found to copy")
endif()

## Compile shaders
find_program(GLSLC glslc REQUIRED)
set(SHADER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/res/glsl)
set(SHADER_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/assets/shaders)
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

file(GLOB_RECURSE SHADER_FILES ${SHADER_SOURCE_DIR}/*.vert ${SHADER_SOURCE_DIR}/*.frag)
set(SPIRV_BINARIES)

foreach(SHADER ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SPIRV ${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv)
    
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${GLSLC} ${SHADER} -o ${SPIRV}
        MAIN_DEPENDENCY ${SHADER}
        COMMENT "Compiling shader: ${SHADER_NAME}"
        VERBATIM
    )
    
    list(APPEND SPIRV_BINARIES ${SPIRV})
endforeach()

if(SHADER_FILES)
    add_custom_target(CompileShaders DEPENDS ${SPIRV_BINARIES})
    add_dependencies(${PROJECT_NAME} CompileShaders)
    message(STATUS "Shader compilation target created")
else()
    message(STATUS "No shaders found to compile")
endif()

## Compress textures
find_program(ASTCENC astcenc)
if(ASTCENC)
    set(TEXTURE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/res/textures)
    set(TEXTURE_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/assets/textures)
    file(MAKE_DIRECTORY ${TEXTURE_OUTPUT_DIR})
    
    file(GLOB_RECURSE TEXTURE_FILES 
        ${TEXTURE_SOURCE_DIR}/*.png
        ${TEXTURE_SOURCE_DIR}/*.jpg
        ${TEXTURE_SOURCE_DIR}/*.jpeg
        ${TEXTURE_SOURCE_DIR}/*.tga
        ${TEXTURE_SOURCE_DIR}/*.bmp
    )
    
    set(ASTC_TEXTURES)
    
    foreach(TEXTURE ${TEXTURE_FILES})
        get_filename_component(TEXTURE_NAME_WE ${TEXTURE} NAME_WE)
        get_filename_component(TEXTURE_DIR ${TEXTURE} DIRECTORY)
        string(REPLACE "${TEXTURE_SOURCE_DIR}" "" RELATIVE_DIR ${TEXTURE_DIR})
        set(OUTPUT_DIR "${TEXTURE_OUTPUT_DIR}${RELATIVE_DIR}")
        file(MAKE_DIRECTORY ${OUTPUT_DIR})
        
        set(ASTC_OUTPUT "${OUTPUT_DIR}/${TEXTURE_NAME_WE}.astc")
        
        add_custom_command(
            OUTPUT ${ASTC_OUTPUT}
            COMMAND ${ASTCENC} -cl ${TEXTURE} ${ASTC_OUTPUT} 6x6 -medium
            MAIN_DEPENDENCY ${TEXTURE}
            COMMENT "Compressing texture: ${TEXTURE_NAME_WE}"
            VERBATIM
        )
        
        list(APPEND ASTC_TEXTURES ${ASTC_OUTPUT})
    endforeach()
    
    if(TEXTURE_FILES)
        add_custom_target(CompressTextures DEPENDS ${ASTC_TEXTURES})
        add_dependencies(${PROJECT_NAME} CompressTextures)
        message(STATUS "Texture compression target created")
    else()
        message(STATUS "No textures found to compress")
    endif()
else()
    message(WARNING "ASTC encoder not found - ASTC texture compression disabled")
endif()

# Testing
enable_testing()
file(GLOB TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.c)

foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} ${PROJECT_NAME})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    message(STATUS "Test added: ${TEST_NAME}")
endforeach()

message(STATUS "Configuration completed for ${PROJECT_NAME}")