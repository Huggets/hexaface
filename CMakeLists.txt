cmake_minimum_required(VERSION 3.0.0)
project(Hexaface VERSION 0.1)

if (NOT DEFINED VALIDATION_LAYERS)
    set(VALIDATION_LAYERS false)
endif()

if (NOT DEFINED WINDOW_SYSTEM)
    set(WINDOW_SYSTEM "XLIB")
endif()

add_compile_options(-Wall)

add_executable(
    hexaface
    src/main.c
    src/window.c
    src/hxf.c
    src/vulkan/vulkan.c
    src/vulkan/vertex.c
    src/math/linear-algebra.c
)

target_link_libraries(hexaface vulkan m)

# If the variable VALIDATION_LAYERS is defined then build with the validation layers enabled
if (VALIDATION_LAYERS)
    target_compile_definitions(hexaface PRIVATE HXF_VALIDATION_LAYERS)
endif()

### Window systems ####
set(window_system_not_defined false)

if (${WINDOW_SYSTEM} STREQUAL "XLIB")
    target_compile_definitions(hexaface PRIVATE HXF_WINDOW_XLIB)
    target_link_libraries(hexaface X11)
else()
    set(window_system_not_defined true)
endif()

if (${window_system_not_defined})
    message(FATAL_ERROR "WINDOW_SYSTEM is not defined or is not set to the correct value: XLIB")
endif()