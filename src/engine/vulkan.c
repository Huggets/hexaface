#include "vulkan.h"
#include "pipeline.h"
#include "../hxf.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

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
static void computeMemoryNeed(HxfEngine* restrict engine, VkDeviceSize* restrict sizes);

/**
 * @brief Allocate host and device memory, then copy the data to it.
 *
 * @param engine A pointer to the HxfEngine that allocate and hold the memory.
 * @param memorySizes An arary of VkDeviceSize that contains the size of ressources.
 */
static void allocateMemory(HxfEngine* restrict engine, VkDeviceSize* restrict sizes);

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
 * @brief Allocate memory and create the buffers and images.
 *
 * @param engine A pointer to the HxfEngine that will own them.
 */
static void createRessources(HxfEngine* restrict engine);

/**
 * @brief Create the depth image view.
 *
 * @param engine A pointer to the HxfEngine that will own it.
 */
static void createDepthImageView(HxfEngine* restrict engine);

/**
 * @brief Update the uniform buffer object.
 *
 * It includes the view-model-projection matrices.
 *
 * @param engine A pointer the HxfEngine that own the uniform buffer object.
 */
static void updateUniformBufferObject(HxfEngine* restrict engine);

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
    engine->drawCommandBuffers = hxfMalloc(HXF_MAX_RENDERED_FRAMES * sizeof(VkCommandBuffer));

    VkCommandBufferAllocateInfo allocInfo = { 0 };
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = engine->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = HXF_MAX_RENDERED_FRAMES;

    if (vkAllocateCommandBuffers(engine->device, &allocInfo, engine->drawCommandBuffers)) {
        HXF_MSG_ERROR("Could not allocate the command Buffer");
        exit(EXIT_FAILURE);
    }
}

static void recordDrawCommandBuffer(HxfEngine* restrict engine, uint32_t imageIndex, uint32_t currentFrameIndex) {
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };

    VkClearValue clearValue[] = {
        {.color = { { 0.0f, 0.0f, 0.0f, 1.0f }} },
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
        engine->drawingData.vertexDeviceBuffer,
        engine->drawingData.vertexDeviceBuffer
    };
    VkDeviceSize offsets[] = {
        engine->drawingData.cubesVerticesBufferOffset,
        engine->drawingData.cubesBufferOffset
    };
    vkCmdBindVertexBuffers(engine->drawCommandBuffers[currentFrameIndex], 0, 2, boundBuffers, offsets);

    vkCmdBindIndexBuffer(engine->drawCommandBuffers[currentFrameIndex], engine->drawingData.vertexDeviceBuffer, engine->drawingData.cubesVerticesIndexBufferOffset, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(engine->drawCommandBuffers[currentFrameIndex], HXF_INDEX_COUNT, HXF_CUBE_COUNT, 0, 0, 0);
    vkCmdEndRenderPass(engine->drawCommandBuffers[currentFrameIndex]);

    if (vkEndCommandBuffer(engine->drawCommandBuffers[currentFrameIndex])) {
        HXF_MSG_ERROR("Could not record the command buffer");
        exit(EXIT_FAILURE);
    }
}

static void transferBuffers(HxfEngine* restrict engine, VkBuffer src, VkBuffer dst, VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkDeviceSize size) {
    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandBufferCount = 1,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = engine->commandPool
    };
    if (vkAllocateCommandBuffers(engine->device, &allocInfo, &commandBuffer)) {
        HXF_MSG_ERROR("Could not allocate a command buffer to transfer data between buffers");
        exit(EXIT_FAILURE);
    }

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo)) {
        HXF_MSG_ERROR("Could not begin to record the command buffer");
        exit(EXIT_FAILURE);
    }
    VkBufferCopy copyRegion = {
        .dstOffset = srcOffset,
        .srcOffset = dstOffset,
        .size = size
    };
    vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer
    };
    vkQueueSubmit(engine->graphicsQueue, 1, &submitInfo, engine->fence);
    vkWaitForFences(engine->device, 1, &engine->fence, VK_TRUE, UINT64_MAX);
    vkResetFences(engine->device, 1, &engine->fence);

    vkFreeCommandBuffers(engine->device, engine->commandPool, 1, &commandBuffer);
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
    engine->nextImageAvailableSemaphores = hxfMalloc(HXF_MAX_RENDERED_FRAMES * sizeof(VkSemaphore));
    engine->nextImageSubmitedSemaphores = hxfMalloc(HXF_MAX_RENDERED_FRAMES * sizeof(VkSemaphore));
    engine->imageRenderedFences = hxfMalloc(HXF_MAX_RENDERED_FRAMES * sizeof(VkFence));

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


