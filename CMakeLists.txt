cmake_minimum_required(VERSION 3.22)
project(nkengine LANGUAGES C CXX)

# Set C and C++ Standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# ----------------------------
# Engine Paths (relative to engine root)
# ----------------------------
set(ENGINE_SRC_DIR "src")

# ----------------------------
# Engine Sources
# ----------------------------
file(GLOB_RECURSE ENGINE_C_SRC "${ENGINE_SRC_DIR}/*.c")
file(GLOB_RECURSE ENGINE_CXX_SRC "${ENGINE_SRC_DIR}/*.cpp")
file(GLOB_RECURSE ENGINE_C_EXAMPLES "examples/*.c")

# ----------------------------
# Define Engine Includes
# These paths are PUBLIC for the consuming project (nkproject)
# ----------------------------
# Define the target as an INTERFACE library before setting its properties
add_library(${PROJECT_NAME}_vendor_includes INTERFACE)

# The target_include_directories command tells the compiler:
# "Add this path to your search list (-I<path>)."
target_include_directories(${PROJECT_NAME}_vendor_includes INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendor>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/examples>
)

# ----------------------------
# Engine Macros (Compiler Definitions)
# ----------------------------
set(MACROS
    -DCGLM_FORCE_DEPTH_ZERO_TO_ONE
    -DVERBOSE
    -DENGINE_EDITOR
    -DDEBUG_TIME
    -DAO_BAKE_HIT_COUNT
)

set(ASAN
    -fsanitize=address
    -fsanitize=undefined
    -ferror-limit=0
    -g
)

# ----------------------------
# 1. Vendor Library (imgui, cglm, stb, cgltf)
# ----------------------------
file(GLOB_RECURSE IMGUI
          "vendor/imgui/*.cpp"
          "vendor/imgui/*.h"
          )
file(GLOB_RECURSE CGLM
          "vendor/cglm/*.h"
          )
file(GLOB_RECURSE STB
          "vendor/stb/*.h"
          )
file(GLOB_RECURSE CGLTF
          "vendor/cgltf/*.h"
          )

# Create a static library for all vendors
add_library(${PROJECT_NAME}_vendors STATIC ${IMGUI} ${CGLM} ${STB} ${CGLTF})

# ----------------------------
# 2. Preprocess WGSL shaders
# ----------------------------
# This custom command will generate the .wgsl files inside the build directory
# and the engine library will depend on them.
set(WGSL_OUT_FILES "")
file(GLOB_RECURSE WGSL_IN_FILES "${ENGINE_SRC_DIR}/backend/std_pipeline/*.wgsl.in")
foreach(wgsl_in ${WGSL_IN_FILES})
    get_filename_component(fname ${wgsl_in} NAME_WE)
    
    # Use CMAKE_BINARY_DIR for a clean, reliable output location (relative to the build root)
    set(wgsl_out "${CMAKE_BINARY_DIR}/generated_shaders/${fname}.wgsl")
    
    # Ensure the output directory exists
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated_shaders")

    add_custom_command(
        OUTPUT ${wgsl_out}
        COMMAND cpp
	ARGS -P ${MACROS} ${wgsl_in} > ${wgsl_out}
        DEPENDS ${wgsl_in}
        COMMENT "Preprocessing ${wgsl_in}"
    )
    
    list(APPEND WGSL_OUT_FILES ${wgsl_out})
endforeach()

# ----------------------------
# 3. Engine Library Definition
# ----------------------------
add_library(${PROJECT_NAME} STATIC
    ${ENGINE_C_SRC}
    ${ENGINE_CXX_SRC}
    ${ENGINE_C_EXAMPLES}
    ${WGSL_OUT_FILES} # Dependency: Ensures shaders are generated before linking
)

# Apply engine-specific macros
target_compile_options(${PROJECT_NAME} PUBLIC ${MACROS})
target_compile_options(${PROJECT_NAME} PRIVATE ${ASAN})

# Link the engine to its vendor libraries and set public includes
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_vendors)
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_vendor_includes)

# Required for Emscripten's WebGPU support if targeting WASM
target_link_libraries(${PROJECT_NAME} PUBLIC webgpu)

target_link_libraries(${PROJECT_NAME} PRIVATE ${ASAN})
