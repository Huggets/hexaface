cmake_minimum_required(VERSION 3.0.0)
project(Hexaface VERSION 0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_compile_options(-Wall)

add_executable(
    hexaface
    src/main.c
    src/hxf.c
    src/app.c
    src/camera.c
    src/win32/window.c
    src/engine/vulkan.c
    src/engine/pipeline.c
    src/engine/input-handler.c
    src/math/linear-algebra.c
)

target_include_directories(hexaface PRIVATE include)

add_custom_target(shaders)
add_custom_command(
    TARGET shaders PRE_BUILD
    COMMAND glslc
    ARGS -o vertex.spv ${CMAKE_SOURCE_DIR}/src/glsl/shader.vert
)
add_custom_command(
    TARGET shaders PRE_BUILD
    COMMAND glslc
    ARGS -o fragment.spv ${CMAKE_SOURCE_DIR}/src/glsl/shader.frag
)

### Variable handling ###

if(NOT DEFINED VALIDATION_LAYERS)
    set(VALIDATION_LAYERS false)
endif()
if(NOT DEFINED DEBUG_ALLOC)
    set(DEBUG_ALLOC false)
endif()

if(VALIDATION_LAYERS)
# If the variable VALIDATION_LAYERS is set to true then build with the validation layers enabled
    target_compile_definitions(hexaface PRIVATE HXF_VALIDATION_LAYERS)
endif()
if(DEBUG_ALLOC)
    # Debug the memory allocation (hxfMalloc and hxfFree for example)
    target_compile_definitions(hexaface PRIVATE HXF_DEBUG_ALLOC)
endif()

if(WIN32) # Compile for windows
    target_compile_definitions(hexaface PRIVATE HXF_WIN32)
    target_link_options(hexaface PRIVATE -municode)
    target_link_directories(hexaface PRIVATE lib)
    target_link_libraries(hexaface vulkan-1 shlwapi)
endif()