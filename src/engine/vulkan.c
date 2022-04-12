#include "vulkan.h"
#include "pipeline.h"
#include "../hxf.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

#include <stb/stb_image.h>

typedef struct TextureImageInfo {
    stbi_uc* pixels;
    int width;
    int height;
    int channels;
} TextureImageInfo;

/*
STATIC FORWARD DECLARATION
*/

/**
 * @brief Create a the vulkan instance.
 *
 * @param engine A pointer to the HxfEngine that will own it.
 */
static void createInstance(HxfEngine* restrict engine);

/**
 * @brief Get a physical device, create the logical device and the queue that are needed.
 *
 * @param engine A pointer to the HxfEngine that will own them.
 */
static void createDevice(HxfEngine* restrict engine);

/**
 * @brief Create the semaphores and the fences.
 *
 * @param engine A pointer to the HxfEngine that will own them.
 */
static void createSyncObjects(HxfEngine* restrict engine);

/**
 * @brief Create the surface.
 *
 * @param engine A pointer to the HxfEngine that will own it.
 */
static void createSurface(HxfEngine* restrict engine);

/**
 * @brief Create the swapchain.
 *
 * It gets the swapchain extent, the swapchain image format, the swapchain image count,
 * the swapchain images and create the swachain image views and the swapchain.
 *
 * @param engine A pointer to the HxfEngine that will own it.
 */
static void createSwapchain(HxfEngine* restrict engine);

/**
 * @brief Create the frame buffers in which the image views of the swapchain
 * will be attached.
 *
 * @param engine A pointer to the HxfEngine that will own them.
 */
static void createFramebuffers(HxfEngine* restrict engine);

/**
 * @brief Create the command pool.
 *
 * @param engine A pointer to the HxfEngine that will own it.
 */
static void createCommandPool(HxfEngine* restrict engine);

/**
 * @brief Create the command buffers.
 *
 * @param engine A pointer to the HxfEngine that will own them.
 */
static void createCommandBuffers(HxfEngine* restrict engine);

/**
 * @brief Record the draw command buffer for the current frame.
 *
 * @param engine A pointer to the HxfEngine that owns it.
 * @param imageIndex The index of the image that will be used by the command buffer.
 * @param currentFrameIndex The index of the frame that is currently rendered.
 */
static void recordDrawCommandBuffer(HxfEngine* restrict engine, uint32_t imageIndex, uint32_t currentFrameIndex);

/**
 * @brief Transfer src buffer data to dst buffer.
 *
 * @param engine A pointer to the HxfEngine that own the buffers.
 * @param src The source buffer.
 * @param dst The destination buffer.
 * @param srcOffset Offset inside src where the copy start.
 * @param dstOffset Offset inside dst where the copy start.
 * @param size The size of the data to transfer.
 */
static void transferBuffers(HxfEngine* restrict engine, VkBuffer src, VkBuffer dst, VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkDeviceSize size);

/**
 * @brief Determine the highest vulkan API version available.
 *
 * @return The highest version available.
 */
static uint32_t determineApiVersion();

/**
 * @brief Get the index of the memory type that have the needed property and that
 * is compatible with the buffers.
 *
 * @param engine A pointer to the HxfEngine that own the buffers and the memory.
 * @param bufferCount The number of buffers.
 * @param buffers An array of buffers that need to be compatible with the memory.
 * @param propertyNeeded The property that the memory must have
 *
 * @return The memory type index if no error, otherwise the program exit with a failure.
 */
static uint32_t getMemoryTypeIndex(HxfEngine* restrict engine, size_t bufferCount, const VkBuffer* buffers, uint32_t propertyNeeded);

/**
 * @brief Get the limits of the physical device.
 *
 * @param engine A pointer to the HxfEngine that own the physical device.
 */
static void getVulkanLimits(HxfEngine* restrict engine);

/**
 * @brief Compute data needed for the device and host memory.
 *
 * For example, image offset and data sizes and offsets inside the memory
 *
 * @param engine A pointer to the HxfEngine that hold the memory.
 * @param sizes An array of 2 VkDeviceSize that will be modified.
 */
static void computeMemoryNeeds(HxfEngine* restrict engine, VkDeviceSize* restrict sizes);

static void transferTextureImage(HxfEngine* restrict engine, TextureImageInfo* restrict textureInfo);

/**
 * @brief Allocate host and device memory, then copy the data to it.
 *
 * @param engine A pointer to the HxfEngine that allocate and hold the memory.
 * @param memorySizes An arary of VkDeviceSize that contains the size of ressources.
 */
static void allocateMemory(HxfEngine* restrict engine, VkDeviceSize* restrict sizes, TextureImageInfo* restrict textureInfo);

/**
 * @brief Create the buffers.
 *
 * @param engine A pointer to the HxfEngine that will own them.
 * @param bufferSize An array containing the size of each buffer.
 */
static void createBuffers(HxfEngine* restrict engine, const VkDeviceSize* restrict bufferSizes);

/**
 * @brief Create the depth image.
 *
 * @param engine A pointer to the HxfEngine that will own it.
 */
static void createDepthImage(HxfEngine* restrict engine);

/**
 * @brief Create all texture images.
 *
 * @param engine A pointer to the HxfEngine that will own them.
 */
static void createTextureImages(HxfEngine* restrict engine, TextureImageInfo* restrict textureInfo);

/**
 * @brief Create all the image views.
 *
 * @param engine A pointer to the HxfEngine that will own them.
 */
static void createImageViews(HxfEngine* restrict engine);

/**
 * @brief Create the sampler for the texture images.
 *
 * @param engine A pointer to the HxfEngine that own the texture images and that will own the sampler.
 */
static void createTextureSampler(HxfEngine* restrict engine);

/**
 * @brief Allocate memory and create the buffers and images.
 *
 * @param engine A pointer to the HxfEngine that will own them.
 */
static void createRessources(HxfEngine* restrict engine);

