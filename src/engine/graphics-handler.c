#include "graphics-handler.h"
#include "pipeline.h"
#include "../hxf.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

#include <stb/stb_image.h>

/**
 * @brief Information on the texture image.
 */
typedef struct TextureImageInfo {
    stbi_uc* pixels; ///< The pixels of the image
    int width; ///< The image’s witdh.
    int height; ///< The image’s height.
    int channels; ///< The number of channels of the image (red, blue, green, alpha...)
} TextureImageInfo;

/*
STATIC FORWARD DECLARATION
*/

/**
 * @brief Create a the vulkan instance.
 *
 * @param graphics A pointer to the HxfGraphicsHandler that will own it.
 */
static void createInstance(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Get a physical device, create the logical device and the queue that are needed.
 *
 * @param graphics A pointer to the HxfGraphicsHandler that will own them.
 */
static void createDevice(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Create the semaphores and the fences.
 *
 * @param graphics A pointer to the HxfGraphicsHandler that will own them.
 */
static void createSyncObjects(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Create the surface.
 *
 * @param graphics A pointer to the HxfGraphicsHandler that will own it.
 */
static void createSurface(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Create the swapchain.
 *
 * It gets the swapchain extent, the swapchain image format, the swapchain image count,
 * the swapchain images and create the swachain image views and the swapchain.
 *
 * @param graphics A pointer to the HxfGraphicsHandler that will own it.
 */
static void createSwapchain(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Create the frame buffers in which the image views of the swapchain
 * will be attached.
 *
 * @param graphics A pointer to the HxfGraphicsHandler that will own them.
 */
static void createFramebuffers(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Create the command pool and the command buffers.
 *
 * @param graphics A pointer to the HxfGraphicsHandler that will own them.
 */
static void createCommandBuffers(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Record the draw command buffer for the current frame.
 *
 * @param graphics A pointer to the HxfGraphicsHandler that owns it.
 * @param imageIndex The index of the image that will be used by the command buffer.
 * @param currentFrameIndex The index of the frame that is currently rendered.
 */
static void recordDrawCommandBuffer(HxfGraphicsHandler* restrict engine, uint32_t imageIndex, uint32_t currentFrameIndex);

/**
 * @brief Transfer src buffer data to dst buffer.
 *
 * @param graphics A pointer to the HxfGraphicsHandler that own the buffers.
 * @param src The source buffer.
 * @param dst The destination buffer.
 * @param srcOffset Offset inside src where the copy start.
 * @param dstOffset Offset inside dst where the copy start.
 * @param size The size of the data to transfer.
 */
static void transferBuffers(HxfGraphicsHandler* restrict engine, VkBuffer src, VkBuffer dst, VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkDeviceSize size);

/**
 * @brief Determine the highest vulkan API version available.
 *
 * @return The highest version available.
 */
static uint32_t determineApiVersion();

/**
 * @brief Get the limits of the physical device.
 *
 * @param graphics A pointer to the HxfGraphicsHandler that own the physical device.
 */
static void getVulkanLimits(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Create the depth image.
 *
 * @param graphics A pointer to the HxfGraphicsHandler that will own it.
 */
static void createDepthImage(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Create all texture images.
 *
 * @param graphics A pointer to the HxfGraphicsHandler that will own them.
 */
static void createTextureImages(HxfGraphicsHandler* restrict engine, TextureImageInfo* restrict textureInfo);

/**
 * @brief Create all the image views.
 *
 * @param graphics A pointer to the HxfGraphicsHandler that will own them.
 */
static void createImageViews(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Create the sampler for the texture images.
 *
 * @param graphics A pointer to the HxfGraphicsHandler that own the texture images and that will own the sampler.
 */
static void createTextureSampler(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Allocate memory and create the buffers and images.
 *
 * @param graphics A pointer to the HxfGraphicsHandler that will own them.
 */
static void createRessources(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Update the buffer that contains the model-view-projection matrices
 *
 * @param graphics A pointer to the HxfGraphicsHandler that own the buffer.
 */
static void updateMvpBuffer(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Update the buffer that hold the pointed cube.
 *
 * @param graphics A pointer to the HxfGraphicsHandler that hold them.
 */
static void updatePointedCubeBuffer(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Return the extensions that are required for the Vulkan instance.
 *
 * The return value is allocated and must be freed.
 *
 * @param extensions A pointer to an array of char string that will be allocated and that will
 * contains the required extensions.
 * @param count A pointer to an int that will contains the number of extensions.
 */
static void getRequiredInstanceExtensions(char*** restrict extensions, int* restrict count);

/**
 * @brief Return the memory type index of a device memory that has exactly the same properties as memoryProperties.
 *
 * @param deviceProperties A pointer to a VkPhysicalDeviceMemoryProperties of the physical device.
 * @param memoryProperties A VkMemoryPropertyFlags containing the required properties.
 */
static uint32_t getMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties* restrict deviceProperties, VkMemoryPropertyFlags memoryProperties);

#if defined(HXF_VALIDATION_LAYERS)
/**
 * @brief The debug messenger callback used to print debug message during vkCreateInstance and
 * vkDestroyInstance.
 */
static VkBool32 instanceDebugMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
);
#endif

/*
IMPLEMENTATION
*/

#if defined(HXF_VALIDATION_LAYERS)
static VkBool32 instanceDebugMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
) {
    fwrite(pCallbackData->pMessage, sizeof(char), strlen(pCallbackData->pMessage), stderr);
    fwrite("\n", sizeof(char), 1, stderr);
    return VK_FALSE;
}
#endif

static uint32_t determineApiVersion() {
    // If vkEnumerateInstanceVersion is not available then the version is 1.0
    // Otherwise this function is called to get the version
    PFN_vkEnumerateInstanceVersion func = (PFN_vkEnumerateInstanceVersion)vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceVersion");
    if (func == NULL) {
        return VK_API_VERSION_1_0;
    }
    else {
        uint32_t version;
        func(&version);
        return version;
    }
}

static void getVulkanLimits(HxfGraphicsHandler* restrict graphics) {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(graphics->physicalDevice, &props);

    vkGetPhysicalDeviceMemoryProperties(graphics->physicalDevice, &graphics->physicalDeviceMemoryProperties);

    graphics->physicalDeviceLimits = props.limits;
}

static void createInstance(HxfGraphicsHandler* restrict graphics) {
    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Hexaface",
        .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
        .apiVersion = determineApiVersion(),
    };

    VkInstanceCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo
    };

    char** requiredExtensions;
    int requiredExtensionsCount;
    getRequiredInstanceExtensions(&requiredExtensions, &requiredExtensionsCount);

    // Test that the required extensions are available
    uint32_t count = 0;
    VkExtensionProperties* extensionsProperties = NULL;

    vkEnumerateInstanceExtensionProperties(NULL, &count, NULL);
    extensionsProperties = hxfMalloc(count * sizeof(VkExtensionProperties));
    vkEnumerateInstanceExtensionProperties(NULL, &count, extensionsProperties);

    int isUnavailable = 1;
    int i = requiredExtensionsCount - 1;

    while (i != -1 && isUnavailable) {
        int j = count - 1;
        while (j != -1 && isUnavailable) {
            if (strcmp(requiredExtensions[i], extensionsProperties[j].extensionName) == 0) {
                isUnavailable = 0;
            }
            j--;
        }
        i--;
    }

    hxfFree(extensionsProperties);

    if (isUnavailable) {
        HXF_FATAL("The required instance extensions are not all available");
    }

#if defined(HXF_VALIDATION_LAYERS)
    // Test that the validation layer is available
    vkEnumerateInstanceLayerProperties(&count, NULL);
    VkLayerProperties* layerProperties = hxfMalloc(count * sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&count, layerProperties);

    const char* const validationLayers[] = { "VK_LAYER_KHRONOS_validation" };

    isUnavailable = 1;
    i = count - 1;
    while (i != -1 && isUnavailable) {
        if (strcmp(layerProperties[i].layerName, validationLayers[0]) == 0) {
            isUnavailable = 0;
        }
        i--;
    }

    hxfFree(layerProperties);

    if (isUnavailable) {
        HXF_FATAL("The layer VK_LAYER_KHRONOS_validation is unavailable");
    }

    // If everything went well, enable them
    info.enabledLayerCount = 1;
    info.ppEnabledLayerNames = validationLayers;

    // Test that the debug utils extension is available
    vkEnumerateInstanceExtensionProperties(validationLayers[0], &count, NULL);
    VkExtensionProperties* extensionProperties = hxfMalloc(count * sizeof(VkExtensionProperties));
    vkEnumerateInstanceExtensionProperties(validationLayers[0], &count, extensionProperties);

    i = count - 1;
    isUnavailable = 1;
    while (i != -1 && isUnavailable) {
        if (strcmp(extensionProperties[i].extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0) {
            isUnavailable = 0;
        }
        i--;
    }

    hxfFree(extensionProperties);

    if (isUnavailable) {
        HXF_FATAL("The extension " VK_EXT_DEBUG_UTILS_EXTENSION_NAME " is unavailable");
    }

    // Add the debug utils extension to the required extensions
    requiredExtensionsCount += 1;
    requiredExtensions = hxfRealloc(requiredExtensions, requiredExtensionsCount * sizeof(char*));
    requiredExtensions[requiredExtensionsCount - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

    info.enabledExtensionCount = (uint32_t)requiredExtensionsCount;
    info.ppEnabledExtensionNames = (const char* const*)requiredExtensions;

    // Add the debug messenger
    VkDebugUtilsMessengerCreateInfoEXT debugInfo = { 0 };
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugInfo.pfnUserCallback = instanceDebugMessengerCallback;

    info.pNext = &debugInfo;
#else
    info.enabledExtensionCount = (uint32_t)requiredExtensionsCount;
    info.ppEnabledExtensionNames = (const char* const*)requiredExtensions;
#endif

    HXF_TRY_VK(vkCreateInstance(&info, NULL, &graphics->instance));

    hxfFree(requiredExtensions);
}

static void createDevice(HxfGraphicsHandler* restrict graphics) {
    // Choose a physical device
    uint32_t count;
    vkEnumeratePhysicalDevices(graphics->instance, &count, NULL);

    if (count == 0) {
        HXF_FATAL("Could not find a device that support vulkan");
    }

    // Take the first device
    VkPhysicalDevice* physicalDevices = hxfMalloc(count * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(graphics->instance, &count, physicalDevices);
    graphics->physicalDevice = physicalDevices[0];
    hxfFree(physicalDevices);

    // Choose the queue that will be used
    float queuePriorities[] = { 1.f };
    VkDeviceQueueCreateInfo queueInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueCount = 1,
        .pQueuePriorities = queuePriorities,
    };

    vkGetPhysicalDeviceQueueFamilyProperties(graphics->physicalDevice, &count, NULL);
    VkQueueFamilyProperties* props = hxfMalloc(count * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(graphics->physicalDevice, &count, props);
    int queueNotFound = 1;
    int i = count - 1;
    while (i != -1 && queueNotFound) {
        if (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueNotFound = 0;
            queueInfo.queueFamilyIndex = i;
            graphics->graphicsQueueFamilyIndex = i;
        }
        i--;
    }

    hxfFree(props);

    if (queueNotFound) {
        HXF_FATAL("No graphics queue found");
    }

    // Verify the extensions are available
    const char* const enabledExtensions[] = { "VK_KHR_swapchain" };
    const uint32_t enabledExtensionCount = 1;

    vkEnumerateDeviceExtensionProperties(graphics->physicalDevice, NULL, &count, NULL);
    VkExtensionProperties* extensionProperties = hxfMalloc(count * sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(graphics->physicalDevice, NULL, &count, extensionProperties);

    i = enabledExtensionCount - 1;
    int available = 1;
    while (available && i != -1) {
        int j = count - 1;
        int notFound = 1;
        while (notFound && j != -1) {
            if (strcmp(enabledExtensions[i], extensionProperties[j].extensionName) == 0) {
                notFound = 0;
            }

            j--;
        }

        if (notFound) {
            available = 0;
        }

        i--;
    }

    hxfFree(extensionProperties);

    if (!available) {
        HXF_FATAL("Not all the required device extensions are available");
    }

    // Create the logical device
    VkDeviceCreateInfo deviceInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueInfo,
        .enabledExtensionCount = enabledExtensionCount,
        .ppEnabledExtensionNames = enabledExtensions,
    };

    HXF_TRY_VK(vkCreateDevice(graphics->physicalDevice, &deviceInfo, NULL, &graphics->device));

    // Get the graphics queue
    vkGetDeviceQueue(graphics->device, queueInfo.queueFamilyIndex, 0, &graphics->graphicsQueue);
}

static void createCommandBuffers(HxfGraphicsHandler* restrict graphics) {
    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = graphics->graphicsQueueFamilyIndex,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    };
    HXF_TRY_VK(vkCreateCommandPool(graphics->device, &poolInfo, NULL, &graphics->commandPool));

    graphics->drawCommandBuffers = &graphics->commandBuffers[0];
    graphics->transferCommandBuffer = &graphics->commandBuffers[HXF_MAX_RENDERED_FRAMES];

    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = graphics->commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = sizeof(graphics->commandBuffers) / sizeof(VkCommandBuffer)
    };
    HXF_TRY_VK(vkAllocateCommandBuffers(graphics->device, &allocInfo, graphics->commandBuffers));
}

static void recordDrawCommandBuffer(HxfGraphicsHandler* restrict graphics, uint32_t imageIndex, uint32_t currentFrameIndex) {
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };

    VkClearValue clearValues[] = {
        {.color = { { 16.0f / 255.0f, 154.0f / 255.0f, 209.0f / 255.0f, 1.0f } }},
        {.depthStencil = { 1.0f, 0.0f }}
    };

    VkRenderPassBeginInfo renderPassBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = graphics->renderPass,
        .renderArea.offset = {
            0, 0
        },
        .renderArea.extent = graphics->swapchainExtent,
        .clearValueCount = 2,
        .pClearValues = clearValues,
        .framebuffer = graphics->swapchainFramebuffers[imageIndex],
    };

    HXF_TRY_VK(vkBeginCommandBuffer(graphics->drawCommandBuffers[currentFrameIndex], &beginInfo));

    vkCmdBeginRenderPass(graphics->drawCommandBuffers[currentFrameIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(graphics->drawCommandBuffers[currentFrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics->cubePipeline);
    vkCmdBindDescriptorSets(
        graphics->drawCommandBuffers[currentFrameIndex],
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        graphics->cubePipelineLayout,
        0, 1, &graphics->cubeDescriptorSets[currentFrameIndex],
        0, NULL
    );
    VkBuffer boundBuffers[] = {
        graphics->drawingData.deviceBuffer,
        graphics->drawingData.deviceBuffer
    };
    VkDeviceSize offsets[] = {
        graphics->drawingData.cubesVerticesOffset - graphics->drawingData.deviceBufferOffset,
        graphics->drawingData.cubeInstancesOffset - graphics->drawingData.deviceBufferOffset
    };
    vkCmdBindVertexBuffers(graphics->drawCommandBuffers[currentFrameIndex], 0, 2, boundBuffers, offsets);
    vkCmdBindIndexBuffer(graphics->drawCommandBuffers[currentFrameIndex], graphics->drawingData.deviceBuffer, graphics->drawingData.cubesVertexIndicesOffset - graphics->drawingData.deviceBufferOffset, VK_INDEX_TYPE_UINT32);

    // The pointed cube

    if (graphics->camera->isPointingToCube) {
        vkCmdDrawIndexed(graphics->drawCommandBuffers[currentFrameIndex], HXF_CUBE_VERTEX_INDEX_COUNT, 1, 0, 0, HXF_CUBE_INSTANCE_COUNT * 6);
    }

    // All the cubes
    // (A draw call for each faces)

    vkCmdDrawIndexed(graphics->drawCommandBuffers[currentFrameIndex], 6, graphics->drawingData.faceTopCount, 0, 0, 0);
    vkCmdDrawIndexed(graphics->drawCommandBuffers[currentFrameIndex], 6, graphics->drawingData.faceBackCount, 6, 0, HXF_CUBE_INSTANCE_COUNT * 1);
    vkCmdDrawIndexed(graphics->drawCommandBuffers[currentFrameIndex], 6, graphics->drawingData.faceBottomCount, 12, 0, HXF_CUBE_INSTANCE_COUNT * 2);
    vkCmdDrawIndexed(graphics->drawCommandBuffers[currentFrameIndex], 6, graphics->drawingData.faceFrontCount, 18, 0, HXF_CUBE_INSTANCE_COUNT * 3);
    vkCmdDrawIndexed(graphics->drawCommandBuffers[currentFrameIndex], 6, graphics->drawingData.faceRightCount, 24, 0, HXF_CUBE_INSTANCE_COUNT * 4);
    vkCmdDrawIndexed(graphics->drawCommandBuffers[currentFrameIndex], 6, graphics->drawingData.faceLeftCount, 30, 0, HXF_CUBE_INSTANCE_COUNT * 5);

    vkCmdBindPipeline(graphics->drawCommandBuffers[currentFrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics->iconPipeline);
    vkCmdBindDescriptorSets(
        graphics->drawCommandBuffers[currentFrameIndex],
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        graphics->iconPipelineLayout,
        0, 1, &graphics->iconDescriptorSets[currentFrameIndex],
        0, NULL
    );
    offsets[0] = graphics->drawingData.iconVerticesOffset - graphics->drawingData.deviceBufferOffset;
    offsets[1] = graphics->drawingData.iconInstancesOffset - graphics->drawingData.deviceBufferOffset;
    vkCmdBindVertexBuffers(graphics->drawCommandBuffers[currentFrameIndex], 0, 2, boundBuffers, offsets);
    vkCmdBindIndexBuffer(graphics->commandBuffers[currentFrameIndex], graphics->drawingData.deviceBuffer, graphics->drawingData.iconVertexIndicesOffset - graphics->drawingData.deviceBufferOffset, VK_INDEX_TYPE_UINT32);
    vkCmdPushConstants(graphics->drawCommandBuffers[currentFrameIndex], graphics->iconPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(HxfIconPushConstantData), &graphics->drawingData.iconPushConstants);
    vkCmdDrawIndexed(graphics->drawCommandBuffers[currentFrameIndex], HXF_ICON_VERTEX_INDEX_COUNT, 1, 0, 0, 0);

    vkCmdEndRenderPass(graphics->drawCommandBuffers[currentFrameIndex]);

    HXF_TRY_VK(vkEndCommandBuffer(graphics->drawCommandBuffers[currentFrameIndex]));
}

static void transferBuffers(HxfGraphicsHandler* restrict graphics, VkBuffer src, VkBuffer dst, VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkDeviceSize size) {
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    HXF_TRY_VK(vkBeginCommandBuffer(*graphics->transferCommandBuffer, &beginInfo));
    VkBufferCopy copyRegion = {
        .srcOffset = srcOffset,
        .dstOffset = dstOffset,
        .size = size
    };
    vkCmdCopyBuffer(*graphics->transferCommandBuffer, src, dst, 1, &copyRegion);
    HXF_TRY_VK(vkEndCommandBuffer(*graphics->transferCommandBuffer));

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = graphics->transferCommandBuffer
    };
    vkQueueSubmit(graphics->graphicsQueue, 1, &submitInfo, graphics->fence);
    vkWaitForFences(graphics->device, 1, &graphics->fence, VK_TRUE, UINT64_MAX);
    vkResetFences(graphics->device, 1, &graphics->fence);
}

static void createSurface(HxfGraphicsHandler* restrict graphics) {
    hxfCreateWindowSurface(graphics->mainWindow, graphics->instance, &graphics->mainWindowSurface);

    VkBool32 isSupported;
    vkGetPhysicalDeviceSurfaceSupportKHR(
        graphics->physicalDevice,
        graphics->graphicsQueueFamilyIndex,
        graphics->mainWindowSurface,
        &isSupported);

    if (!isSupported) {
        HXF_FATAL("The Window System Integration is not supported");
    }

}

static void getRequiredInstanceExtensions(char*** restrict extensions, int* restrict count) {
    hxfGetRequiredWindowExtensions(extensions, count);
}

static void createSwapchain(HxfGraphicsHandler* restrict graphics) {
    // Get the surfaceInformations
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VkSurfaceFormatKHR* surfaceFormats;
    uint32_t surfaceFormatCount;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(graphics->physicalDevice, graphics->mainWindowSurface, &surfaceCapabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(graphics->physicalDevice, graphics->mainWindowSurface, &surfaceFormatCount, NULL);
    surfaceFormats = hxfMalloc(surfaceFormatCount * sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(graphics->physicalDevice, graphics->mainWindowSurface, &surfaceFormatCount, surfaceFormats);

    // Choose the swapchain image format and colorspace
    graphics->swapchainImageFormat = VK_FORMAT_R8G8B8A8_SRGB;
    VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    int notFound = 1;
    int i = surfaceFormatCount - 1;
    while (notFound && i != -1) {
        if (surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && surfaceFormats[i].format == VK_FORMAT_R8G8B8A8_SRGB) {
            notFound = 0;
        }

        i--;
    }

    if (notFound) {
        graphics->swapchainImageFormat = surfaceFormats[0].format;
        colorSpace = surfaceFormats[0].colorSpace;
    }

    hxfFree(surfaceFormats);

    // Choose the image count
    uint32_t imageCount = 3; // Triple buffering
    // Clamp imageCount
    if (imageCount < surfaceCapabilities.minImageCount) {
        imageCount = surfaceCapabilities.minImageCount;
    }
    else if (imageCount > surfaceCapabilities.maxImageCount) {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    // Set the swapchain extent of the engine
    graphics->swapchainExtent = surfaceCapabilities.currentExtent;

    // Create the swapchain
    VkSwapchainCreateInfoKHR swapchainInfo = { 0 };
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = graphics->mainWindowSurface;
    swapchainInfo.minImageCount = imageCount;
    swapchainInfo.imageFormat = graphics->swapchainImageFormat;
    swapchainInfo.imageColorSpace = colorSpace;
    swapchainInfo.imageExtent = graphics->swapchainExtent;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.preTransform = surfaceCapabilities.currentTransform;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; // Choose FIFO for now as it is always available
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

    HXF_TRY_VK(vkCreateSwapchainKHR(graphics->device, &swapchainInfo, NULL, &graphics->swapchain));

    // Get the swapchain images
    vkGetSwapchainImagesKHR(graphics->device, graphics->swapchain, &graphics->swapchainImageCount, NULL);
    graphics->swapchainImages = hxfMalloc(graphics->swapchainImageCount * sizeof(VkImage));
    vkGetSwapchainImagesKHR(graphics->device, graphics->swapchain, &graphics->swapchainImageCount, graphics->swapchainImages);

    // Create the swapchain image views
    graphics->swapchainImageView = hxfMalloc(graphics->swapchainImageCount * sizeof(VkImageView));

    VkImageViewCreateInfo imageViewInfo = { 0 };
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = graphics->swapchainImageFormat;
    imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;

    for (int i = graphics->swapchainImageCount - 1; i != -1; i--) {
        imageViewInfo.image = graphics->swapchainImages[i];

        HXF_TRY_VK(vkCreateImageView(graphics->device, &imageViewInfo, NULL, &graphics->swapchainImageView[i]));
    }
}

static void createFramebuffers(HxfGraphicsHandler* restrict graphics) {
    graphics->swapchainFramebuffers = hxfMalloc(graphics->swapchainImageCount * sizeof(VkFramebuffer));

    VkFramebufferCreateInfo framebufferInfo = { 0 };
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = graphics->renderPass;
    framebufferInfo.attachmentCount = 2;
    framebufferInfo.width = graphics->swapchainExtent.width;
    framebufferInfo.height = graphics->swapchainExtent.height;
    framebufferInfo.layers = 1;

    for (int i = graphics->swapchainImageCount - 1; i != -1; i--) {
        VkImageView attachments[] = {
            graphics->swapchainImageView[i],
            graphics->drawingData.depthImageView
        };
        framebufferInfo.pAttachments = attachments;

        HXF_TRY_VK(vkCreateFramebuffer(graphics->device, &framebufferInfo, NULL, &graphics->swapchainFramebuffers[i]));
    }
}

static void createSyncObjects(HxfGraphicsHandler* restrict graphics) {
    VkSemaphoreCreateInfo semaInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

    VkFenceCreateInfo signaledFenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    VkFenceCreateInfo unsignaledFenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };

    VkResult result = vkCreateFence(graphics->device, &unsignaledFenceInfo, NULL, &graphics->fence);

    int i = 0;
    while (!result && i != HXF_MAX_RENDERED_FRAMES) {
        result =
            vkCreateSemaphore(graphics->device, &semaInfo, NULL, &graphics->nextImageAvailableSemaphores[i])
            || vkCreateSemaphore(graphics->device, &semaInfo, NULL, &graphics->nextImageSubmitedSemaphores[i])
            || vkCreateFence(graphics->device, &signaledFenceInfo, NULL, &graphics->imageRenderedFences[i]);
        i++;
    }

    if (result) {
        HXF_FATAL("Could not create the syncronisation objects");
    }
}

static void createTextureImages(HxfGraphicsHandler* restrict graphics, TextureImageInfo* restrict textureInfo) {
    // Load the textures

    char* texturePath = hxfMalloc(sizeof(char) * (strlen(graphics->appdataDirectory) + 31));
    strcpy(texturePath, graphics->appdataDirectory);
    strcat(texturePath, "/textures/textures.png");

    textureInfo->pixels = stbi_load(texturePath, &textureInfo->width, &textureInfo->height, &textureInfo->channels, STBI_rgb_alpha);

    hxfFree(texturePath);

    if (!textureInfo->pixels) {
        HXF_FATAL("Could not load texture files");
    }

    // Create the images

    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .extent = {
            .width = textureInfo->width,
            .height = textureInfo->height,
            .depth = 1
        },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &graphics->graphicsQueueFamilyIndex,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };
    HXF_TRY_VK(vkCreateImage(graphics->device, &imageInfo, NULL, &graphics->drawingData.textureImage));
}

static uint32_t getMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties* restrict deviceProperties, VkMemoryPropertyFlags memoryProperties) {
    uint32_t index = -1;

    uint32_t i = 0;
    while (i != deviceProperties->memoryTypeCount && index == -1) {
        // If the properties are exactly the same
        if ((deviceProperties->memoryTypes[i].propertyFlags ^ memoryProperties) == 0) {
            index = i;
        }
        i++;
    }

    if (index == -1) {
        HXF_FATAL("Could not find a compatible memory type");
    }

    return index;
}

static VkDeviceSize getAlignement(VkDeviceSize alignementRequirement, VkDeviceSize offset) {
    VkDeviceSize alignement = offset % alignementRequirement;
    return (alignement == 0)
        ? 0
        : alignementRequirement - alignement;
}

/**
 * @brief Align an object according to its memory requirements.
 *
 * @param memoryRequirements A pointer to the VkMemoryRequirements that describe the needs for the object.
 * @param memoryOffset The offset where the alignement starts. It is then increased with the size of the
 * object (and the alignement).
 * @param objectOffset A pointer to the object memory offset that will be modified.
 * @param objectSize A pointer to the object memory size that will be modified.
 */
static void alignObject(const VkMemoryRequirements* restrict memoryRequirements, VkDeviceSize* restrict memoryOffset, VkDeviceSize* restrict objectOffset, VkDeviceSize* restrict objectSize) {
    *objectOffset = *memoryOffset + getAlignement(memoryRequirements->alignment, *memoryOffset);
    *objectSize = memoryRequirements->size;
    *memoryOffset = *objectOffset + *objectSize;
}

/**
 * @brief Align the buffer in memory according to the memory requirements.
 *
 * It also updates the offsets.
 *
 * @param memoryRequirements The buffer memory requirements.
 * @param bufferOffset The offset in memory of the buffer.
 * @param offsets An array of pointer to the offsets of the objects inside the buffer
 * that will be modified according the memoryRequirements.
 * @param offsetCount The number of elements of offsets.
 */
static void alignBuffer(const VkMemoryRequirements* restrict memoryRequirements, VkDeviceSize* restrict bufferOffset, VkDeviceSize** restrict offsets, size_t offsetCount) {
    VkDeviceSize additionalOffset = getAlignement(memoryRequirements->alignment, *bufferOffset);

    for (int i = 0; i != offsetCount; i++) {
        *(offsets[i]) += additionalOffset;
    }

    *bufferOffset += additionalOffset;
}

static void allocateMemory(HxfGraphicsHandler* restrict graphics, const TextureImageInfo* restrict textureInfo) {
    HxfDrawingData* const restrict drawingData = &graphics->drawingData; // Reference to the drawing data
    const VkDeviceSize textureImageSize = textureInfo->width * textureInfo->height * textureInfo->channels;
    VkDeviceSize deviceBufferSizeRequired;
    VkDeviceSize transferBufferSizeRequired;
    VkDeviceSize deviceBufferDataSize;
    VkDeviceSize hostMemorySize;   ///< The total size of the host memory
    VkDeviceSize deviceMemorySize; //< The total size of the device memory

    VkBufferCreateInfo bufferInfo = {
        // Default value that does not change accross the different buffers
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &graphics->graphicsQueueFamilyIndex,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    /*
    COMPUTE THE MEMORY OFFSETS
    */

    size_t memoryOffset = 0;
    VkMemoryRequirements memoryRequirements;

    /*****************
     * DEVICE MEMORY *
     *****************/

    // Depth image
    vkGetImageMemoryRequirements(graphics->device, graphics->drawingData.depthImage, &memoryRequirements);
    alignObject(&memoryRequirements, &memoryOffset, &graphics->drawingData.depthImageOffset, &graphics->drawingData.depthImageSize);

    // Texture image
    vkGetImageMemoryRequirements(graphics->device, drawingData->textureImage, &memoryRequirements);
    alignObject(&memoryRequirements, &memoryOffset, &drawingData->textureImageOffset, &drawingData->textureImageSize);

// --- Start of the device buffer --- //

    drawingData->deviceBufferOffset = memoryOffset;

    // Cube vertex
    drawingData->cubesVerticesOffset = memoryOffset;
    drawingData->cubesVerticesSize = sizeof(drawingData->cubesVertices);
    memoryOffset = drawingData->cubesVerticesOffset + drawingData->cubesVerticesSize;

    // Cube vertex index
    drawingData->cubesVertexIndicesOffset = memoryOffset;
    drawingData->cubesVertexIndicesSize = sizeof(drawingData->cubesVertexIndices);
    memoryOffset = drawingData->cubesVertexIndicesOffset + drawingData->cubesVertexIndicesSize;

    // Cube instance
    drawingData->cubeInstancesOffset = memoryOffset;
    drawingData->cubeInstancesSize = sizeof(drawingData->cubeInstances);
    memoryOffset = drawingData->cubeInstancesOffset + drawingData->cubeInstancesSize;

    // Pointed cube
    drawingData->pointedCubeOffset = memoryOffset;
    drawingData->pointedCubeSize = sizeof(HxfCubeInstanceData);
    memoryOffset = drawingData->pointedCubeOffset + drawingData->pointedCubeSize;

    // Icon vertex data
    drawingData->iconVerticesOffset = memoryOffset;
    drawingData->iconVerticesSize = sizeof(drawingData->iconVertices);
    memoryOffset = drawingData->iconVerticesOffset + drawingData->iconVerticesSize;

    // Icon vertex index
    drawingData->iconVertexIndicesOffset = memoryOffset;
    drawingData->iconVertexIndicesSize = sizeof(drawingData->iconVertexIndices);
    memoryOffset = drawingData->iconVertexIndicesOffset + drawingData->iconVertexIndicesSize;

    // Icon instance data
    drawingData->iconInstancesOffset = memoryOffset;
    drawingData->iconInstancesSize = sizeof(drawingData->iconInstances);
    memoryOffset = drawingData->iconInstancesOffset + drawingData->iconInstancesSize;

    // Buffer creation
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferInfo.size = memoryOffset - drawingData->deviceBufferOffset;
    HXF_TRY_VK(vkCreateBuffer(graphics->device, &bufferInfo, NULL, &drawingData->deviceBuffer));
    vkGetBufferMemoryRequirements(graphics->device, drawingData->deviceBuffer, &memoryRequirements);
    deviceBufferDataSize = bufferInfo.size;

    VkDeviceSize* deviceBufferOffsets[] = {
        &drawingData->cubesVerticesOffset,
        &drawingData->cubesVertexIndicesOffset,
        &drawingData->cubeInstancesOffset,
        &drawingData->pointedCubeOffset,
        &drawingData->iconVerticesOffset,
        &drawingData->iconInstancesOffset
    };
    alignBuffer(&memoryRequirements, &drawingData->deviceBufferOffset, deviceBufferOffsets, sizeof(deviceBufferOffsets) / sizeof(VkDeviceSize*));
    deviceBufferSizeRequired = memoryRequirements.size;
    memoryOffset = drawingData->deviceBufferOffset + deviceBufferSizeRequired;

// --- End of the device buffer --- //

    deviceMemorySize = memoryOffset;

    /***************
     * HOST MEMORY *
     ***************/

    memoryOffset = 0;

// --- Start of host buffer data --- //

    drawingData->hostBufferOffset = memoryOffset;

    // Mvp
    drawingData->mvpOffset = memoryOffset;
    drawingData->mvpSize = sizeof(drawingData->mvp);
    memoryOffset = drawingData->mvpOffset + drawingData->mvpSize;

    // Host buffer
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferInfo.size = memoryOffset - drawingData->hostBufferOffset;
    HXF_TRY_VK(vkCreateBuffer(graphics->device, &bufferInfo, NULL, &drawingData->hostBuffer));
    vkGetBufferMemoryRequirements(graphics->device, drawingData->hostBuffer, &memoryRequirements);

    VkDeviceSize* hostBufferOffsets[] = {
        &drawingData->mvpOffset
    };
    alignBuffer(&memoryRequirements, &drawingData->hostBufferOffset, hostBufferOffsets, sizeof(hostBufferOffsets) / sizeof(VkDeviceSize*));
    memoryOffset = drawingData->hostBufferOffset + memoryRequirements.size;

// --- End of host buffer data --- //

    hostMemorySize = memoryOffset;

    // Transfer buffer
    bufferInfo.size = deviceBufferSizeRequired; // We need to copy from the device buffer
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    HXF_TRY_VK(vkCreateBuffer(graphics->device, &bufferInfo, NULL, &drawingData->transferBuffer));
    vkGetBufferMemoryRequirements(graphics->device, drawingData->transferBuffer, &memoryRequirements);
    transferBufferSizeRequired = memoryRequirements.size;

    // Allocate the memories

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO
    };

    allocInfo.allocationSize = max(hostMemorySize, transferBufferSizeRequired); // We need to transfer to the device as well
    allocInfo.memoryTypeIndex = getMemoryTypeIndex(&graphics->physicalDeviceMemoryProperties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    HXF_TRY_VK(vkAllocateMemory(graphics->device, &allocInfo, NULL, &graphics->hostMemory));

    allocInfo.allocationSize = deviceMemorySize; // todo this may not work, it should get the buffer and image memory requirements (not sure, TO VERIFY)
    allocInfo.memoryTypeIndex = getMemoryTypeIndex(&graphics->physicalDeviceMemoryProperties, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    HXF_TRY_VK(vkAllocateMemory(graphics->device, &allocInfo, NULL, &graphics->deviceMemory));

    // Memory binding

    vkBindBufferMemory(graphics->device, drawingData->hostBuffer, graphics->hostMemory, drawingData->hostBufferOffset);
    vkBindBufferMemory(graphics->device, drawingData->deviceBuffer, graphics->deviceMemory, drawingData->deviceBufferOffset);
    vkBindBufferMemory(graphics->device, drawingData->transferBuffer, graphics->hostMemory, 0);
    vkBindImageMemory(graphics->device, graphics->drawingData.depthImage, graphics->deviceMemory, graphics->drawingData.depthImageOffset);
    vkBindImageMemory(graphics->device, drawingData->textureImage, graphics->deviceMemory, drawingData->textureImageOffset);

    // Transfer the device buffers data, from the host to the device memory

    void* data;
    HXF_TRY_VK(vkMapMemory(graphics->device, graphics->hostMemory, 0, transferBufferSizeRequired, 0, &data));
    data -= drawingData->deviceBufferOffset; // Start from 0 instead of using the device memory offset
    memcpy(data + drawingData->cubesVerticesOffset, drawingData->cubesVertices, drawingData->cubesVerticesSize);
    memcpy(data + drawingData->cubesVertexIndicesOffset, drawingData->cubesVertexIndices, drawingData->cubesVertexIndicesSize);
    memcpy(data + drawingData->cubeInstancesOffset, drawingData->cubeInstances, drawingData->cubeInstancesSize);
    memcpy(data + drawingData->iconVerticesOffset, drawingData->iconVertices, drawingData->iconVerticesSize);
    memcpy(data + drawingData->iconVertexIndicesOffset, drawingData->iconVertexIndices, drawingData->iconVertexIndicesSize);
    memcpy(data + drawingData->iconInstancesOffset, drawingData->iconInstances, drawingData->iconInstancesSize);
    vkUnmapMemory(graphics->device, graphics->hostMemory); // todo removing unmapping, and use the same mapping to transfer the vertex/instance data and the texture image.

    transferBuffers(graphics, drawingData->transferBuffer, drawingData->deviceBuffer, 0, 0, deviceBufferDataSize);

    /*
    TEXTURE IMAGE TRANSFER
    */

    // Write the texture in memory

    HXF_TRY_VK(vkMapMemory(graphics->device, graphics->hostMemory, 0, textureImageSize, 0, &data));
    memcpy(data, textureInfo->pixels, textureImageSize);
    vkUnmapMemory(graphics->device, graphics->hostMemory);

    // Record a command buffer that will transition the image and transfer the texture in an image

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    HXF_TRY_VK(vkBeginCommandBuffer(*graphics->transferCommandBuffer, &beginInfo));

    // Transition the image layout to a transfer layout

    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = drawingData->textureImage,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };
    vkCmdPipelineBarrier(
        *graphics->transferCommandBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        0, NULL, 0, NULL, 1, &barrier
    );

    // Copy the texture data from the buffer to the texture image

    VkBufferImageCopy imageCopy = {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1
        },
        .imageOffset = {
            0, 0, 0
        },
        .imageExtent = {
            .width = textureInfo->width,
            .height = textureInfo->height,
            .depth = 1
        }
    };
    vkCmdCopyBufferToImage(*graphics->transferCommandBuffer, drawingData->transferBuffer, drawingData->textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

    // Transition the image layout to a shader read only layout, to be able to use it in the shaders

    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    vkCmdPipelineBarrier(
        *graphics->transferCommandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0,
        0, NULL, 0, NULL, 1, &barrier
    );

    HXF_TRY_VK(vkEndCommandBuffer(*graphics->transferCommandBuffer));

    // Execute the command buffer

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = graphics->transferCommandBuffer
    };
    vkQueueSubmit(graphics->graphicsQueue, 1, &submitInfo, graphics->fence);
    vkWaitForFences(graphics->device, 1, &graphics->fence, VK_TRUE, UINT64_MAX);
    vkResetFences(graphics->device, 1, &graphics->fence);

    // Write the host memory data that is actually needed

    HXF_TRY_VK(vkMapMemory(graphics->device, graphics->hostMemory, 0, drawingData->mvpSize, 0, &data));
    memcpy(data, &drawingData->mvp, drawingData->mvpSize);
    vkUnmapMemory(graphics->device, graphics->hostMemory);
}

static void createDepthImage(HxfGraphicsHandler* restrict graphics) {
    // Find a format for the image
    VkFormat formats[] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
    };

    int i = 0;
    int notFound = 1;
    while (i != 3 && notFound) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(graphics->physicalDevice, formats[i], &properties);
        if ((properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            graphics->drawingData.depthImageFormat = formats[i];
            notFound = 0;
        }

        i++;
    }

    if (notFound) {
        HXF_FATAL("No image format found for the depth image");
    }

    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = graphics->drawingData.depthImageFormat,
        .extent = { graphics->mainWindow->width, graphics->mainWindow->height, 1 },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &graphics->graphicsQueueFamilyIndex,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    HXF_TRY_VK(vkCreateImage(graphics->device, &imageInfo, NULL, &graphics->drawingData.depthImage));
}

static void createImageViews(HxfGraphicsHandler* restrict graphics) {
    // depth image view

    VkImageViewCreateInfo imageViewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = graphics->drawingData.depthImage,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = graphics->drawingData.depthImageFormat,
        .components = { 0 },
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };
    HXF_TRY_VK(vkCreateImageView(graphics->device, &imageViewInfo, NULL, &graphics->drawingData.depthImageView));

    // texture images view

    imageViewInfo.image = graphics->drawingData.textureImage;
    imageViewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    HXF_TRY_VK(vkCreateImageView(graphics->device, &imageViewInfo, NULL, &graphics->drawingData.textureImageView));
}

static void createTextureSampler(HxfGraphicsHandler* restrict graphics) {
    VkSamplerCreateInfo samplerInfo = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_NEAREST,
        .minFilter = VK_FILTER_NEAREST,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
        .anisotropyEnable = VK_FALSE,
        .compareEnable = VK_FALSE,
        .maxLod = 0,
        .minLod = 0,
        .borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
        .unnormalizedCoordinates = VK_FALSE
    };
    HXF_TRY_VK(vkCreateSampler(graphics->device, &samplerInfo, NULL, &graphics->drawingData.textureSampler));
}

static void createRessources(HxfGraphicsHandler* restrict graphics) {
    TextureImageInfo textureInfo;

    createDepthImage(graphics);
    createTextureImages(graphics, &textureInfo);

    allocateMemory(graphics, &textureInfo);

    createImageViews(graphics);
    createTextureSampler(graphics);

    stbi_image_free(textureInfo.pixels);
}

static void updateMvpBuffer(HxfGraphicsHandler* restrict graphics) {
    // Update the view matrix according to the camera

    graphics->drawingData.mvp.view = hxfViewMatrix(&graphics->camera->position, &graphics->camera->direction, &graphics->camera->up);

    void* data;
    HXF_TRY_VK(vkMapMemory(
        graphics->device,
        graphics->hostMemory,
        graphics->drawingData.mvpOffset,
        graphics->drawingData.mvpSize,
        0,
        &data
    ));
    memcpy(data, &graphics->drawingData.mvp, graphics->drawingData.mvpSize);
    vkUnmapMemory(graphics->device, graphics->hostMemory);
}

static void updatePointedCubeBuffer(HxfGraphicsHandler* restrict graphics) {
    void* data;
    HXF_TRY_VK(vkMapMemory(graphics->device, graphics->hostMemory, 0, graphics->drawingData.pointedCubeSize, 0, &data));
    const HxfCubeInstanceData pointedCube = {
        {
            (float)graphics->camera->nearPointedCube.x,
            (float)graphics->camera->nearPointedCube.y,
            (float)graphics->camera->nearPointedCube.z
        },
        0
    };
    memcpy(data, &pointedCube, sizeof(pointedCube));
    vkUnmapMemory(graphics->device, graphics->hostMemory);

    transferBuffers(graphics, graphics->drawingData.transferBuffer, graphics->drawingData.deviceBuffer, 0, graphics->drawingData.pointedCubeOffset - graphics->drawingData.deviceBufferOffset, sizeof(pointedCube));
}

void hxfGraphicsInit(HxfGraphicsHandler* restrict graphics) {
    createInstance(graphics);
    createDevice(graphics);
    getVulkanLimits(graphics);
    createSyncObjects(graphics);
    createCommandBuffers(graphics);

    // Create the buffers and the images
    createRessources(graphics);

    createSurface(graphics);
    createSwapchain(graphics);
    createPipelines(graphics);
    createFramebuffers(graphics);
}

void hxfGraphicsDestroy(HxfGraphicsHandler* restrict graphics) {
    for (int i = graphics->swapchainImageCount - 1; i != -1; i--) {
        vkDestroyFramebuffer(graphics->device, graphics->swapchainFramebuffers[i], NULL);
    }
    hxfFree(graphics->swapchainFramebuffers);

    vkDestroyPipeline(graphics->device, graphics->cubePipeline, NULL);
    vkDestroyPipeline(graphics->device, graphics->iconPipeline, NULL);
    vkDestroyPipelineLayout(graphics->device, graphics->cubePipelineLayout, NULL);
    vkDestroyPipelineLayout(graphics->device, graphics->iconPipelineLayout, NULL);
    vkDestroyRenderPass(graphics->device, graphics->renderPass, NULL);
    vkDestroyPipelineCache(graphics->device, graphics->pipelineCache, NULL);

    vkDestroyDescriptorPool(graphics->device, graphics->cubeDescriptorPool, NULL);
    vkDestroyDescriptorSetLayout(graphics->device, graphics->cubeDescriptorSetLayout, NULL);
    vkDestroyDescriptorPool(graphics->device, graphics->iconDescriptorPool, NULL);
    vkDestroyDescriptorSetLayout(graphics->device, graphics->iconDescriptorSetLayout, NULL);

    for (int i = graphics->swapchainImageCount - 1; i != -1; i--) {
        vkDestroyImageView(graphics->device, graphics->swapchainImageView[i], NULL);
    }
    hxfFree(graphics->swapchainImageView);
    hxfFree(graphics->swapchainImages);
    vkDestroySwapchainKHR(graphics->device, graphics->swapchain, NULL);
    vkDestroySurfaceKHR(graphics->instance, graphics->mainWindowSurface, NULL);

    vkDestroySampler(graphics->device, graphics->drawingData.textureSampler, NULL);
    vkDestroyImage(graphics->device, graphics->drawingData.textureImage, NULL);
    vkDestroyImageView(graphics->device, graphics->drawingData.depthImageView, NULL);
    vkDestroyImage(graphics->device, graphics->drawingData.depthImage, NULL);
    vkDestroyImageView(graphics->device, graphics->drawingData.textureImageView, NULL);

    vkDestroyBuffer(graphics->device, graphics->drawingData.transferBuffer, NULL);
    vkDestroyBuffer(graphics->device, graphics->drawingData.hostBuffer, NULL);
    vkDestroyBuffer(graphics->device, graphics->drawingData.deviceBuffer, NULL);
    vkFreeMemory(graphics->device, graphics->deviceMemory, NULL);
    vkFreeMemory(graphics->device, graphics->hostMemory, NULL);

    vkFreeCommandBuffers(graphics->device, graphics->commandPool, 1, graphics->commandBuffers);
    vkDestroyCommandPool(graphics->device, graphics->commandPool, NULL);

    vkDestroyFence(graphics->device, graphics->fence, NULL);
    for (int i = HXF_MAX_RENDERED_FRAMES - 1; i != -1; i--) {
        vkDestroyFence(graphics->device, graphics->imageRenderedFences[i], NULL);
        vkDestroySemaphore(graphics->device, graphics->nextImageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(graphics->device, graphics->nextImageSubmitedSemaphores[i], NULL);
    }

    vkDestroyDevice(graphics->device, NULL);
    vkDestroyInstance(graphics->instance, NULL);
}

void hxfGraphicsFrame(HxfGraphicsHandler* restrict graphics) {
    vkWaitForFences(graphics->device, 1, &graphics->imageRenderedFences[graphics->currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(graphics->device, 1, &graphics->imageRenderedFences[graphics->currentFrame]);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(graphics->device, graphics->swapchain, UINT64_MAX, graphics->nextImageAvailableSemaphores[graphics->currentFrame], NULL, &imageIndex);

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo = { 0 };
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &graphics->nextImageAvailableSemaphores[graphics->currentFrame];
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &graphics->drawCommandBuffers[graphics->currentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &graphics->nextImageSubmitedSemaphores[graphics->currentFrame];

    if (graphics->camera->isPointingToCube) {
        updatePointedCubeBuffer(graphics);
    }

    // The uniforms buffer must be the last buffer to be updated as other functions may overwrite
    // the memory where the buffer is bound.
    updateMvpBuffer(graphics);

    vkResetCommandBuffer(graphics->drawCommandBuffers[graphics->currentFrame], 0);
    recordDrawCommandBuffer(graphics, imageIndex, graphics->currentFrame);

    HXF_TRY_VK(vkQueueSubmit(graphics->graphicsQueue, 1, &submitInfo, graphics->imageRenderedFences[graphics->currentFrame]));

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &graphics->nextImageSubmitedSemaphores[graphics->currentFrame],
        .swapchainCount = 1,
        .pSwapchains = &graphics->swapchain,
        .pImageIndices = &imageIndex,
    };

    vkQueuePresentKHR(graphics->graphicsQueue, &presentInfo);

    graphics->currentFrame = (graphics->currentFrame + 1) % HXF_MAX_RENDERED_FRAMES;
}

void hxfGraphicsStop(HxfGraphicsHandler* restrict graphics) {
    vkDeviceWaitIdle(graphics->device);
}

void hxfGraphicsUpdateCubeBuffer(HxfGraphicsHandler* restrict graphics) {
    void* data;
    HXF_TRY_VK(vkMapMemory(graphics->device, graphics->hostMemory, 0, graphics->drawingData.cubeInstancesSize, 0, &data));
    memcpy(data, graphics->drawingData.cubeInstances, graphics->drawingData.cubeInstancesSize);
    vkUnmapMemory(graphics->device, graphics->hostMemory);

    transferBuffers(graphics, graphics->drawingData.transferBuffer, graphics->drawingData.deviceBuffer, 0, graphics->drawingData.cubeInstancesOffset - graphics->drawingData.deviceBufferOffset, graphics->drawingData.cubeInstancesSize);
}

void hxfGraphicsUpdateIconBuffer(HxfGraphicsHandler* restrict graphics) {
    void* data;
    HXF_TRY_VK(vkMapMemory(graphics->device, graphics->hostMemory, 0, graphics->drawingData.iconInstancesSize, 0, &data));
    memcpy(data, graphics->drawingData.iconInstances, graphics->drawingData.iconInstancesSize);
    vkUnmapMemory(graphics->device, graphics->hostMemory);

    transferBuffers(graphics, graphics->drawingData.transferBuffer, graphics->drawingData.deviceBuffer, 0, graphics->drawingData.iconInstancesOffset - graphics->drawingData.deviceBufferOffset, graphics->drawingData.iconInstancesSize);
}