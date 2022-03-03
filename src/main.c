#include <stdlib.h>
#include "vulkan/vulkan.h"

int main(int argc, char ** argv) {
    HxfVulkanInstance instance;

    hxfInitVulkan(&instance);
    hxfRunVulkan(&instance);
    hxfDestroyVulkan(&instance);

    return EXIT_SUCCESS;
}