/**
 * @brief Update the uniform buffer object.
 *
 * It includes the view-model-projection matrices.
 *
 * @param engine A pointer to the HxfEngine that own the uniform buffer object.
 */
static void updateUniformBufferObject(HxfEngine* restrict engine);

/**
 * @brief Update the buffer that hold the pointed cube.
 *
 * @param engine A pointer to the HxfEngine that hold them.
 */
static void updatePointedCubeBuffer(HxfEngine* restrict engine);

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

static uint32_t getMemoryTypeIndex(HxfEngine* restrict engine, size_t bufferCount, const VkBuffer* buffers, uint32_t propertyNeeded) {
    VkMemoryRequirements memoryRequirements;
    uint32_t memoryTypeBits = 0;
    for (int i = 0; i != bufferCount; i++) {
        vkGetBufferMemoryRequirements(engine->device, buffers[i], &memoryRequirements);
        memoryTypeBits = memoryTypeBits | memoryRequirements.memoryTypeBits;
    }

    uint32_t memoryTypeIndex = 0;
    int notFound = 1;
    while (memoryTypeIndex != engine->physicalDeviceMemoryProperties.memoryTypeCount && notFound) {
        if ((memoryTypeBits & (1 << memoryTypeIndex)) && (engine->physicalDeviceMemoryProperties.memoryTypes[memoryTypeIndex].propertyFlags & propertyNeeded) == propertyNeeded) {
            notFound = 0;
        }
        memoryTypeIndex++;
    }

    if (notFound) {
        HXF_MSG_ERROR("Could not find an appropriate memory type");
        exit(EXIT_FAILURE);
    }

    return memoryTypeIndex;
}

static void getVulkanLimits(HxfEngine* restrict engine) {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(engine->physicalDevice, &props);

    vkGetPhysicalDeviceMemoryProperties(engine->physicalDevice, &engine->physicalDeviceMemoryProperties);

    engine->physicalDeviceLimits = props.limits;
}

static void createInstance(HxfEngine* restrict engine) {
    VkApplicationInfo appInfo = { 0 };
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hexaface";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.apiVersion = determineApiVersion();

    VkInstanceCreateInfo info = { 0 };
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pApplicationInfo = &appInfo;

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
        HXF_MSG_ERROR("The required instance extensions are not all available");
        exit(EXIT_FAILURE);
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
        HXF_MSG_ERROR("The layer VK_LAYER_KHRONOS_validation is unavailable");
        exit(EXIT_FAILURE);
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
        HXF_MSG_ERROR("The extension " VK_EXT_DEBUG_UTILS_EXTENSION_NAME " is unavailable");
        exit(EXIT_FAILURE);
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

    if (vkCreateInstance(&info, NULL, &engine->instance)) {
        HXF_MSG_ERROR("Could not create the instance");
        exit(EXIT_FAILURE);
    }

    hxfFree(requiredExtensions);
}

static void createDevice(HxfEngine* restrict engine) {
    // Choose a physical device
    uint32_t count;
    vkEnumeratePhysicalDevices(engine->instance, &count, NULL);

    if (count == 0) {
        HXF_MSG_ERROR("Could not find a device that support vulkan");
        exit(EXIT_FAILURE);
    }

    // Take the first device
    VkPhysicalDevice* physicalDevices = hxfMalloc(count * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(engine->instance, &count, physicalDevices);
    engine->physicalDevice = physicalDevices[0];
    hxfFree(physicalDevices);

    // Choose the queue that will be used
    float queuePriorities[] = { 1.f };
    VkDeviceQueueCreateInfo queueInfo = { 0 };
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queuePriorities;

    vkGetPhysicalDeviceQueueFamilyProperties(engine->physicalDevice, &count, NULL);
    VkQueueFamilyProperties* props = hxfMalloc(count * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(engine->physicalDevice, &count, props);
    int queueNotFound = 1;
    int i = count - 1;
    while (i != -1 && queueNotFound) {
        if (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueNotFound = 0;
            queueInfo.queueFamilyIndex = i;
            engine->graphicsQueueFamilyIndex = i;
        }
        i--;
    }

    hxfFree(props);

    if (queueNotFound) {
        HXF_MSG_ERROR("No graphics queue found");
        exit(EXIT_FAILURE);
    }

    // Verify the extensions are available
    const char* const enabledExtensions[] = { "VK_KHR_swapchain" };
    const uint32_t enabledExtensionCount = 1;

    vkEnumerateDeviceExtensionProperties(engine->physicalDevice, NULL, &count, NULL);
    VkExtensionProperties* extensionProperties = hxfMalloc(count * sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(engine->physicalDevice, NULL, &count, extensionProperties);

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
        HXF_MSG_ERROR("Not all the required device extensions are available");
        exit(EXIT_FAILURE);
    }

    // Create the logical device
    VkDeviceCreateInfo deviceInfo = { 0 };
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledExtensionCount = enabledExtensionCount;
    deviceInfo.ppEnabledExtensionNames = enabledExtensions;

    if (vkCreateDevice(engine->physicalDevice, &deviceInfo, NULL, &engine->device)) {
        HXF_MSG_ERROR("Could not create the logical device");
        exit(EXIT_FAILURE);
    }

    // Get the graphics queue
    vkGetDeviceQueue(engine->device, queueInfo.queueFamilyIndex, 0, &engine->graphicsQueue);
}

static void createCommandPool(HxfEngine* restrict engine) {
    VkCommandPoolCreateInfo poolInfo = { 0 };
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = engine->graphicsQueueFamilyIndex;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(engine->device, &poolInfo, NULL, &engine->commandPool)) {
        HXF_MSG_ERROR("Could not create command pool");
        exit(EXIT_FAILURE);
    }
}

static void createCommandBuffers(HxfEngine* restrict engine) {
    engine->drawCommandBuffers = &engine->commandBuffers[0];
    engine->transferCommandBuffer = &engine->commandBuffers[HXF_MAX_RENDERED_FRAMES];

    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = engine->commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = sizeof(engine->commandBuffers) / sizeof(VkCommandBuffer)
    };
    if (vkAllocateCommandBuffers(engine->device, &allocInfo, engine->commandBuffers)) {
        HXF_MSG_ERROR("Could not allocate the command Buffer");
        exit(EXIT_FAILURE);
    }
}

