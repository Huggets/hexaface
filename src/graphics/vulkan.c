#include "vulkan.h"
#include <stdlib.h>
#include "../window.h"
#include <string.h>
#include <time.h>

#include <math.h> // TODO useful??

#ifdef HXF_VALIDATION_LAYERS
/**
 * \def VALIDATION_LAYER_COUNT
 * \brief The number of validation layers needed.
 */
#define VALIDATION_LAYER_COUNT 1
#endif



/**
 * \struct QueueFamilyIndices
 * \brief Used to find a suitable GPU that has the required queue families.
 */
typedef struct QueueFamilyIndices {
    uint32_t graphicsFamily;
    int hasGraphicsFamily;
    uint32_t presentFamily;
    int hasPresentFamily;
} QueueFamilyIndices;

/**
 * \struct SwapchainSupportDetails
 * \brief Describe informations about a swapchain.
 */
typedef struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR * formats;
    uint32_t formatSize;
    VkPresentModeKHR * presentModes;
    uint32_t presentModeSize;
} SwapchainSupportDetails;

#ifdef HXF_VALIDATION_LAYERS
/**
 * \brief The list of the validation layers needed.
 */
static const char * validationLayers[VALIDATION_LAYER_COUNT] = {
    "VK_LAYER_KHRONOS_validation"
};
#endif

/**
 * \brief The number of enabled device extensions.
 */
static const int deviceExtensionCount = 1;

/**
 * \brief The needed device extensions.
 */
static const char * deviceExtensions[1] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef HXF_VALIDATION_LAYERS
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
    void * pUserData
) {
    fprintf(stderr, "validation layer: %s\n", pCallbackData->pMessage);

    return VK_FALSE;
}

static VkResult createDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo,
    const VkAllocationCallbacks * pAllocator,
    VkDebugUtilsMessengerEXT * pDebugMessenger
) {
    PFN_vkCreateDebugUtilsMessengerEXT func =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void destroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks * pAllocator
) {
    PFN_vkDestroyDebugUtilsMessengerEXT func =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debugMessenger, pAllocator);
    }
}

static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT * createInfo) {
    createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo->pNext = NULL;
    createInfo->flags = 0;
    createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo->pfnUserCallback = debugCallback;
    createInfo->pUserData = NULL;
}

static void setupDebugMessenger(HxfVulkanInstance * instance) {
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(&createInfo);

    if (createDebugUtilsMessengerEXT(instance->instance, &createInfo, NULL, &instance->debugMessenger)) {
        HXF_MSG_ERROR("failed to set up debug messenger");
        exit(EXIT_FAILURE);
    }
}
#endif

/**
 * \return the version of vulkan supported by the implementation.
 */
static uint32_t getInstanceVersion() {
    // If vkEnumerateInstanceVersion is unavailable then it's 1.0
    if (vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceVersion") == NULL) {
        return VK_API_VERSION_1_0;
    }

    // Otherwise we call this function
    uint32_t version;
    vkEnumerateInstanceVersion(&version);

    return version;
}

/**
 * \brief Check if the extensions are all supported by Vulkan.
 *
 * If that is not the case, then it prints the incompatible extensions and exits the program.
 */
static void checkExtensionSupport(char ** extensions, int count) {
#ifdef HXF_VALIDATION_LAYERS
    const char validationLayerName[] = "VK_LAYER_KHRONOS_validation";
    uint32_t globalExtensionCount;
    vkEnumerateInstanceExtensionProperties(NULL, &globalExtensionCount, NULL);
    uint32_t validationLayersExtensionCount;
    vkEnumerateInstanceExtensionProperties(validationLayerName, &validationLayersExtensionCount, NULL);
    uint32_t extensionCount = globalExtensionCount + validationLayersExtensionCount;

    VkExtensionProperties * instanceExtensions =
        hxfMalloc(sizeof(VkExtensionProperties) * extensionCount);

    vkEnumerateInstanceExtensionProperties(NULL, &globalExtensionCount, instanceExtensions);
    vkEnumerateInstanceExtensionProperties(validationLayerName, &validationLayersExtensionCount,
        instanceExtensions + globalExtensionCount);
#else
    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);

    VkExtensionProperties * instanceExtensions = hxfMalloc(sizeof(VkExtensionProperties) * extensionCount);
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, instanceExtensions);
#endif

    int unsupported = 0;
    int i = 0;
    while (i != count) {
        int extensionFound = 0;
        int j = 0;
        while (j != extensionCount && !extensionFound) {
            if (strcmp(extensions[i], instanceExtensions[j].extensionName) == 0) {
                extensionFound = 1;
            }
            j++;
        }

        if (!extensionFound) {
            unsupported = 1;
            HXF_MSG_ERROR("extension %s not supported", extensions[i]);
        }

        i++;
    }

    free(instanceExtensions);

    if (unsupported) {
        exit(EXIT_FAILURE);
    }
}

#ifdef HXF_VALIDATION_LAYERS
/**
 * \brief Check if the validation layer are supported, and exit the program if they aren't
 */
static void checkValidationLayerSupport(const char * validationLayers[], int count) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    VkLayerProperties * availableLayers = hxfMalloc(sizeof(VkLayerProperties) * layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    int supported = 1;
    int i = 0;
    while (i != count && supported) {
        int layerFound = 0;
        int j = 0;
        while (j != layerCount && !layerFound) {
            if (strcmp(validationLayers[i], availableLayers[j].layerName) == 0) {
                layerFound = 1;
            }
            j++;
        }

        // If the layer is not found, then validation layers are not supported so we exit from the loop
        supported = layerFound;

        i++;
    }

    free(availableLayers);

    if (!supported) {
        HXF_MSG_ERROR("validation layers requested, but not available!");
        exit(EXIT_FAILURE);
    }
}
#endif

/**
 * \return the list of the required extensions for Vulkan.
 *
 * Note: extensions need to be freed when not needed anymore.
 */