static void computeMemoryNeed(HxfEngine* restrict engine, VkDeviceSize* restrict sizes) {
    // Memory architecture:
    // host memory: host buffer
    // device memory: depth image, vertex device buffer

    // Buffer architecture:
    // host buffer: ubo
    // vertex device buffer: vertex positions, index data, cubes

    VkDeviceSize* hostBufferSizeNeeded = &sizes[0];
    VkDeviceSize* vertexDeviceBufferSizeNeeded = &sizes[1];
    size_t currentBufferSize = 0; ///< Size of the current buffer, this value increase each time data is appended

    /* HOST MEMORY */

    /* host buffer */
    // ubo
    engine->drawingData.uboBufferOffset = 0;
    engine->drawingData.uboBufferSize = sizeof(engine->drawingData.ubo) * HXF_MAX_RENDERED_FRAMES;
    currentBufferSize = engine->drawingData.uboBufferOffset + engine->drawingData.uboBufferSize;

    *hostBufferSizeNeeded = currentBufferSize;

    /* DEVICE MEMORY */

    currentBufferSize = 0;

    // vertex data
    engine->drawingData.cubesVerticesBufferOffset = 0;
    engine->drawingData.cubesVerticesBufferSize = sizeof(engine->drawingData.cubesVertices);
    currentBufferSize =
        engine->drawingData.cubesVerticesBufferOffset
        + engine->drawingData.cubesVerticesBufferSize;

    // index data
    engine->drawingData.cubesVerticesIndexBufferOffset = currentBufferSize;
    engine->drawingData.cubesVerticesIndexBufferSize = sizeof(engine->drawingData.cubesVerticesIndex);
    currentBufferSize =
        engine->drawingData.cubesVerticesIndexBufferOffset
        + engine->drawingData.cubesVerticesIndexBufferSize;

    // cubes
    engine->drawingData.cubesBufferOffset = currentBufferSize;
    engine->drawingData.cubesBufferSize = sizeof(engine->drawingData.cubes);
    currentBufferSize =
        engine->drawingData.cubesBufferOffset
        + engine->drawingData.cubesBufferSize;

    *vertexDeviceBufferSizeNeeded = currentBufferSize;


    // depth image
    VkMemoryRequirements memoryRequirement;
    vkGetImageMemoryRequirements(engine->device, engine->depthImage, &memoryRequirement);

    engine->depthImageMemoryOffset = 0;
    engine->depthImageMemorySize = memoryRequirement.size;

    /* MEMORY OFFSET */

    engine->drawingData.hostBufferMemoryOffset = 0;
    engine->drawingData.vertexDeviceBufferMemoryOffset = engine->depthImageMemoryOffset + engine->depthImageMemorySize;
}