static void recordDrawCommandBuffer(HxfEngine* restrict engine, uint32_t imageIndex, uint32_t currentFrameIndex) {
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };

    VkClearValue clearValue[] = {
        {.color = { { 16.0f / 255.0f, 154.0f / 255.0f, 209.0f / 255.0f, 1.0f }} },
        {.depthStencil = { 1.0f, 0.0f } }
    };

    VkRenderPassBeginInfo renderPassBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = engine->renderPass,
        .renderArea.offset.x = 0,
        .renderArea.offset.y = 0,
        .renderArea.extent = engine->swapchainExtent,
        .clearValueCount = 2,
        .pClearValues = clearValue,
        .framebuffer = engine->swapchainFramebuffers[imageIndex],
    };

    if (vkBeginCommandBuffer(engine->drawCommandBuffers[currentFrameIndex], &beginInfo)) {
        HXF_MSG_ERROR("Could not begin command buffer recording");
        exit(EXIT_FAILURE);
    }

    vkCmdBeginRenderPass(engine->drawCommandBuffers[currentFrameIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(engine->drawCommandBuffers[currentFrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, engine->graphicsPipeline);
    vkCmdBindDescriptorSets(
        engine->drawCommandBuffers[currentFrameIndex],
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        engine->graphicsPipelineLayout,
        0, 1, &engine->descriptorSets[currentFrameIndex],
        0, NULL
    );
    VkBuffer boundBuffers[] = {
        engine->drawingData.deviceBuffer,
        engine->drawingData.deviceBuffer
    };
    VkDeviceSize offsets[] = {
        engine->drawingData.cubesVerticesBufferOffset,
        engine->drawingData.facesBufferOffset
    };
    vkCmdBindVertexBuffers(engine->drawCommandBuffers[currentFrameIndex], 0, 2, boundBuffers, offsets);
    vkCmdBindIndexBuffer(engine->drawCommandBuffers[currentFrameIndex], engine->drawingData.deviceBuffer, engine->drawingData.cubesVertexIndicesBufferOffset, VK_INDEX_TYPE_UINT32);

    // The pointed cube

    if (engine->camera->isPointingToCube) {
        vkCmdDrawIndexed(engine->drawCommandBuffers[currentFrameIndex], HXF_INDEX_COUNT, 1, 0, 0, HXF_CUBE_COUNT * 6);
    }

    // All the cubes
    // (A draw call for each faces)

    vkCmdDrawIndexed(engine->drawCommandBuffers[currentFrameIndex], 6, engine->drawingData.faceTopCount, 0, 0, 0);
    vkCmdDrawIndexed(engine->drawCommandBuffers[currentFrameIndex], 6, engine->drawingData.faceBackCount, 6, 0, HXF_CUBE_COUNT * 1);
    vkCmdDrawIndexed(engine->drawCommandBuffers[currentFrameIndex], 6, engine->drawingData.faceBottomCount, 12, 0, HXF_CUBE_COUNT * 2);
    vkCmdDrawIndexed(engine->drawCommandBuffers[currentFrameIndex], 6, engine->drawingData.faceFrontCount, 18, 0, HXF_CUBE_COUNT * 3);
    vkCmdDrawIndexed(engine->drawCommandBuffers[currentFrameIndex], 6, engine->drawingData.faceRightCount, 24, 0, HXF_CUBE_COUNT * 4);
    vkCmdDrawIndexed(engine->drawCommandBuffers[currentFrameIndex], 6, engine->drawingData.faceLeftCount, 30, 0, HXF_CUBE_COUNT * 5);

    vkCmdEndRenderPass(engine->drawCommandBuffers[currentFrameIndex]);

    if (vkEndCommandBuffer(engine->drawCommandBuffers[currentFrameIndex])) {
        HXF_MSG_ERROR("Could not record the command buffer");
        exit(EXIT_FAILURE);
    }
}

static void transferBuffers(HxfEngine* restrict engine, VkBuffer src, VkBuffer dst, VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkDeviceSize size) {
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    if (vkBeginCommandBuffer(*engine->transferCommandBuffer, &beginInfo)) {
        HXF_MSG_ERROR("Could not begin to record the command buffer");
        exit(EXIT_FAILURE);
    }
    VkBufferCopy copyRegion = {
        .dstOffset = srcOffset,
        .srcOffset = dstOffset,
        .size = size
    };
    vkCmdCopyBuffer(*engine->transferCommandBuffer, src, dst, 1, &copyRegion);
    vkEndCommandBuffer(*engine->transferCommandBuffer);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = engine->transferCommandBuffer
    };
    vkQueueSubmit(engine->graphicsQueue, 1, &submitInfo, engine->fence);
    vkWaitForFences(engine->device, 1, &engine->fence, VK_TRUE, UINT64_MAX);
    vkResetFences(engine->device, 1, &engine->fence);
}

static void createSurface(HxfEngine* restrict engine) {
    hxfCreateWindowSurface(engine->mainWindow, engine->instance, &engine->mainWindowSurface);

    VkBool32 isSupported;
    vkGetPhysicalDeviceSurfaceSupportKHR(
        engine->physicalDevice,
        engine->graphicsQueueFamilyIndex,
        engine->mainWindowSurface,
        &isSupported);

    if (!isSupported) {
        HXF_MSG_ERROR("The Window System Integration is not supported");
        exit(EXIT_FAILURE);
    }

}

static void getRequiredInstanceExtensions(char*** restrict extensions, int* restrict count) {
    hxfGetRequiredWindowExtensions(extensions, count);
}

static void createSwapchain(HxfEngine* restrict engine) {
    // Get the surfaceInformations
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VkSurfaceFormatKHR* surfaceFormats;
    uint32_t surfaceFormatCount;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(engine->physicalDevice, engine->mainWindowSurface, &surfaceCapabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(engine->physicalDevice, engine->mainWindowSurface, &surfaceFormatCount, NULL);
    surfaceFormats = hxfMalloc(surfaceFormatCount * sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(engine->physicalDevice, engine->mainWindowSurface, &surfaceFormatCount, surfaceFormats);

    // Choose the swapchain image format and colorspace
    engine->swapchainImageFormat = VK_FORMAT_R8G8B8A8_SRGB;
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
        engine->swapchainImageFormat = surfaceFormats[0].format;
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
    engine->swapchainExtent = surfaceCapabilities.currentExtent;

    // Create the swapchain
    VkSwapchainCreateInfoKHR swapchainInfo = { 0 };
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = engine->mainWindowSurface;
    swapchainInfo.minImageCount = imageCount;
    swapchainInfo.imageFormat = engine->swapchainImageFormat;
    swapchainInfo.imageColorSpace = colorSpace;
    swapchainInfo.imageExtent = engine->swapchainExtent;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.preTransform = surfaceCapabilities.currentTransform;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; // Choose FIFO for now as it is always available
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(engine->device, &swapchainInfo, NULL, &engine->swapchain)) {
        HXF_MSG_ERROR("Could not create the swapchain");
        exit(EXIT_FAILURE);
    }

    // Get the swapchain images
    vkGetSwapchainImagesKHR(engine->device, engine->swapchain, &engine->swapchainImageCount, NULL);
    engine->swapchainImages = hxfMalloc(engine->swapchainImageCount * sizeof(VkImage));
    vkGetSwapchainImagesKHR(engine->device, engine->swapchain, &engine->swapchainImageCount, engine->swapchainImages);

    // Create the swapchain image views
    engine->swapchainImageViews = hxfMalloc(engine->swapchainImageCount * sizeof(VkImageView));

    VkImageViewCreateInfo imageViewInfo = { 0 };
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = engine->swapchainImageFormat;
    imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;

    for (int i = engine->swapchainImageCount - 1; i != -1; i--) {
        imageViewInfo.image = engine->swapchainImages[i];

        if (vkCreateImageView(engine->device, &imageViewInfo, NULL, &engine->swapchainImageViews[i])) {
            HXF_MSG_ERROR("Could not create the swapchain image views");
            exit(EXIT_FAILURE);
        }
    }
}

static void createFramebuffers(HxfEngine* restrict engine) {
    engine->swapchainFramebuffers = hxfMalloc(engine->swapchainImageCount * sizeof(VkFramebuffer));

    VkFramebufferCreateInfo framebufferInfo = { 0 };
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = engine->renderPass;
    framebufferInfo.attachmentCount = 2;
    framebufferInfo.width = engine->swapchainExtent.width;
    framebufferInfo.height = engine->swapchainExtent.height;
    framebufferInfo.layers = 1;

    for (int i = engine->swapchainImageCount - 1; i != -1; i--) {
        VkImageView attachments[] = {
            engine->swapchainImageViews[i],
            engine->depthImageView
        };
        framebufferInfo.pAttachments = attachments;

        if (vkCreateFramebuffer(engine->device, &framebufferInfo, NULL, &engine->swapchainFramebuffers[i])) {
            HXF_MSG_ERROR("Could not create the framebuffers");
            exit(EXIT_FAILURE);
        }
    }
}

static void createSyncObjects(HxfEngine* restrict engine) {
    VkSemaphoreCreateInfo semaInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

    VkFenceCreateInfo signaledFenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    VkFenceCreateInfo unsignaledFenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };

    VkResult result = vkCreateFence(engine->device, &unsignaledFenceInfo, NULL, &engine->fence);

    int i = 0;
    while (!result && i != HXF_MAX_RENDERED_FRAMES) {
        result =
            vkCreateSemaphore(engine->device, &semaInfo, NULL, &engine->nextImageAvailableSemaphores[i])
            || vkCreateSemaphore(engine->device, &semaInfo, NULL, &engine->nextImageSubmitedSemaphores[i])
            || vkCreateFence(engine->device, &signaledFenceInfo, NULL, &engine->imageRenderedFences[i]);
        i++;
    }

    if (result) {
        HXF_MSG_ERROR("Could not create the syncronisation objects");
        exit(EXIT_FAILURE);
    }
}


static void computeMemoryNeeds(HxfEngine* restrict engine, VkDeviceSize* restrict sizes) {
    // Memory architecture:
    // host memory: host buffer, + pointed cube staging buffer, faces staging buffer
    // device memory: depth image, textureImages, device buffer

    // Buffer architecture:
    // host buffer: ubo
    // device buffer: vertices, indices, faces + the pointed cube

    VkDeviceSize* hostBufferSizeNeeded = &sizes[0];
    VkDeviceSize* deviceBufferSizeNeeded = &sizes[1];

    size_t currentBufferSize = 0; ///< Size of the current buffer, this value increase each time data is appended

    /* HOST MEMORY */

    /* host buffer */
    // ubo

    engine->drawingData.uboBufferOffset = 0;
    engine->drawingData.uboBufferSize = sizeof(engine->drawingData.ubo) * HXF_MAX_RENDERED_FRAMES;
    currentBufferSize = engine->drawingData.uboBufferOffset + engine->drawingData.uboBufferSize;

    *hostBufferSizeNeeded = currentBufferSize;

    // Pointed cube

    engine->drawingData.pointedCubeHostOffset = currentBufferSize;
    engine->drawingData.pointedCubeSize = sizeof(HxfCubeData);

    // transfer buffer

    engine->drawingData.facesSrcTransferBufferOffset = engine->drawingData.pointedCubeHostOffset + engine->drawingData.pointedCubeSize;

    /* DEVICE MEMORY */

    currentBufferSize = 0;

    // vertices

    engine->drawingData.cubesVerticesBufferOffset = 0;
    engine->drawingData.cubesVerticesBufferSize = sizeof(engine->drawingData.cubesVertices);
    currentBufferSize =
        engine->drawingData.cubesVerticesBufferOffset
        + engine->drawingData.cubesVerticesBufferSize;

    // indices

    engine->drawingData.cubesVertexIndicesBufferOffset = currentBufferSize;
    engine->drawingData.cubesVertexIndicesBufferSize = sizeof(engine->drawingData.cubesVertexIndices);
    currentBufferSize =
        engine->drawingData.cubesVertexIndicesBufferOffset
        + engine->drawingData.cubesVertexIndicesBufferSize;

    // faces

    engine->drawingData.facesBufferOffset = currentBufferSize;
    engine->drawingData.facesBufferSize = sizeof(engine->drawingData.faces);
    currentBufferSize =
        engine->drawingData.facesBufferOffset
        + engine->drawingData.facesBufferSize;

    // pointed cube

    engine->drawingData.pointedCubeDeviceOffset = currentBufferSize;
    currentBufferSize = engine->drawingData.pointedCubeDeviceOffset + engine->drawingData.pointedCubeSize;

    *deviceBufferSizeNeeded = currentBufferSize;

    size_t memoryOffset = 0;
    size_t modulo = 0;
    size_t additionalOffset;

    // depth image

    VkMemoryRequirements memoryRequirement;
    vkGetImageMemoryRequirements(engine->device, engine->depthImage, &memoryRequirement);

    engine->depthImageMemoryOffset = 0;
    engine->depthImageMemorySize = memoryRequirement.size;
    memoryOffset = engine->depthImageMemoryOffset + engine->depthImageMemorySize;

    // texture image

    vkGetImageMemoryRequirements(engine->device, engine->drawingData.textureImage, &memoryRequirement);
    modulo = memoryOffset % memoryRequirement.alignment;
    if (modulo == 0) {
        additionalOffset = 0;
    }
    else {
        additionalOffset = memoryRequirement.alignment - modulo;
    }
    engine->drawingData.textureImageMemoryOffset = memoryOffset + additionalOffset;
    engine->drawingData.textureImageMemorySize = memoryRequirement.size;
    memoryOffset = engine->drawingData.textureImageMemoryOffset + engine->drawingData.textureImageMemorySize;

    /* MEMORY OFFSET */

    engine->drawingData.hostBufferMemoryOffset = 0;
    engine->drawingData.deviceBufferMemoryOffset = memoryOffset;
}

static void transferTextureImage(HxfEngine* restrict engine, TextureImageInfo* restrict textureInfo) {
    // Write the texture data to memory

    VkBuffer srcBuffer;
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &engine->graphicsQueueFamilyIndex,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .size = textureInfo->width * textureInfo->height * textureInfo->channels,
    };
    if (vkCreateBuffer(engine->device, &bufferInfo, NULL, &srcBuffer)) {
        HXF_MSG_ERROR("Could not create the buffer");
        exit(EXIT_FAILURE);
    }

    vkBindBufferMemory(engine->device, srcBuffer, engine->hostMemory, 0);

    void* data;
    if (vkMapMemory(engine->device, engine->hostMemory, 0, engine->drawingData.textureImageMemorySize, 0, &data)) {
        HXF_MSG_ERROR("Could not map memory");
        exit(EXIT_FAILURE);
    }
    memcpy(data, textureInfo->pixels, bufferInfo.size);
    vkUnmapMemory(engine->device, engine->hostMemory);

    // Begin to record the buffer

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    if (vkBeginCommandBuffer(*engine->transferCommandBuffer, &beginInfo)) {
        HXF_MSG_ERROR("Could not begin to record the command buffer");
        exit(EXIT_FAILURE);
    }

    // Transition image layout to be able to transfer it

    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = engine->drawingData.textureImage,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };
    vkCmdPipelineBarrier(
        *engine->transferCommandBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        0, NULL, 0, NULL, 1, &barrier
    );

    // Transfer the texture images to the device memory

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
    vkCmdCopyBufferToImage(*engine->transferCommandBuffer, srcBuffer, engine->drawingData.textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

    // Transition the image layout to be able to use in the shader

    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    vkCmdPipelineBarrier(
        *engine->transferCommandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0,
        0, NULL, 0, NULL, 1, &barrier
    );

    vkEndCommandBuffer(*engine->transferCommandBuffer);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = engine->transferCommandBuffer
    };
    vkQueueSubmit(engine->graphicsQueue, 1, &submitInfo, engine->fence);
    vkWaitForFences(engine->device, 1, &engine->fence, VK_TRUE, UINT64_MAX);
    vkResetFences(engine->device, 1, &engine->fence);

    vkDestroyBuffer(engine->device, srcBuffer, NULL);
}

