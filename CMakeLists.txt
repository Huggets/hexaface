cmake_minimum_required(VERSION 3.0.0)
project(Hexaface VERSION 0.1)

add_compile_options(-Wall)

add_executable(
    hexaface
    src/main.c
    src/window.c
    src/vulkan/vulkan.c
    src/hxf.c
)

target_link_libraries(hexaface X11 vulkan)
# If VALIDATION_LAYERS is defined, then the validation are enabled
target_compile_definitions(hexaface PRIVATE HXF_VALIDATION_LAYERS)