static void getRequiredExtensions(char *** extensions, size_t * count) {
#ifdef HXF_VALIDATION_LAYERS
    *count = HXF_WINDOW_REQUIRED_VULKAN_EXTENSIONS_COUNT + 2;
    *extensions = malloc(sizeof(char *) * (HXF_WINDOW_REQUIRED_VULKAN_EXTENSIONS_COUNT + 2));
    hxfGetRequiredWindowVulkanExtension(extensions);

    (*extensions)[*count - 2] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    (*extensions)[*count - 1] = VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME;
#else
    *count = HXF_WINDOW_REQUIRED_VULKAN_EXTENSIONS_COUNT;
    *extensions = malloc(sizeof(char) * HXF_WINDOW_REQUIRED_VULKAN_EXTENSIONS_COUNT);
    hxfGetRequiredWindowVulkanExtension(extensions);
#endif
}

static uint32_t findMemoryType(HxfVulkanInstance * instance, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(instance->physicalDevice, &memProperties);

    for (uint32_t i = 0; i != memProperties.memoryTypeCount; i++) {
        if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    HXF_MSG_ERROR("failed to find suitable memory type");
    exit(EXIT_FAILURE);

    return 0; // Never execute, just here to avoid compiler warning
}

static VkFormat findSupportedFormat(
    HxfVulkanInstance * instance,
    const VkFormat * candidates, size_t candidatesCount,
    VkImageTiling tiling,
    VkFormatFeatureFlags features
) {
    for (int i = 0; i != candidatesCount; i++) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(instance->physicalDevice, candidates[i], &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return candidates[i];
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return candidates[i];
        }
    }

    HXF_MSG_ERROR("failed to find supported format");
    exit(EXIT_FAILURE);

    return -1; // To avoid compiler warning
}