static void allocateMemory(HxfEngine* restrict engine, VkDeviceSize* restrict sizes, TextureImageInfo* restrict textureInfo) {
    const VkDeviceSize* const restrict hostBufferSize = &sizes[0];
    const VkDeviceSize* const restrict deviceBufferSize = &sizes[1];

    VkMemoryRequirements hostBufferSizeRequired;
    vkGetBufferMemoryRequirements(engine->device, engine->drawingData.hostBuffer, &hostBufferSizeRequired);

    // The amount of memory needed for the host memory.
    const VkDeviceSize hostMemorySizeNeeded = max(hostBufferSizeRequired.size + engine->drawingData.pointedCubeSize + engine->drawingData.facesBufferSize, *deviceBufferSize); // for staging buffer
    // The amount of memory needed for the device memory.
    const VkDeviceSize deviceMemorySizeNeeded = engine->drawingData.deviceBufferMemoryOffset + *deviceBufferSize;

    // Create two buffers that will transfer the data from the host memory to the device memory

    // TODO maybe remove these two buffer and use the transfer buffers (transferSrcBuffer and transferDstBuffer)
    // that are also used for transfering the faces data.

    VkBuffer srcBuffer;
    VkBuffer dstBuffer;
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &engine->graphicsQueueFamilyIndex,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .size = *deviceBufferSize,
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    };
    if (vkCreateBuffer(engine->device, &bufferInfo, NULL, &srcBuffer)) {
        HXF_MSG_ERROR("Could not create the source temporary buffer");
        exit(EXIT_FAILURE);
    }
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    if (vkCreateBuffer(engine->device, &bufferInfo, NULL, &dstBuffer)) {
        HXF_MSG_ERROR("Could not create the destination temporary buffer");
        exit(EXIT_FAILURE);
    }

    // Allocate the host memory.
    // It allocate the maximum between the host buffer required size and the device buffer required size
    // as it will store these two buffers but not at the same time.

    VkBuffer requiredBuffers[] = {
        engine->drawingData.hostBuffer,
        srcBuffer
    };
    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = hostMemorySizeNeeded,
        .memoryTypeIndex = getMemoryTypeIndex(engine, 2, requiredBuffers, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
    };
    if (vkAllocateMemory(engine->device, &allocInfo, NULL, &engine->hostMemory)) {
        HXF_MSG_ERROR("Could not allocate host memory");
        exit(EXIT_FAILURE);
    }

    // Allocate the device memory

    requiredBuffers[0] = engine->drawingData.deviceBuffer;
    requiredBuffers[1] = dstBuffer;
    allocInfo.allocationSize = deviceMemorySizeNeeded; // Depth image + device buffer
    allocInfo.memoryTypeIndex = getMemoryTypeIndex(engine, 2, requiredBuffers, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (vkAllocateMemory(engine->device, &allocInfo, NULL, &engine->deviceMemory)) {
        HXF_MSG_ERROR("Could not allocate device memory");
        exit(EXIT_FAILURE);
    }

    // Bind all the buffers and images

    vkBindBufferMemory(engine->device, srcBuffer, engine->hostMemory, 0);
    vkBindBufferMemory(engine->device, dstBuffer, engine->deviceMemory, engine->drawingData.deviceBufferMemoryOffset);

    vkBindBufferMemory(engine->device, engine->drawingData.hostBuffer, engine->hostMemory, 0);
    vkBindBufferMemory(engine->device, engine->drawingData.deviceBuffer, engine->deviceMemory, engine->drawingData.deviceBufferMemoryOffset);

    vkBindBufferMemory(engine->device, engine->drawingData.facesSrcTransferBuffer, engine->hostMemory, engine->drawingData.facesSrcTransferBufferOffset);
    vkBindBufferMemory(engine->device, engine->drawingData.facesDstTransferBuffer, engine->deviceMemory, engine->drawingData.deviceBufferMemoryOffset + engine->drawingData.facesBufferOffset);

    vkBindImageMemory(engine->device, engine->depthImage, engine->deviceMemory, engine->depthImageMemoryOffset);
    vkBindImageMemory(engine->device, engine->drawingData.textureImage, engine->deviceMemory, engine->drawingData.textureImageMemoryOffset);

    // Transfer the texture images

    transferTextureImage(engine, textureInfo);

    // Write to host memory the data that will be transfered

    void* data;
    if (vkMapMemory(engine->device, engine->hostMemory, 0, *deviceBufferSize, 0, &data)) {
        HXF_MSG_ERROR("Could not map memory");
        exit(EXIT_FAILURE);
    }
    memcpy(data, engine->drawingData.cubesVertices, engine->drawingData.cubesVerticesBufferSize);
    memcpy(data + engine->drawingData.cubesVertexIndicesBufferOffset, engine->drawingData.cubesVertexIndices, engine->drawingData.cubesVertexIndicesBufferSize);
    memcpy(data + engine->drawingData.facesBufferOffset, engine->drawingData.faces, engine->drawingData.facesBufferSize);
    vkUnmapMemory(engine->device, engine->hostMemory);

    // Transfer the data

    transferBuffers(engine, srcBuffer, dstBuffer, 0, 0, *deviceBufferSize);

    vkDestroyBuffer(engine->device, srcBuffer, NULL);
    vkDestroyBuffer(engine->device, dstBuffer, NULL);

    // Write to the host memory the image textures data

    // Write to the device memory the data it actually needs

    if (vkMapMemory(engine->device, engine->hostMemory, 0, *hostBufferSize, 0, &data)) {
        HXF_MSG_ERROR("Could not map memory");
        exit(EXIT_FAILURE);
    }
    memcpy(data, &engine->drawingData.ubo, engine->drawingData.uboBufferSize);
    vkUnmapMemory(engine->device, engine->hostMemory);
}

