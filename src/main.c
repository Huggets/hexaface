#include <stdlib.h>
#include "graphics/vulkan.h"

int main(int argc, char ** argv) {
    HxfVulkanInstance instance;

    hxfInitVulkan(&instance);
    hxfRunVulkan(&instance);
    hxfDestroyVulkan(&instance);

    return EXIT_SUCCESS;
}