static VkFormat findDepthFormat(HxfVulkanInstance * instance) {
    VkFormat formats[3] = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
    return findSupportedFormat(
        instance,
        formats,
        3,
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

static void createBuffer(
    HxfVulkanInstance * instance,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer * buffer,
    VkDeviceMemory * bufferMemory
) {
    VkBufferCreateInfo bufferInfo;
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = NULL;
    bufferInfo.flags = 0;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.queueFamilyIndexCount = 0;
    bufferInfo.pQueueFamilyIndices = NULL;
    
    if (vkCreateBuffer(instance->device, &bufferInfo, NULL, buffer)) {
        HXF_MSG_ERROR("failed to create buffer");
        exit(EXIT_FAILURE);
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(instance->device, *buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(instance, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(instance->device, &allocInfo, NULL, bufferMemory)) {
        HXF_MSG_ERROR("failed to allocate buffer memory");
        exit(EXIT_FAILURE);
    }

    vkBindBufferMemory(instance->device, *buffer, *bufferMemory, 0);
}

static void copyBuffer(HxfVulkanInstance * instance, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = instance->commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(instance->device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion;
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.pWaitDstStageMask = NULL;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = NULL;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = NULL;

    vkQueueSubmit(instance->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(instance->graphicsQueue);

    vkFreeCommandBuffers(instance->device, instance->commandPool, 1, &commandBuffer);
}

static void createImage(
    HxfVulkanInstance * instance,
    uint32_t width, uint32_t height,
    VkFormat format,
    VkImageTiling tiling, VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImage * image,
    VkDeviceMemory * imageMemory
) {
        VkImageCreateInfo imageInfo = {0};                                                                                  
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;                                                          
        imageInfo.imageType = VK_IMAGE_TYPE_2D;                                                                         
        imageInfo.extent.width = width;                                                                                 
        imageInfo.extent.height = height;                                                                               
        imageInfo.extent.depth = 1;                                                                                     
        imageInfo.mipLevels = 1;                                                                                        
        imageInfo.arrayLayers = 1;                                                                                      
        imageInfo.format = format;                                                                                      
        imageInfo.tiling = tiling;                                                                                      
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;                                                            
        imageInfo.usage = usage;                                                                                        
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;                                                                      
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;                                                              
                                                                                                                        
        if (vkCreateImage(instance->device, &imageInfo, NULL, image)) {                                         
            HXF_MSG_ERROR("failed to create image!");                                                   
            exit(EXIT_FAILURE);
        }                                                                                                               
                                                                                                                        
        VkMemoryRequirements memRequirements;                                                                           
        vkGetImageMemoryRequirements(instance->device, *image, &memRequirements);                                                  
                                                                                                                        
        VkMemoryAllocateInfo allocInfo = {0};                                                                               
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;                                                       
        allocInfo.allocationSize = memRequirements.size;                                                                
        allocInfo.memoryTypeIndex = findMemoryType(instance, memRequirements.memoryTypeBits, properties);                         
                                                                                                                        
        if (vkAllocateMemory(instance->device, &allocInfo, NULL, imageMemory)) {                                
            HXF_MSG_ERROR("failed to allocate image memory!");    
            exit(EXIT_FAILURE);                                           
        }                                                                                                               
                                                                                                                        
        vkBindImageMemory(instance->device, *image, *imageMemory, 0); 
}

static VkImageView createImageView(HxfVulkanInstance * instance, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {                                                       
    VkImageViewCreateInfo viewInfo = {0};                                                                               
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;                                                      
    viewInfo.image = image;                                                                                         
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;                                                                      
    viewInfo.format = format;                                                                                       
    viewInfo.subresourceRange.aspectMask = aspectFlags;                                               
    viewInfo.subresourceRange.baseMipLevel = 0;                                                                     
    viewInfo.subresourceRange.levelCount = 1;                                                                       
    viewInfo.subresourceRange.baseArrayLayer = 0;                                                                   
    viewInfo.subresourceRange.layerCount = 1;                                                                       
                                                                                                                    
    VkImageView imageView;                                                                                          
    if (vkCreateImageView(instance->device, &viewInfo, NULL, &imageView)) {                                  
        HXF_MSG_ERROR("failed to create texture image view!");
        exit(EXIT_FAILURE);
    }                                                                                                               
                                                                                                                    
    return imageView;                                                                                               
}

/**
 * \return 1 if it has it, 0 otherwise
 */
static int hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

static VkCommandBuffer beginSingleTimeCommands(HxfVulkanInstance * instance) {                                                                         
    VkCommandBufferAllocateInfo allocInfo = {0};                                                                        
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;                                               
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;                                                              
    allocInfo.commandPool = instance->commandPool;                                         
    allocInfo.commandBufferCount = 1;                                                                               
                                                                                                                    
    VkCommandBuffer commandBuffer;                                                                                  
    vkAllocateCommandBuffers(instance->device, &allocInfo, &commandBuffer);                                                   
                                                                                                                    
    VkCommandBufferBeginInfo beginInfo = {0};                                                                           
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;                                                  
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;                                                  
                                                                                                                    
    vkBeginCommandBuffer(commandBuffer, &beginInfo);                                                                
                                                                                                                    
    return commandBuffer;                                                                                           
}

void endSingleTimeCommands(HxfVulkanInstance * instance, VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(instance->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(instance->graphicsQueue);

    vkFreeCommandBuffers(instance->device, instance->commandPool, 1, &commandBuffer);
} 

static void transitionImageLayout(HxfVulkanInstance * instance, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {      
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(instance);

    VkImageMemoryBarrier barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (hasStencilComponent(format)) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;
              
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                                                                                                                    
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else {                                                                                                        
        HXF_MSG_ERROR("unsupported layout transition!");
        exit(EXIT_FAILURE);
    }                                                                                                               

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, NULL,
        0, NULL,                                                                                                 
        1, &barrier                                                                                                 
    );                                                                                                              
                                                                                                                    
    endSingleTimeCommands(instance, commandBuffer);                                                                           
}

static void createInstance(HxfVulkanInstance * instance) {
#ifdef HXF_VALIDATION_LAYERS
    checkValidationLayerSupport(validationLayers, VALIDATION_LAYER_COUNT);
#endif

    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
    appInfo.pApplicationName = "Hexaface";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = getInstanceVersion();

    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = NULL;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;

#ifdef HXF_VALIDATION_LAYERS
    instanceInfo.enabledLayerCount = VALIDATION_LAYER_COUNT;
    instanceInfo.ppEnabledLayerNames = validationLayers;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    populateDebugMessengerCreateInfo(&debugCreateInfo);
    instanceInfo.pNext = &debugCreateInfo;
#else
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = NULL;
#endif

    // Get the required extensions
    int enabledExtensionCount;
    char ** enabledExtensions;
    getRequiredExtensions(&enabledExtensions, (size_t *)&enabledExtensionCount);

    instanceInfo.enabledExtensionCount = enabledExtensionCount;
    instanceInfo.ppEnabledExtensionNames = (const char * const *)enabledExtensions;

    checkExtensionSupport(enabledExtensions, enabledExtensionCount);

    VkResult vkResult = vkCreateInstance(&instanceInfo, NULL, &instance->instance);

    // windowExtensions was allocated by hxfGetRequiredWindowVulkanExtension so we need to free it now
    free(enabledExtensions);

    if (vkResult != VK_SUCCESS) {
        HXF_MSG_ERROR("failed to create instance")
            exit(EXIT_FAILURE);
    }
}

/**
 * \brief Find the queue families of device.
 */
static QueueFamilyIndices findQueueFamilies(HxfVulkanInstance * instance, VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    indices.hasGraphicsFamily = 0;
    indices.hasPresentFamily = 0;

    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
    VkQueueFamilyProperties * queueFamilies = hxfMalloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (int i = 0; i != queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            indices.hasGraphicsFamily = 1;
        }

        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, instance->surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
            indices.hasPresentFamily = 1;
        }

        if (indices.hasGraphicsFamily && indices.hasPresentFamily) {
            break;
        }
    }

    free(queueFamilies);

    return indices;
}

/**
 * \brief check if the device support all the needed extension.
 *
 * \return 1 if it is supported, 0 if not.
 */
static int checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);
    VkExtensionProperties * availableExtensions = hxfMalloc(sizeof(VkExtensionProperties) * extensionCount);
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, availableExtensions);

    free(availableExtensions);

    int supported = 1;
    int i = 0;
    while (i != deviceExtensionCount && supported) {
        int found = 0;
        int j = 0;
        while (j != extensionCount && !found) {
            if (strcmp(deviceExtensions[i], availableExtensions[j].extensionName) == 0) {
                found = 1;
            }
            j++;
        }

        // If it is not found, then the loop end because not all extensions are supported
        supported = found;

        i++;
    }

    return supported;
}

/**
 * Note: If the size of format or present mode are different than zero, it means they have been allocated and thus,
 * the must be freed when not used anymore.
 */
static SwapchainSupportDetails querySwapchainSupport(HxfVulkanInstance * instance, VkPhysicalDevice device) {
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, instance->surface, &details.capabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, instance->surface, &details.formatSize, NULL);
    if (details.formatSize != 0) {
        details.formats = hxfMalloc(sizeof(VkSurfaceFormatKHR) * details.formatSize);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, instance->surface, &details.formatSize, details.formats);
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, instance->surface, &details.presentModeSize, NULL);
    if (details.presentModeSize != 0) {
        details.presentModes = hxfMalloc(sizeof(VkPresentModeKHR) * details.presentModeSize);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, instance->surface, &details.presentModeSize, details.presentModes);
    }

    return details;
}

/**
 * \brief Check if a physical device is suitable with our needs.
 *
 * A device is suitable if the queues support graphics and present on the same queue and if the device extensions are
 * supported.
 *
 * \return 1 if the device is suitable, 0 if not suitable.
 */
static int isDeviceSuitable(HxfVulkanInstance * instance, VkPhysicalDevice device) {
    QueueFamilyIndices indices = findQueueFamilies(instance, device);

    int extensionSupported = checkDeviceExtensionSupport(device);

    int swapchainAdequate = 0;
    if (extensionSupported) {
        SwapchainSupportDetails swapchainSupport = querySwapchainSupport(instance, device);
        swapchainAdequate = swapchainSupport.formatSize != 0 && swapchainSupport.presentModeSize != 0;

        // Deallocation if needed
        if (swapchainSupport.formatSize != 0) {
            free(swapchainSupport.formats);
        }

        if (swapchainSupport.presentModeSize != 0) {
            free(swapchainSupport.presentModes);
        }
    }

    return indices.hasGraphicsFamily && indices.hasPresentFamily && indices.graphicsFamily == indices.presentFamily &&
        extensionSupported &&
        swapchainAdequate;
}

/**
 * \brief Select the physical device that will be used to do the operations.
 */