static void createBuffers(HxfEngine* restrict engine, const VkDeviceSize* restrict bufferSizes) {
    // Host buffer

    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = bufferSizes[0],
        .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &engine->graphicsQueueFamilyIndex,
    };
    if (vkCreateBuffer(engine->device, &bufferInfo, NULL, &engine->drawingData.hostBuffer)) {
        HXF_MSG_ERROR("Could not create the host buffer");
        exit(EXIT_FAILURE);
    }

    // Vertex device buffer

    bufferInfo.size = bufferSizes[1];
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if (vkCreateBuffer(engine->device, &bufferInfo, NULL, &engine->drawingData.deviceBuffer)) {
        HXF_MSG_ERROR("Could not create the vertex device buffer");
        exit(EXIT_FAILURE);
    }

    // faces transfer src

    bufferInfo.size = engine->drawingData.facesBufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    if (vkCreateBuffer(engine->device, &bufferInfo, NULL, &engine->drawingData.facesSrcTransferBuffer)) {
        HXF_MSG_ERROR("Could not create the faces src transfer buffer");
        exit(EXIT_FAILURE);
    }

    // faces transfer dst

    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    if (vkCreateBuffer(engine->device, &bufferInfo, NULL, &engine->drawingData.facesDstTransferBuffer)) {
        HXF_MSG_ERROR("Could not create the faces dst transfer buffer");
        exit(EXIT_FAILURE);
    }
}

