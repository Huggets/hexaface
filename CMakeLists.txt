cmake_minimum_required(VERSION 3.0.0)
project(Hexaface VERSION 0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_compile_options(-Wall)

add_executable(
    hexaface
    src/deps-compile.c
    src/main.c
    src/hxf.c
    src/app.c
    src/camera.c
    src/world.c
    src/win32/window.c
    src/engine/graphics-handler.c
    src/engine/pipeline.c
    src/engine/input-handler.c
    src/engine/game-handler.c
    src/math/linear-algebra.c
    src/container/map.c
)

target_include_directories(hexaface PRIVATE include)

add_custom_target(shaders)
add_custom_command(
    TARGET shaders PRE_BUILD
    COMMAND glslc ARGS -o ${CMAKE_HOME_DIRECTORY}/appdata/shaders/vertexCube.spv ${CMAKE_SOURCE_DIR}/src/glsl/cube.vert
    COMMAND glslc ARGS -o ${CMAKE_HOME_DIRECTORY}/appdata/shaders/vertexIcon.spv ${CMAKE_SOURCE_DIR}/src/glsl/icon.vert
    COMMAND glslc ARGS -o ${CMAKE_HOME_DIRECTORY}/appdata/shaders/vertexPointer.spv ${CMAKE_SOURCE_DIR}/src/glsl/pointer.vert
    COMMAND glslc ARGS -o ${CMAKE_HOME_DIRECTORY}/appdata/shaders/fragmentCube.spv ${CMAKE_SOURCE_DIR}/src/glsl/cube.frag
    COMMAND glslc ARGS -o ${CMAKE_HOME_DIRECTORY}/appdata/shaders/fragmentIcon.spv ${CMAKE_SOURCE_DIR}/src/glsl/icon.frag
    COMMAND glslc ARGS -o ${CMAKE_HOME_DIRECTORY}/appdata/shaders/fragmentPointer.spv ${CMAKE_SOURCE_DIR}/src/glsl/pointer.frag
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
    target_link_directories(hexaface PRIVATE lib)
    target_link_libraries(hexaface vulkan-1)
    target_link_options(hexaface PRIVATE -Wl,-Bstatic -lwinpthread)
endif()