static void pickPhysicalDevice(HxfVulkanInstance * instance) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance->instance, &deviceCount, NULL);

    if (deviceCount == 0) {
        HXF_MSG_ERROR("failed to find GPUs suitable with Vulkan support!");
        exit(EXIT_FAILURE);
    }

    VkPhysicalDevice * devices = hxfMalloc(sizeof(VkPhysicalDevice) * deviceCount);
    vkEnumeratePhysicalDevices(instance->instance, &deviceCount, devices);

    int i;
    for (i = deviceCount - 1; i != -1; i--) {
        if (isDeviceSuitable(instance, devices[i])) {
            instance->physicalDevice = devices[i];
            break;
        }
    }

    if (i == -1) {
        HXF_MSG_ERROR("failed to find a suitable GPU");
        exit(EXIT_FAILURE);
    }

    free(devices);
}

static void createLogicalDevice(HxfVulkanInstance * instance) {
    QueueFamilyIndices indices = findQueueFamilies(instance, instance->physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext = NULL;
    queueCreateInfo.flags = 0;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily; // This is the same as indices.presentFamily
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = NULL;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = deviceExtensionCount;
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions;
    deviceCreateInfo.pEnabledFeatures = NULL;

    // For compatibility with older implementation, set the enabled layers.
    // But for newer implementation it does nothing.
#ifdef HXF_VALIDATION_LAYERS
    deviceCreateInfo.enabledLayerCount = VALIDATION_LAYER_COUNT;
    deviceCreateInfo.ppEnabledLayerNames = validationLayers;
#else
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = NULL;
#endif

    if (vkCreateDevice(instance->physicalDevice, &deviceCreateInfo, NULL, &instance->device)) {
        HXF_MSG_ERROR("failed to create logical device");
        exit(EXIT_FAILURE);
    }

    vkGetDeviceQueue(instance->device, indices.graphicsFamily, 0, &instance->graphicsQueue);
    vkGetDeviceQueue(instance->device, indices.presentFamily, 0, &instance->presentQueue);
}

static void createSurface(HxfVulkanInstance * instance) {
    if (hxfCreateVulkanSurface(&instance->window, instance->instance, &instance->surface) == HXF_ERROR) {
        HXF_MSG_ERROR("failed to create the window surface");
        exit(EXIT_FAILURE);
    }
}

static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR * availableFormats, int count) {
    for (int i = 0; i != count; i++) {
        if (
            availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        ) {
            return availableFormats[i];
        }
    }

    return availableFormats[0];
}

static VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR * availablePresentModes, int count) {
    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D chooseSwapchainExtent(HxfVulkanInstance * instance, VkSurfaceCapabilitiesKHR capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        hxfUpdateWindowInformation(&instance->window);
        VkExtent2D actualExtent = {
            instance->window.info.height,
            instance->window.info.width
        };

        if (actualExtent.height < capabilities.minImageExtent.height)
        {
            actualExtent.height = capabilities.minImageExtent.height;
        }
        else if (actualExtent.height > capabilities.maxImageExtent.height) {
            actualExtent.height = capabilities.maxImageExtent.height;
        }

        if (actualExtent.width < capabilities.minImageExtent.width)
        {
            actualExtent.width = capabilities.minImageExtent.width;
        }
        else if (actualExtent.width > capabilities.maxImageExtent.width) {
            actualExtent.width = capabilities.maxImageExtent.width;
        }

        return actualExtent;
    }
}

static void createSwapchain(HxfVulkanInstance * instance) {
    SwapchainSupportDetails swapchainSupport = querySwapchainSupport(instance, instance->physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats, swapchainSupport.formatSize);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainSupport.presentModes, swapchainSupport.presentModeSize);
    VkExtent2D extent = chooseSwapchainExtent(instance, swapchainSupport.capabilities);

    free(swapchainSupport.formats);
    free(swapchainSupport.presentModes);

    uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
    if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
        imageCount = swapchainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.surface = instance->surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    // The present and graphics queue are the same so we write this
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = NULL;
    createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(instance->device, &createInfo, NULL, &instance->swapchain)) {
        HXF_MSG_ERROR("failed to create the swapchain");
        exit(EXIT_FAILURE);
    }

    vkGetSwapchainImagesKHR(instance->device, instance->swapchain, &instance->swapchainImageCount, NULL);
    instance->swapchainImages = hxfMalloc(sizeof(VkImage) * instance->swapchainImageCount);
    vkGetSwapchainImagesKHR(instance->device, instance->swapchain, &instance->swapchainImageCount, instance->swapchainImages);

    instance->swapchainImageFormat = surfaceFormat.format;
    instance->swapchainExtent = extent;
}

static void createImageViews(HxfVulkanInstance * instance) {
    instance->swapchainImageViews = hxfMalloc(sizeof(VkImageView) * instance->swapchainImageCount);

    for (size_t i = 0; i < instance->swapchainImageCount; i++) {
        VkImageViewCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.pNext = NULL;
        createInfo.flags = 0;
        createInfo.image = instance->swapchainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = instance->swapchainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(instance->device, &createInfo, NULL, &instance->swapchainImageViews[i])) {
            HXF_MSG_ERROR("failed to create image views");
            exit(EXIT_FAILURE);
        }
    }
}

static VkShaderModule createShaderModule(HxfVulkanInstance * instance, const void * code, size_t size) {
    VkShaderModuleCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.codeSize = size;
    createInfo.pCode = code;

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(instance->device, &createInfo, NULL, &shaderModule)) {
        HXF_MSG_ERROR("failed to create shader module");
        exit(EXIT_FAILURE);
    }

    return shaderModule;
}