static void createTextureImages(HxfEngine* restrict engine, TextureImageInfo* restrict textureInfo) {
    // Load the textures

    char* texturePath = hxfMalloc(sizeof(char) * (strlen(engine->appdataDirectory) + 31));
    strcpy(texturePath, engine->appdataDirectory);
    strcat(texturePath, "/textures/textures.png");

    textureInfo->pixels = stbi_load(texturePath, &textureInfo->width, &textureInfo->height, &textureInfo->channels, STBI_rgb_alpha);

    hxfFree(texturePath);

    if (!textureInfo->pixels) {
        HXF_MSG_ERROR("Could not load texture files");
        exit(EXIT_FAILURE);
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
        .pQueueFamilyIndices = &engine->graphicsQueueFamilyIndex,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };
    if (vkCreateImage(engine->device, &imageInfo, NULL, &engine->drawingData.textureImage)) {
        HXF_MSG_ERROR("Could not create the texture image");
        exit(EXIT_FAILURE);
    }
}

static void createDepthImage(HxfEngine* restrict engine) {
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
        vkGetPhysicalDeviceFormatProperties(engine->physicalDevice, formats[i], &properties);
        if ((properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            engine->depthImageFormat = formats[i];
            notFound = 0;
        }

        i++;
    }

    if (notFound) {
        HXF_MSG_ERROR("No image format found for the depth image");
        exit(EXIT_FAILURE);
    }

    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = engine->depthImageFormat,
        .extent = { engine->mainWindow->width, engine->mainWindow->height, 1 },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &engine->graphicsQueueFamilyIndex,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    if (vkCreateImage(engine->device, &imageInfo, NULL, &engine->depthImage)) {
        HXF_MSG_ERROR("Could not create the depth image");
        exit(EXIT_FAILURE);
    }
}

