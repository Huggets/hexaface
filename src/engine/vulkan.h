#pragma once

#include <vulkan/vulkan.h>
#include "../window.h"
#include "../math/linear-algebra.h"
#include "../camera.h"
#include "../input.h"
#include "../world.h"

#include <stdalign.h>

/**
 * @brief The maximum number of frames that can be rendered at the same time.
 */
#define HXF_MAX_RENDERED_FRAMES 2

#define HXF_VERTEX_COUNT 24
#define HXF_INDEX_COUNT 36
#define HXF_CUBE_COUNT HXF_WORLD_LENGTH * HXF_WORLD_LENGTH * HXF_WORLD_LENGTH

typedef struct HxfCubeData {
    HxfVec3 cubePosition;
    HxfVec3 cubeColor;
} HxfCubeData;

typedef struct HxfUniformBufferObject {
    alignas(16) HxfMat4 model;
    alignas(16) HxfMat4 view;
    alignas(16) HxfMat4 projection;
} HxfUniformBufferObject;

/**
 * @brief Contains information on the things that will be drawn.
 *
 * For example, the vertex data, the model, view, projection matrices,
 * the buffer that holds them...
 */
typedef struct HxfDrawingData {
    /**
     * @brief Buffer on the host memory.
     */
    VkBuffer hostBuffer;
    /**
     * @brief Buffer on the local device memory for vertex relative data.
     */
    VkBuffer deviceBuffer;

    /**
     * @brief The vertex position used to draw a cube.
     */
    HxfVec3 cubesVertices[HXF_VERTEX_COUNT];
    /**
     * @brief The index of the vertices that is used to draw a cube.
     */
    uint32_t cubesVertexIndices[HXF_INDEX_COUNT];
    /**
     * @brief The Uniform buffer object of the shaders.
     */
    HxfUniformBufferObject ubo;
    /**
     * @brief Position of each cubes.
     */
    HxfCubeData cubes[HXF_CUBE_COUNT];

    /**
     * @brief The number of cubes to draw.
     */
    size_t cubeCount;

    /**
     * @brief Offset of the vertex positions in the buffer.
     */
    size_t cubesVerticesBufferOffset;
    /**
     * @brief Size of the vertex positions inside the buffer.
     */
    size_t cubesVerticesBufferSize;
    /**
     * @brief Offset of the index data in the buffer.
     */
    size_t cubesVertexIndicesBufferOffset;
    /**
     * @brief Size of the vertex data inside the buffer.
     */
    size_t cubesVertexIndicesBufferSize;
    /**
     * @brief Offset of the ubo in the buffer.
     */
    size_t uboBufferOffset;
    /**
     * @brief Size of the ubo inside the buffer
     */
    size_t uboBufferSize;
    /**
     * @brief Offset of the cubes inside the buffer.
     */
    size_t cubesBufferOffset;
    /**
     * @brief Size of the cubes inside the buffer.
     */
    size_t cubesBufferSize;

    /**
     * @brief Offset of the hostBuffer inside the memory.
     */
    size_t hostBufferMemoryOffset;
    /**
     * @brief Offset of the vertxDeviceBuffer inside the memory.
     */
    size_t deviceBufferMemoryOffset;
} HxfDrawingData;

typedef struct HxfEngine {
    HxfWindow* mainWindow;
    VkSurfaceKHR mainWindowSurface;

    const HxfKeyboardState* keyboardState;
    const HxfCamera* camera;

    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    uint32_t graphicsQueueFamilyIndex;

    VkPhysicalDeviceLimits physicalDeviceLimits;
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

    VkSwapchainKHR swapchain;
    VkImage* swapchainImages; ///< size = swapchainImageCount
    VkImageView* swapchainImageViews; ///< size = swapchainImageCount
    VkFramebuffer* swapchainFramebuffers; ///< size = swapchainImageCount
    uint32_t swapchainImageCount; ///< The number of images of the swapchain
    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;

    VkImage depthImage;
    VkImageView depthImageView;
    VkFormat depthImageFormat;
    size_t depthImageMemoryOffset; ///< Offset of the depth image inside the memory
    size_t depthImageMemorySize; ///< Size of the depth image inside the memory

    VkPipeline graphicsPipeline;
    VkPipelineLayout graphicsPipelineLayout;
    VkRenderPass renderPass;
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet* descriptorSets; ///< size = HXF_MAX_RENDERED_FRAMES

    VkCommandPool commandPool;
    VkCommandBuffer* drawCommandBuffers; ///< size = HXF_MAX_RENDERED_FRAMES

    VkSemaphore* nextImageAvailableSemaphores; ///< size = HXF_MAX_RENDERED_FRAMES
    VkSemaphore* nextImageSubmitedSemaphores; ///< size = HXF_MAX_RENDERED_FRAMES
    VkFence* imageRenderedFences; ///< size = HXF_MAX_RENDERED_FRAMES

    /**
     * @brief Fence that can be used for anything.
     * 
     * It is used when transfering data from one buffer to another.
     */
    VkFence fence;

    VkDeviceMemory hostMemory; ///< Memory that is available on the host
    VkDeviceMemory deviceMemory; ///< Memory only available for the device

    /**
     * @brief Data relative to the drawing.
     * 
     * It contains the vertex position, the index buffer, the MVP matrix...
     */
    HxfDrawingData drawingData;

    uint32_t currentFrame; ///< The index of the frame that is currently rendered
} HxfEngine;

/**
 * @brief Initialize the engine.
 *
 * @param engine The engine to initialize.
 */
void hxfInitEngine(HxfEngine* restrict engine);

/**
 * @brief Destroy the engine.
 *
 * @param engine The engine to destroy.
 */
void hxfDestroyEngine(HxfEngine* restrict engine);

/**
 * @brief Run a single frame of the engine.
 *
 * @param engine The engine that process it.
 */
void hxfEngineFrame(HxfEngine* restrict engine);

/**
 * @brief Stop the engine.
 *
 * End all engine processing then return.
 *
 * @param engine The engine to stop.
 */
void hxfStopEngine(HxfEngine* restrict engine);