static void createGraphicsPipeline(HxfVulkanInstance * instance) {
    void * vertShaderCode;
    void * fragShaderCode;
    size_t vertShaderCodeSize;
    size_t fragShaderCodeSize;

    if (
        readFile("../data/shaders/vert.spv", &vertShaderCode, &vertShaderCodeSize) == HXF_ERROR ||
        readFile("../data/shaders/frag.spv", &fragShaderCode, &fragShaderCodeSize) == HXF_ERROR
    ) {
        HXF_MSG_ERROR("failed to read shaders file in data/shaders/");
        exit(EXIT_FAILURE);
    };

    VkShaderModule vertShaderModule = createShaderModule(instance, vertShaderCode, vertShaderCodeSize);
    VkShaderModule fragShaderModule = createShaderModule(instance, fragShaderCode, fragShaderCodeSize);

    free(vertShaderCode);
    free(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo;
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.pNext = NULL;
    vertShaderStageInfo.flags = 0;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";
    vertShaderStageInfo.pSpecializationInfo = NULL;

    VkPipelineShaderStageCreateInfo fragShaderStageInfo;
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.pNext = NULL;
    fragShaderStageInfo.flags = 0;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";
    fragShaderStageInfo.pSpecializationInfo = NULL;

    VkPipelineShaderStageCreateInfo shaderStages[2] = {
        vertShaderStageInfo,
        fragShaderStageInfo
    };

    VkVertexInputBindingDescription * bindingDescriptions = hxfVertexGetBindingDescriptions();
    VkVertexInputAttributeDescription * attributeDescriptions = hxfVertexGetAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = NULL;
    vertexInputInfo.flags = 0;
    vertexInputInfo.vertexBindingDescriptionCount = HXF_VERTEX_BINDING_DESCRIPTION_COUNT;
    vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions;
    vertexInputInfo.vertexAttributeDescriptionCount = HXF_VERTEX_ATTRIBUTE_DESCRIPTION_COUNT;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.pNext = NULL;
    inputAssembly.flags = 0;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)instance->swapchainExtent.width;
    viewport.height = (float)instance->swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = instance->swapchainExtent;

    VkPipelineViewportStateCreateInfo viewportState;
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = NULL;
    viewportState.flags = 0;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer;
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.pNext = NULL;
    rasterizer.flags = 0;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampling;
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.pNext = NULL;
    multisampling.flags = 0;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = NULL;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending;
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext = NULL;
    colorBlending.flags = 0;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineDepthStencilStateCreateInfo depthStencil = {0};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo;
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pNext = NULL;
    pipelineLayoutInfo.flags = 0;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &instance->descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = NULL;

    if (vkCreatePipelineLayout(instance->device, &pipelineLayoutInfo, NULL, &instance->pipelineLayout)) {
        HXF_MSG_ERROR("failed to create pipeline layout");
        exit(EXIT_FAILURE);
    }

    VkGraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = NULL;
    pipelineInfo.flags = 0;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = NULL;
    pipelineInfo.layout = instance->pipelineLayout;
    pipelineInfo.renderPass = instance->renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(instance->device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &instance->graphicsPipeline)) {
        HXF_MSG_ERROR("failed to create graphics pipeline");
        exit(EXIT_FAILURE);
    }

    free(attributeDescriptions);
    free(bindingDescriptions);

    vkDestroyShaderModule(instance->device, fragShaderModule, NULL);
    vkDestroyShaderModule(instance->device, vertShaderModule, NULL);
}

static void createDepthRessources(HxfVulkanInstance * instance) {
    VkFormat depthFormat = findDepthFormat(instance);

    createImage(
        instance,
        instance->swapchainExtent.width, instance->swapchainExtent.height, depthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &instance->depthImage, &instance->depthImageMemory);

    instance->depthImageView = createImageView(instance, instance->depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    transitionImageLayout(
        instance,
        instance->depthImage,
        depthFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

static void createRenderPass(HxfVulkanInstance * instance) {
    VkAttachmentDescription colorAttachment;
    colorAttachment.flags = 0;
    colorAttachment.format = instance->swapchainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef;
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment = {0};
    depthAttachment.format = findDepthFormat(instance);
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {0};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription attachments[2] = { colorAttachment, depthAttachment };

    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency;
    dependency.dependencyFlags = 0;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo;
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.pNext = NULL;
    renderPassInfo.flags = 0;
    renderPassInfo.attachmentCount = 2;
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(instance->device, &renderPassInfo, NULL, &instance->renderPass)) {
        HXF_MSG_ERROR("failed to create render pass");
        exit(EXIT_FAILURE);
    }
}

static void createFramebuffers(HxfVulkanInstance * instance) {
    instance->swapchainFrameBuffers = hxfMalloc(sizeof(VkFramebuffer) * instance->swapchainImageCount);

    for (int i = 0; i != instance->swapchainImageCount; i++) {
        VkImageView attachments[2] = {
            instance->swapchainImageViews[i],
            instance->depthImageView
        };

        VkFramebufferCreateInfo framebufferInfo;
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.pNext = NULL;
        framebufferInfo.flags = 0;
        framebufferInfo.renderPass = instance->renderPass;
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = instance->swapchainExtent.width;
        framebufferInfo.height = instance->swapchainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(instance->device, &framebufferInfo, NULL, &instance->swapchainFrameBuffers[i])) {
            HXF_MSG_ERROR("failed to create framebuffer");
            exit(EXIT_FAILURE);
        }
    }
}

static void createCommandPool(HxfVulkanInstance * instance) {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(instance, instance->physicalDevice);

    VkCommandPoolCreateInfo poolInfo;
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.pNext = NULL;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

    if (vkCreateCommandPool(instance->device, &poolInfo, NULL, &instance->commandPool)) {
        HXF_MSG_ERROR("failed to create command pool");
        exit(EXIT_FAILURE);
    }
}

static void recordCommandBuffer(HxfVulkanInstance * instance, VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo;
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = NULL;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo)) {
        HXF_MSG_ERROR("failed to begin recording comman buffer");
        exit(EXIT_FAILURE);
    }

    VkRenderPassBeginInfo renderPassInfo;
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.pNext = NULL;
    renderPassInfo.renderPass = instance->renderPass;
    renderPassInfo.framebuffer = instance->swapchainFrameBuffers[imageIndex];
    renderPassInfo.renderArea.offset.x = 0;
    renderPassInfo.renderArea.offset.y = 0;
    renderPassInfo.renderArea.extent = instance->swapchainExtent;

    VkClearValue clearValues[2] = {0};
    clearValues[0].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = (VkClearDepthStencilValue){1.0f, 0};
    
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, instance->graphicsPipeline);

    const VkDeviceSize offsetsBinding0[] = { 0 };
    const VkDeviceSize offsetsBinding1[] = { sizeof(HxfVertex) * HXF_VERTEX_COUNT };
    const VkDeviceSize indexOffset =
        sizeof(HxfVertex) * HXF_VERTEX_COUNT + sizeof(HxfVertexInstanceData) * HXF_INSTANCE_COUNT;
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &instance->buffer, offsetsBinding0);
    vkCmdBindVertexBuffers(commandBuffer, 1, 1, &instance->buffer, offsetsBinding1);
    vkCmdBindIndexBuffer(commandBuffer, instance->buffer, indexOffset, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, instance->pipelineLayout, 0, 1,
        &instance->descriptorSets[instance->currentFrame], 0, NULL);

    vkCmdDrawIndexed(commandBuffer, (uint32_t)HXF_INDICE_COUNT, HXF_INSTANCE_COUNT, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer)) {
        HXF_MSG_ERROR("failed to record command buffer");
        exit(EXIT_FAILURE);
    }
}