static void createImageViews(HxfEngine* restrict engine) {
    // depth image view

    VkImageViewCreateInfo imageViewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = engine->depthImage,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = engine->depthImageFormat,
        .components = { 0 },
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };
    if (vkCreateImageView(engine->device, &imageViewInfo, NULL, &engine->depthImageView)) {
        HXF_MSG_ERROR("Could not create the depth image view");
        exit(EXIT_FAILURE);
    }

    // texture images view

    imageViewInfo.image = engine->drawingData.textureImage;
    imageViewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    if (vkCreateImageView(engine->device, &imageViewInfo, NULL, &engine->drawingData.textureImageView)) {
        HXF_MSG_ERROR("Could not create the texture images views");
        exit(EXIT_FAILURE);
    }
}

static void createTextureSampler(HxfEngine* restrict engine) {
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
    if (vkCreateSampler(engine->device, &samplerInfo, NULL, &engine->drawingData.textureSampler)) {
        HXF_MSG_ERROR("Could not create the texture sampler");
        exit(EXIT_FAILURE);
    }
}

static void createRessources(HxfEngine* restrict engine) {
    createDepthImage(engine);

    TextureImageInfo textureInfo;
    createTextureImages(engine, &textureInfo);

    VkDeviceSize sizes[2] = {};
    computeMemoryNeeds(engine, sizes);

    const VkDeviceSize hostBufferSizeNeeded = sizes[0];
    const VkDeviceSize vertexDeviceBufferSizeNeeded = sizes[1];
    VkDeviceSize bufferSizes[2] = {
        hostBufferSizeNeeded,
        vertexDeviceBufferSizeNeeded
    };

    createBuffers(engine, bufferSizes);
    allocateMemory(engine, bufferSizes, &textureInfo);

    createImageViews(engine);
    createTextureSampler(engine);

    stbi_image_free(textureInfo.pixels);
}

static void updateUniformBufferObject(HxfEngine* restrict engine) {
    // Update the view matrix according to the camera

    engine->drawingData.ubo.view = hxfViewMatrix(
        &engine->camera->position,
        &engine->camera->direction,
        &engine->camera->up
    );

    // Update the memory

    void* data;
    vkMapMemory(
        engine->device,
        engine->hostMemory,
        engine->drawingData.uboBufferOffset,
        engine->drawingData.uboBufferSize,
        0,
        &data
    );
    memcpy(data, &engine->drawingData.ubo, engine->drawingData.uboBufferSize);
    vkUnmapMemory(engine->device, engine->hostMemory);
}

static void updatePointedCubeBuffer(HxfEngine* restrict engine) {
    VkBuffer hostBuffer;
    VkBuffer deviceBuffer;

    VkBufferCreateInfo bufferInfo = {
       .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
       .queueFamilyIndexCount = 1,
       .pQueueFamilyIndices = &engine->graphicsQueueFamilyIndex,
       .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
       .size = engine->drawingData.pointedCubeSize,
       .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    };
    if (vkCreateBuffer(engine->device, &bufferInfo, NULL, &hostBuffer)) {
        HXF_MSG_ERROR("Could not create staging buffer");
        exit(EXIT_FAILURE);
    }
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    if (vkCreateBuffer(engine->device, &bufferInfo, NULL, &deviceBuffer)) {
        HXF_MSG_ERROR("Could not create staging buffer");
        exit(EXIT_FAILURE);
    }

    vkBindBufferMemory(engine->device, hostBuffer, engine->hostMemory, engine->drawingData.pointedCubeHostOffset);
    vkBindBufferMemory(engine->device, deviceBuffer, engine->deviceMemory, engine->drawingData.deviceBufferMemoryOffset + engine->drawingData.pointedCubeDeviceOffset);

    void* data;
    if (vkMapMemory(engine->device, engine->hostMemory, engine->drawingData.hostBufferMemoryOffset + engine->drawingData.pointedCubeHostOffset, engine->drawingData.pointedCubeSize, 0, &data)) {
        HXF_MSG_ERROR("Could not map memory");
        exit(EXIT_FAILURE);
    }

    const HxfCubeData pointedCube = {
        { (float)engine->camera->pointedCube.x, (float)engine->camera->pointedCube.y, (float)engine->camera->pointedCube.z },
        0
    };

    memcpy(data, &pointedCube, sizeof(pointedCube));
    vkUnmapMemory(engine->device, engine->hostMemory);

    transferBuffers(engine, hostBuffer, deviceBuffer, 0, 0, sizeof(pointedCube));

    vkDestroyBuffer(engine->device, hostBuffer, NULL);
    vkDestroyBuffer(engine->device, deviceBuffer, NULL);
}

