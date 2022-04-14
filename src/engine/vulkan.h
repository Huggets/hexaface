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

#define HXF_CUBE_VERTEX_DATA_COUNT 24
#define HXF_CUBE_VERTEX_INDEX_COUNT 36
#define HXF_CUBE_COUNT (HXF_WORLD_LENGTH * HXF_WORLD_LENGTH * HXF_WORLD_LENGTH)

#define HXF_ICON_VERTEX_DATA_COUNT 4
#define HXF_ICON_VERTEX_INDEX_COUNT 6
#define HXF_ICON_INSTANCE_DATA_COUNT 1

#define HXF_TEXTURE_COUNT 5

#define HXF_FACES_TOP 0
#define HXF_FACES_BACK 1
#define HXF_FACES_BOTTOM 2
#define HXF_FACES_FRONT 3
#define HXF_FACES_RIGHT 4
#define HXF_FACES_LEFT 5

typedef struct HxfCubeData {
    alignas(16) HxfVec3 position;
    alignas(4)  uint32_t textureIndex;
} HxfCubeData;

typedef struct HxfVertexData {
    alignas(16) HxfVec3 position;
    alignas(8)  HxfVec2 texelCoordinate;
    alignas(4)  uint32_t textureIndex;
} HxfVertexData;

typedef struct HxfUniformBufferObject {
    alignas(16) HxfMat4 model;
    alignas(16) HxfMat4 view;
    alignas(16) HxfMat4 projection;
} HxfUniformBufferObject;

typedef struct HxfIconVertexData {
    alignas(8) HxfVec2 position;
    alignas(8) HxfVec2 texelCoordinate;
} HxfIconVertexData;

typedef struct HxfIconInstanceData {
    alignas(4) uint32_t textureIndex;
} HxfIconInstanceData;

typedef struct HxfIconPushData {
    alignas(4) uint32_t windowWidth;
    alignas(4) uint32_t windowHeight;
} HxfIconPushData;

/**
 * @brief Contains information on the things that will be drawn.
 *
 * For example, the vertex data, the model, view, projection matrices,
 * the buffer that holds them...
 */
typedef struct HxfDrawingData {
    VkBuffer hostBuffer; ///< Buffer on the host memory.
    VkBuffer cubeBuffer; ///< Buffer that contains the cubes data
    VkBuffer iconBuffer; ///< Buffer that contains the icons data
    VkBuffer facesSrcTransferBuffer;
    VkBuffer facesDstTransferBuffer;
    VkBuffer pointedCubeSrcBuffer;
    VkBuffer pointedCubeDstBuffer;

    /**
     * @brief The texture images.
     *
     * For example the grass texture.
     */
    VkImage textureImage;
    VkImageView textureImageView;
    VkSampler textureSampler;

    /**
     * @brief The vertex position used to draw a cube.
     */
    HxfVertexData cubesVertices[HXF_CUBE_VERTEX_DATA_COUNT];
    /**
     * @brief The index of the vertices that is used to draw a cube.
     */
    uint32_t cubesVertexIndices[HXF_CUBE_VERTEX_INDEX_COUNT];
    /**
     * @brief The Uniform buffer object of the shaders.
     */
    HxfUniformBufferObject ubo;
    /**
     * @brief Data for each faces of the cubes.
     *
     * Top, back, bottom, front, right, left.
     */
    HxfCubeData faces[6][HXF_CUBE_COUNT];

    HxfIconVertexData iconVertices[HXF_ICON_VERTEX_DATA_COUNT];
    HxfIconInstanceData iconInstances[HXF_ICON_INSTANCE_DATA_COUNT];
    HxfIconPushData iconPush;
    uint32_t iconVertexIndices[HXF_ICON_VERTEX_INDEX_COUNT];

    /**
     * @brief The number of front face to draw.
     */
    size_t faceFrontCount;
    /**
     * @brief The number of back face to draw.
     */
    size_t faceBackCount;
    /**
     * @brief The number of top face to draw.
     */
    size_t faceTopCount;
    /**
     * @brief The number of bottom face to draw.
     */
    size_t faceBottomCount;
    /**
     * @brief The number of right face to draw.
     */
    size_t faceRightCount;
    /**
     * @brief The number of left face to draw.
     */
    size_t faceLeftCount;

    // Memory offsets

    size_t cubesVerticesOffset;
    size_t cubesVerticesSize;
    size_t cubesVertexIndicesOffset;
    size_t cubesVertexIndicesSize;
    size_t uboOffset;
    size_t uboSize;
    size_t facesOffset;
    size_t facesSize;
    size_t pointedCubeHostOffset;
    size_t pointedCubeDeviceOffset;
    size_t pointedCubeSize;
    size_t textureImageOffset;
    size_t textureImageSize;
    size_t iconVerticesOffset;
    size_t iconVerticesSize;
    size_t iconInstanceOffset;
    size_t iconInstanceSize;

    size_t hostBufferOffset;
    size_t deviceBufferOffset;
    size_t facesSrcTransferBufferOffset;
    size_t iconBufferOffset;
} HxfDrawingData;

typedef struct HxfEngine {
    HxfWindow* mainWindow;
    VkSurfaceKHR mainWindowSurface;

    const char* const appdataDirectory;
    const HxfKeyboardState* keyboardState;
    const HxfCamera* camera;
    const HxfWorld* world;

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
    size_t depthImageOffset; ///< Offset of the depth image inside the memory
    size_t depthImageSize; ///< Size of the depth image inside the memory

    VkPipelineCache pipelineCache;
    VkPipeline cubePipeline;
    VkPipeline iconPipeline;
    VkPipelineLayout cubePipelineLayout;
    VkPipelineLayout iconPipelineLayout;
    VkRenderPass renderPass;
    VkDescriptorPool cubeDescriptorPool;
    VkDescriptorPool iconDescriptorPool;
    VkDescriptorSetLayout cubeDescriptorSetLayout;
    VkDescriptorSetLayout iconDescriptorSetLayout;
    VkDescriptorSet cubeDescriptorSets[HXF_MAX_RENDERED_FRAMES];
    VkDescriptorSet iconDescriptorSets[HXF_MAX_RENDERED_FRAMES];

    VkCommandPool commandPool;
    /**
     * @brief All the command buffers allocated from the commandPool.
     *
     * List of the command buffers:
     * - HXF_MAX_RENDERED_FRAMES draw command buffers
     * - 1 transfer command buffer
     */
    VkCommandBuffer commandBuffers[HXF_MAX_RENDERED_FRAMES + 1];
    /**
     * @brief A pointer to the first draw command buffer.
     */
    VkCommandBuffer* drawCommandBuffers;
    /**
     * @brief A pointer to the transferCommandBuffer.
     */
    VkCommandBuffer* transferCommandBuffer;

    VkSemaphore nextImageAvailableSemaphores[HXF_MAX_RENDERED_FRAMES];
    VkSemaphore nextImageSubmitedSemaphores[HXF_MAX_RENDERED_FRAMES];
    VkFence imageRenderedFences[HXF_MAX_RENDERED_FRAMES];

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

/**
 * @brief Update the buffer that contains the cubes data.
 *
 * @param engine The engine that hold the buffer.
 */
void hxfEngineUpdateCubeBuffer(HxfEngine* restrict engine);

void hxfEngineUpdateIconBuffer(HxfEngine* restrict engine);