static void createCommandBuffers(HxfVulkanInstance * instance) {
    instance->commandBuffers = hxfMalloc(sizeof(VkCommandBuffer) * HXF_MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.commandPool = instance->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)HXF_MAX_FRAMES_IN_FLIGHT;

    if (vkAllocateCommandBuffers(instance->device, &allocInfo, instance->commandBuffers)) {
        HXF_MSG_ERROR("failed to allocate command buffers");
        exit(EXIT_FAILURE);
    }
}

static void createSyncObjects(HxfVulkanInstance * instance) {
    instance->imageAvailableSemaphores = hxfMalloc(sizeof(VkSemaphore) * HXF_MAX_FRAMES_IN_FLIGHT);
    instance->renderFinishedSemaphores = hxfMalloc(sizeof(VkSemaphore) * HXF_MAX_FRAMES_IN_FLIGHT);
    instance->inFlightFences = hxfMalloc(sizeof(VkFence) * HXF_MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo;
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = NULL;
    semaphoreInfo.flags = 0;

    VkFenceCreateInfo fenceInfo;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = NULL;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i != HXF_MAX_FRAMES_IN_FLIGHT; i++) {
        if (
            vkCreateSemaphore(instance->device, &semaphoreInfo, NULL, &instance->imageAvailableSemaphores[i]) ||
            vkCreateSemaphore(instance->device, &semaphoreInfo, NULL, &instance->renderFinishedSemaphores[i]) ||
            vkCreateFence(instance->device, &fenceInfo, NULL, &instance->inFlightFences[i])
            ) {
            HXF_MSG_ERROR("failed to create sync objects");
            exit(EXIT_FAILURE);
        }
    }
}

static void cleanupSwapchain(HxfVulkanInstance * instance) {
    vkDestroyImageView(instance->device, instance->depthImageView, NULL);
    vkDestroyImage(instance->device, instance->depthImage, NULL);
    vkFreeMemory(instance->device, instance->depthImageMemory, NULL);

    for (int i = 0; i != instance->swapchainImageCount; i++) {
        vkDestroyFramebuffer(instance->device, instance->swapchainFrameBuffers[i], NULL);
    }

    free(instance->swapchainFrameBuffers);

    vkDestroyPipeline(instance->device, instance->graphicsPipeline, NULL);
    vkDestroyPipelineLayout(instance->device, instance->pipelineLayout, NULL);
    vkDestroyRenderPass(instance->device, instance->renderPass, NULL);

    for (int i = 0; i != instance->swapchainImageCount; i++) {
        vkDestroyImageView(instance->device, instance->swapchainImageViews[i], NULL);
    }
    free(instance->swapchainImageViews);
    free(instance->swapchainImages);

    vkDestroySwapchainKHR(instance->device, instance->swapchain, NULL);
}

static void recreateSwapchain(HxfVulkanInstance * instance) {
    vkDeviceWaitIdle(instance->device);

    cleanupSwapchain(instance);

    createSwapchain(instance);
    createImageViews(instance);
    createRenderPass(instance);
    createGraphicsPipeline(instance);
    createDepthRessources(instance);
    createFramebuffers(instance);
}