void hxfInitEngine(HxfEngine* restrict engine) {
    createInstance(engine);
    createDevice(engine);
    getVulkanLimits(engine);
    createSyncObjects(engine);
    createCommandPool(engine);
    createCommandBuffers(engine);

    // Create the buffers and the images
    createRessources(engine);

    createSurface(engine);
    createSwapchain(engine);
    createGraphicsPipeline(engine);
    createFramebuffers(engine);
}

void hxfDestroyEngine(HxfEngine* restrict engine) {
    for (int i = engine->swapchainImageCount - 1; i != -1; i--) {
        vkDestroyFramebuffer(engine->device, engine->swapchainFramebuffers[i], NULL);
    }
    hxfFree(engine->swapchainFramebuffers);

    vkDestroyPipeline(engine->device, engine->graphicsPipeline, NULL);
    vkDestroyPipelineLayout(engine->device, engine->graphicsPipelineLayout, NULL);
    vkDestroyRenderPass(engine->device, engine->renderPass, NULL);

    vkDestroyDescriptorPool(engine->device, engine->descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(engine->device, engine->descriptorSetLayout, NULL);

    for (int i = engine->swapchainImageCount - 1; i != -1; i--) {
        vkDestroyImageView(engine->device, engine->swapchainImageViews[i], NULL);
    }
    hxfFree(engine->swapchainImageViews);
    hxfFree(engine->swapchainImages);
    vkDestroySwapchainKHR(engine->device, engine->swapchain, NULL);
    vkDestroySurfaceKHR(engine->instance, engine->mainWindowSurface, NULL);

    vkDestroySampler(engine->device, engine->drawingData.textureSampler, NULL);
    vkDestroyImage(engine->device, engine->drawingData.textureImage, NULL);
    vkDestroyImageView(engine->device, engine->depthImageView, NULL);
    vkDestroyImage(engine->device, engine->depthImage, NULL);
    vkDestroyImageView(engine->device, engine->drawingData.textureImageView, NULL);

    vkDestroyBuffer(engine->device, engine->drawingData.facesSrcTransferBuffer, NULL);
    vkDestroyBuffer(engine->device, engine->drawingData.facesDstTransferBuffer, NULL);
    vkDestroyBuffer(engine->device, engine->drawingData.deviceBuffer, NULL);
    vkDestroyBuffer(engine->device, engine->drawingData.hostBuffer, NULL);
    vkFreeMemory(engine->device, engine->deviceMemory, NULL);
    vkFreeMemory(engine->device, engine->hostMemory, NULL);

    vkFreeCommandBuffers(engine->device, engine->commandPool, 1, engine->commandBuffers);
    vkDestroyCommandPool(engine->device, engine->commandPool, NULL);

    vkDestroyFence(engine->device, engine->fence, NULL);
    for (int i = HXF_MAX_RENDERED_FRAMES - 1; i != -1; i--) {
        vkDestroyFence(engine->device, engine->imageRenderedFences[i], NULL);
        vkDestroySemaphore(engine->device, engine->nextImageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(engine->device, engine->nextImageSubmitedSemaphores[i], NULL);
    }

    vkDestroyDevice(engine->device, NULL);
    vkDestroyInstance(engine->instance, NULL);
}

void hxfEngineFrame(HxfEngine* restrict engine) {
    vkWaitForFences(engine->device, 1, &engine->imageRenderedFences[engine->currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(engine->device, 1, &engine->imageRenderedFences[engine->currentFrame]);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(engine->device, engine->swapchain, UINT64_MAX, engine->nextImageAvailableSemaphores[engine->currentFrame], NULL, &imageIndex);

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo = { 0 };
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &engine->nextImageAvailableSemaphores[engine->currentFrame];
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &engine->drawCommandBuffers[engine->currentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &engine->nextImageSubmitedSemaphores[engine->currentFrame];

    updateUniformBufferObject(engine);

    if (engine->camera->isPointingToCube) {
        updatePointedCubeBuffer(engine);
    }

    vkResetCommandBuffer(engine->drawCommandBuffers[engine->currentFrame], 0);
    recordDrawCommandBuffer(engine, imageIndex, engine->currentFrame);

    if (vkQueueSubmit(engine->graphicsQueue, 1, &submitInfo, engine->imageRenderedFences[engine->currentFrame])) {
        HXF_MSG_ERROR("Could not submit to queue");
        exit(EXIT_FAILURE);
    }

    VkPresentInfoKHR presentInfo = { 0 };
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &engine->nextImageSubmitedSemaphores[engine->currentFrame];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &engine->swapchain;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(engine->graphicsQueue, &presentInfo);

    engine->currentFrame = (engine->currentFrame + 1) % HXF_MAX_RENDERED_FRAMES;
}

void hxfStopEngine(HxfEngine* restrict engine) {
    vkDeviceWaitIdle(engine->device);
}

void hxfEngineUpdateCubeBuffer(HxfEngine* restrict engine) {
    void* data;
    if (vkMapMemory(engine->device, engine->hostMemory, engine->drawingData.facesSrcTransferBufferOffset, engine->drawingData.facesBufferSize, 0, &data)) {
        HXF_MSG_ERROR("Could not map memory");
        exit(EXIT_FAILURE);
    }
    memcpy(data, engine->drawingData.faces, engine->drawingData.facesBufferSize);
    vkUnmapMemory(engine->device, engine->hostMemory);

    transferBuffers(engine, engine->drawingData.facesSrcTransferBuffer, engine->drawingData.facesDstTransferBuffer, 0, 0, engine->drawingData.facesBufferSize);
}