static void allocateMemory(HxfEngine* restrict engine, VkDeviceSize* restrict sizes) {
    const VkDeviceSize* const restrict hostBufferSize = &sizes[0];
    const VkDeviceSize* const restrict vertexBufferSize = &sizes[1];

    // Create two buffers that will transfer the data from the host memory to the device memory
    VkBuffer srcBuffer;
    VkBuffer dstBuffer;
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &engine->graphicsQueueFamilyIndex,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .size = *vertexBufferSize,
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

    VkMemoryRequirements hostBufferSizeRequired;
    vkGetBufferMemoryRequirements(engine->device, engine->drawingData.hostBuffer, &hostBufferSizeRequired);
    const VkDeviceSize hostMemorySize = max(hostBufferSizeRequired.size, *vertexBufferSize);

    VkBuffer requiredBuffers[] = {
        engine->drawingData.hostBuffer,
        srcBuffer
    };
    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = hostMemorySize,
        .memoryTypeIndex = getMemoryTypeIndex(engine, 2, requiredBuffers, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
    };
    if (vkAllocateMemory(engine->device, &allocInfo, NULL, &engine->hostMemory)) {
        HXF_MSG_ERROR("Could not allocate host memory");
        exit(EXIT_FAILURE);
    }

    // Allocate the device memory

    requiredBuffers[0] = engine->drawingData.vertexDeviceBuffer;
    requiredBuffers[1] = dstBuffer;
    allocInfo.allocationSize = engine->depthImageMemorySize + *vertexBufferSize; // Depth image + vertex buffer
    allocInfo.memoryTypeIndex = getMemoryTypeIndex(engine, 2, requiredBuffers, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (vkAllocateMemory(engine->device, &allocInfo, NULL, &engine->deviceMemory)) {
        HXF_MSG_ERROR("Could not allocate device memory");
        exit(EXIT_FAILURE);
    }

    // Bind all the buffers

    vkBindBufferMemory(engine->device, srcBuffer, engine->hostMemory, 0);
    vkBindBufferMemory(engine->device, dstBuffer, engine->deviceMemory, engine->drawingData.vertexDeviceBufferMemoryOffset);

    vkBindBufferMemory(engine->device, engine->drawingData.hostBuffer, engine->hostMemory, 0);
    vkBindBufferMemory(engine->device, engine->drawingData.vertexDeviceBuffer, engine->deviceMemory, engine->drawingData.vertexDeviceBufferMemoryOffset);

    // Write to host memory the data that will be transfered

    void* data;
    if (vkMapMemory(engine->device, engine->hostMemory, 0, *vertexBufferSize, 0, &data)) {
        HXF_MSG_ERROR("Could not map memory");
        exit(EXIT_FAILURE);
    }
    memcpy(data, engine->drawingData.cubesVertices, engine->drawingData.cubesVerticesBufferSize);
    memcpy(data + engine->drawingData.cubesVerticesIndexBufferOffset, engine->drawingData.cubesVerticesIndex, engine->drawingData.cubesVerticesIndexBufferSize);
    memcpy(data + engine->drawingData.cubesBufferOffset, engine->drawingData.cubes, engine->drawingData.cubesBufferSize);
    vkUnmapMemory(engine->device, engine->hostMemory);

    // Transfer the data

    transferBuffers(engine, srcBuffer, dstBuffer, 0, 0, *vertexBufferSize);

    vkDestroyBuffer(engine->device, srcBuffer, NULL);
    vkDestroyBuffer(engine->device, dstBuffer, NULL);

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
    if (vkCreateBuffer(engine->device, &bufferInfo, NULL, &engine->drawingData.vertexDeviceBuffer)) {
        HXF_MSG_ERROR("Could not create the vertex device buffer");
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

static void createDepthImageView(HxfEngine* restrict engine) {
    vkBindImageMemory(engine->device, engine->depthImage, engine->deviceMemory, engine->depthImageMemoryOffset);

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
}

static void createRessources(HxfEngine* restrict engine) {
    // First create the depth image to get its memory needs
    createDepthImage(engine);

    VkDeviceSize sizes[2] = {};
    computeMemoryNeed(engine, sizes);

    const VkDeviceSize hostBufferSizeNeeded = sizes[0];
    const VkDeviceSize vertexDeviceBufferSizeNeeded = sizes[1];
    VkDeviceSize bufferSizes[2] = {
        hostBufferSizeNeeded,
        vertexDeviceBufferSizeNeeded
    };

    createBuffers(engine, bufferSizes);
    allocateMemory(engine, bufferSizes);

    createDepthImageView(engine);
}

static void updateUniformBufferObject(HxfEngine* restrict engine) {
    HxfVec3 normalizedDirection = hxfVec3Normalize(&engine->camera->direction);
    engine->drawingData.ubo.view = hxfViewMatrix(
        &engine->camera->position,
        &normalizedDirection,
        &engine->camera->up
    );

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
    hxfFree(engine->descriptorSets);
    vkDestroyDescriptorSetLayout(engine->device, engine->descriptorSetLayout, NULL);

    for (int i = engine->swapchainImageCount - 1; i != -1; i--) {
        vkDestroyImageView(engine->device, engine->swapchainImageViews[i], NULL);
    }
    hxfFree(engine->swapchainImageViews);
    hxfFree(engine->swapchainImages);
    vkDestroySwapchainKHR(engine->device, engine->swapchain, NULL);
    vkDestroySurfaceKHR(engine->instance, engine->mainWindowSurface, NULL);

    vkDestroyImageView(engine->device, engine->depthImageView, NULL);
    vkDestroyImage(engine->device, engine->depthImage, NULL);

    vkDestroyBuffer(engine->device, engine->drawingData.vertexDeviceBuffer, NULL);
    vkDestroyBuffer(engine->device, engine->drawingData.hostBuffer, NULL);
    vkFreeMemory(engine->device, engine->deviceMemory, NULL);
    vkFreeMemory(engine->device, engine->hostMemory, NULL);

    vkFreeCommandBuffers(engine->device, engine->commandPool, 1, engine->drawCommandBuffers);
    hxfFree(engine->drawCommandBuffers);
    vkDestroyCommandPool(engine->device, engine->commandPool, NULL);

    vkDestroyFence(engine->device, engine->fence, NULL);
    for (int i = HXF_MAX_RENDERED_FRAMES - 1; i != -1; i--) {
        vkDestroyFence(engine->device, engine->imageRenderedFences[i], NULL);
        vkDestroySemaphore(engine->device, engine->nextImageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(engine->device, engine->nextImageSubmitedSemaphores[i], NULL);
    }
    hxfFree(engine->imageRenderedFences);
    hxfFree(engine->nextImageSubmitedSemaphores);
    hxfFree(engine->nextImageAvailableSemaphores);

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