static void updateUniformBuffer(HxfVulkanInstance * instance, uint32_t currentImage) {
    hxfUpdateWindowInformation(&instance->window); // The latest information on the width and height

    HxfVec3 direction = {
        cosf(instance->yaw) * cosf(instance->pitch),
        sin(instance->pitch),
        sin(instance->yaw) * cos(instance->pitch)
    };
    instance->front = hxfVec3Normalize(direction);

    HxfUniformBufferObject ubo = {
        HXF_MAT4_IDENTITY,
        hxfViewMatrix(
            instance->pos,
            instance->front,
            (HxfVec3){0.f, -1.f, 0.f}),
        hxfPerspectiveProjection(0.1f, 10.f, 1.0472f, (float)instance->window.info.width / (float)instance->window.info.height)
    };

    ubo.model = hxfMat4MulMat(ubo.model, hxfMat4ScaleMatrix((HxfVec3){0.25f, 0.25f, 0.25f}));
    ubo.model = hxfMat4MulMat(ubo.model, hxfMat4TranslationMatrix((HxfVec3){0.f, 0.0f, -0.5f}));

    void * data;
    vkMapMemory(instance->device, instance->uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(instance->device, instance->uniformBuffersMemory[currentImage]);
}

static void drawFrame(HxfVulkanInstance * instance) {
    vkWaitForFences(instance->device, 1, &instance->inFlightFences[instance->currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(instance->device, instance->swapchain, UINT64_MAX,
        instance->imageAvailableSemaphores[instance->currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapchain(instance);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        HXF_MSG_ERROR("failed to acquire swapchain image");
        exit(EXIT_FAILURE);
    }

    vkResetFences(instance->device, 1, &instance->inFlightFences[instance->currentFrame]);

    vkResetCommandBuffer(instance->commandBuffers[instance->currentFrame], 0);
    recordCommandBuffer(instance, instance->commandBuffers[instance->currentFrame], imageIndex);

    updateUniformBuffer(instance, instance->currentFrame);

    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;

    VkSemaphore waitSemaphores[] = { instance->imageAvailableSemaphores[instance->currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &instance->commandBuffers[instance->currentFrame];

    VkSemaphore signalSemaphores[] = { instance->renderFinishedSemaphores[instance->currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(instance->graphicsQueue, 1, &submitInfo, instance->inFlightFences[instance->currentFrame])) {
        HXF_MSG_ERROR("failed to submit draw command buffer");
        exit(EXIT_FAILURE);
    }

    VkPresentInfoKHR presentInfo;
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = NULL;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapchains[] = { instance->swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = NULL;

    result = vkQueuePresentKHR(instance->presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        recreateSwapchain(instance);
    }
    else if (result != VK_SUCCESS) {
        HXF_MSG_ERROR("failed to present swapchain image");
        exit(EXIT_FAILURE);
    }

    instance->currentFrame = (instance->currentFrame + 1) % HXF_MAX_FRAMES_IN_FLIGHT;
}

/**
 * \brief Create the buffer that hold the vertices data (vertex data, instance data, index).
 */
static void createBuffers(HxfVulkanInstance * instance) {
    const size_t verticesSize = sizeof(HxfVertex) * HXF_VERTEX_COUNT;
    const size_t instanceDataSize = sizeof(HxfVertexInstanceData) * HXF_INSTANCE_COUNT;
    const size_t indicesSize = sizeof(uint32_t) * HXF_INDICE_COUNT;
    VkDeviceSize bufferSize = verticesSize + instanceDataSize + indicesSize;

    // Create the staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer(
        instance,
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &stagingBuffer, &stagingBufferMemory);

    // Map the data
    void * data;
    vkMapMemory(instance->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, instance->vertices, verticesSize);
    memcpy(data + verticesSize, instance->instanceData, instanceDataSize);
    memcpy(data + verticesSize + instanceDataSize, instance->indices, indicesSize);
    vkUnmapMemory(instance->device, stagingBufferMemory);

    // Move the data to a VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT memory
    createBuffer(
        instance,
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &instance->buffer, &instance->bufferMemory);

    copyBuffer(instance, stagingBuffer, instance->buffer, bufferSize);

    vkDestroyBuffer(instance->device, stagingBuffer, NULL);
    vkFreeMemory(instance->device, stagingBufferMemory, NULL);
}

static void createUniformBuffers(HxfVulkanInstance * instance) {
    VkDeviceSize bufferSize = sizeof(HxfUniformBufferObject);

    instance->uniformBuffers = hxfMalloc(sizeof(VkBuffer) * HXF_MAX_FRAMES_IN_FLIGHT);
    instance->uniformBuffersMemory = hxfMalloc(sizeof(VkDeviceMemory) * HXF_MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i != HXF_MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(
            instance,
            bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &instance->uniformBuffers[i], &instance->uniformBuffersMemory[i]);
    }
}

static void createDescriptorSetLayout(HxfVulkanInstance * instance) {
    VkDescriptorSetLayoutBinding uboLayoutBinding = {0};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    
    VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(instance->device, &layoutInfo, NULL, &instance->descriptorSetLayout)) {
        HXF_MSG_ERROR("failed to create descriptor set layout");
        exit(EXIT_FAILURE);
    }
}

static void createDescriptorPool(HxfVulkanInstance * instance) {
    VkDescriptorPoolSize poolSize = {0};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = (uint32_t)HXF_MAX_FRAMES_IN_FLIGHT;

    VkDescriptorPoolCreateInfo poolInfo = {0};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = (uint32_t)HXF_MAX_FRAMES_IN_FLIGHT;

    if (vkCreateDescriptorPool(instance->device, &poolInfo, NULL, &instance->descriptorPool)) {
        HXF_MSG_ERROR("failed to create descriptor pool");
        exit(EXIT_FAILURE);
    }
}

static void createDescriptorSets(HxfVulkanInstance * instance) {
    VkDescriptorSetLayout * layouts = hxfMalloc(sizeof(VkDescriptorSetLayout) * HXF_MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i != HXF_MAX_FRAMES_IN_FLIGHT; i++) {
        layouts[i] = instance->descriptorSetLayout;
    }

    VkDescriptorSetAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = instance->descriptorPool;
    allocInfo.descriptorSetCount = (uint32_t)HXF_MAX_FRAMES_IN_FLIGHT;
    allocInfo.pSetLayouts = layouts;

    instance->descriptorSets = hxfMalloc(sizeof(VkDescriptorSet) * HXF_MAX_FRAMES_IN_FLIGHT);

    if (vkAllocateDescriptorSets(instance->device, &allocInfo, instance->descriptorSets)) {
        HXF_MSG_ERROR("failed to allocate discriptor sets");
        exit(EXIT_FAILURE);
    }

    free(layouts);

    for (size_t i = 0; i != HXF_MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo = {0};
        bufferInfo.buffer = instance->uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(HxfUniformBufferObject);

        VkWriteDescriptorSet descriptorWrite = {0};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = instance->descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(instance->device, 1, &descriptorWrite, 0, NULL);
    }
}

void hxfInitVulkan(HxfVulkanInstance * instance) {
    instance->currentFrame = 0;

    const HxfVec3 red = {1.0f, 0.0f, 0.0f};
    const HxfVec3 green = {0.0f, 1.0f, 0.0f};

    const HxfVertex vertices[HXF_VERTEX_COUNT] = {
        {{-0.5f, -0.5f, -0.5f}, red},
        {{0.5f, -0.5f, -0.5f}, red},
        {{0.5f, -0.5f, 0.5f}, red},
        {{-0.5f, -0.5f, 0.5f}, red},

        {{-0.5f, 0.5f, -0.5f}, green},
        {{0.5f, 0.5f, -0.5f}, green},
        {{0.5f, 0.5f, 0.5f}, green},
        {{-0.5f, 0.5f, 0.5f}, green}
    };

    const uint32_t indices[HXF_INDICE_COUNT] = {
        0, 1, 2, 2, 3, 0, // Top face
        3, 2, 6, 6, 7, 3, // Front face
        7, 6, 5, 5, 4, 7, // Bottom face
        4, 5, 1, 1, 0, 4, // Back face
        7, 4, 0, 0, 3, 7, // Left face
        5, 6, 2, 2, 1, 5  // Right face
    };

    HxfVertexInstanceData instanceData[HXF_INSTANCE_COUNT] = {0};
    for (int i = 0; i != HXF_INSTANCE_COUNT; i++) {
        instanceData[i].offset.x = i;
    }


    for (unsigned int i = HXF_VERTEX_COUNT - 1; i != -1; i--) {
        instance->vertices[i] = vertices[i];
    }
    for (unsigned int i = HXF_INDICE_COUNT - 1; i != -1; i--) {
        instance->indices[i] = indices[i];
    }
    for (unsigned int i = HXF_INSTANCE_COUNT - 1; i != -1; i--) {
        instance->instanceData[i] = instanceData[i];
    }

    if (hxfCreateWindow(&instance->window) == HXF_WINDOW_CREATION_ERROR) {
        HXF_MSG_ERROR("could not create the window");
        exit(EXIT_FAILURE);
    }

    createInstance(instance);
#ifdef HXF_VALIDATION_LAYERS
    setupDebugMessenger(instance);
#endif
    createSurface(instance);
    pickPhysicalDevice(instance);
    createLogicalDevice(instance);
    createSwapchain(instance);
    createImageViews(instance);
    createRenderPass(instance);
    createDescriptorSetLayout(instance);
    createGraphicsPipeline(instance);
    createCommandPool(instance);
    createDepthRessources(instance);
    createFramebuffers(instance);
    createBuffers(instance);
    createUniformBuffers(instance);
    createDescriptorPool(instance);
    createDescriptorSets(instance);
    createCommandBuffers(instance);
    createSyncObjects(instance);
}

void hxfDestroyVulkan(HxfVulkanInstance * instance) {
    cleanupSwapchain(instance);

    for (size_t i = 0; i != HXF_MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(instance->device, instance->uniformBuffers[i], NULL);
        vkFreeMemory(instance->device, instance->uniformBuffersMemory[i], NULL);
    }

    free(instance->uniformBuffers);
    free(instance->uniformBuffersMemory);

    free(instance->descriptorSets);
    vkDestroyDescriptorPool(instance->device, instance->descriptorPool, NULL);

    vkDestroyDescriptorSetLayout(instance->device, instance->descriptorSetLayout, NULL);

    vkDestroyBuffer(instance->device, instance->buffer, NULL);
    vkFreeMemory(instance->device, instance->bufferMemory, NULL);

    for (size_t i = 0; i != HXF_MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(instance->device, instance->imageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(instance->device, instance->renderFinishedSemaphores[i], NULL);
        vkDestroyFence(instance->device, instance->inFlightFences[i], NULL);
    }

    free(instance->commandBuffers);
    vkDestroyCommandPool(instance->device, instance->commandPool, NULL);

    vkDestroySurfaceKHR(instance->instance, instance->surface, NULL);
    vkDestroyDevice(instance->device, NULL);

#ifdef HXF_VALIDATION_LAYERS
    destroyDebugUtilsMessengerEXT(instance->instance, instance->debugMessenger, NULL);
#endif

    vkDestroyInstance(instance->instance, NULL);
}

void hxfRunVulkan(HxfVulkanInstance * instance) {
    int isRunning = 1;

    instance->pitch = 0.0f;
    instance->yaw = -1.57079632f;
    instance->pos = (HxfVec3){0};
    instance->front = (HxfVec3){0};

    const clock_t executionStart = clock();
    clock_t lastTime = executionStart;
    while (isRunning) {
        clock_t startTime = clock();
        instance->lastFrameTime = (startTime - lastTime) / (float)CLOCKS_PER_SEC;
        lastTime = startTime;

        // Read all the events
        while (hxfHasPendingEvents(&instance->window)) {
            HxfEvent event;
            hxfGetNextEvent(&instance->window, &event);

            if (event.type == HXF_EVENT_TYPE_KEYPRESS) {
                switch(event.data) {
                case HXF_EVENT_KEY_ESCAPE:
                    isRunning = 0;
                    break;
                }
            }
            else if (event.type == HXF_EVENT_TYPE_WINDOW_SHOULD_CLOSE) {
                isRunning = 0;
            }
        }

        if (instance->window.keysState.arrowUp) {
            instance->pitch -= instance->lastFrameTime * 150.f;
        }
        if (instance->window.keysState.arrowDown) {
            instance->pitch += instance->lastFrameTime * 150.f;
        }
        if (instance->window.keysState.arrowLeft) {
            instance->yaw += instance->lastFrameTime * 150.f;
        }
        if (instance->window.keysState.arrowRight) {
            instance->yaw -= instance->lastFrameTime * 150.f;
        }

        if (instance->window.keysState.w) {
            HxfVec3 tmp = instance->front;
            tmp.y = 0.0f;
            tmp = hxfVec3Normalize(tmp);
            float inc = instance->lastFrameTime * 50.0f;
            tmp.x *= inc;
            tmp.y *= inc;
            tmp.z *= inc;

            instance->pos = hxfVec3Add(instance->pos, tmp);
        }
        if (instance->window.keysState.s) {
            HxfVec3 tmp = instance->front;
            tmp.y = 0.0f;
            tmp = hxfVec3Normalize(tmp);
            float inc = instance->lastFrameTime * 50.0f;
            tmp.x *= -inc;
            tmp.y *= -inc;
            tmp.z *= -inc;

            instance->pos = hxfVec3Add(instance->pos, tmp);
        }
        if (instance->window.keysState.q) {
            HxfVec3 tmp = hxfVec3Normalize(hxfVec3Cross(instance->front, (HxfVec3){0.f, -1.f, 0.f}));
            float inc = instance->lastFrameTime * 50.0f;
            tmp.x *= -inc;
            tmp.y *= -inc;
            tmp.z *= -inc;

            instance->pos = hxfVec3Add(instance->pos, tmp);
        }
        if (instance->window.keysState.d) {
            HxfVec3 tmp = hxfVec3Normalize(hxfVec3Cross(instance->front, (HxfVec3){0.f, -1.f, 0.f}));
            float inc = instance->lastFrameTime * 50.0f;
            tmp.x *= inc;
            tmp.y *= inc;
            tmp.z *= inc;

            instance->pos = hxfVec3Add(instance->pos, tmp);
        }

        drawFrame(instance);
    }

    vkDeviceWaitIdle(instance->device);
}