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
#define HXF_CUBE_INSTANCE_COUNT HXF_WORLD_PIECE_CUBE_COUNT * HXF_HORIZONTAL_VIEW_DISTANCE * HXF_HORIZONTAL_VIEW_DISTANCE * HXF_VERTICAL_VIEW_DISTANCE
#define HXF_CUBE_INSTANCES_SIZE 6 * sizeof(HxfCubeInstanceData) * HXF_CUBE_INSTANCE_COUNT

#define HXF_ICON_VERTEX_DATA_COUNT 4
#define HXF_ICON_VERTEX_INDEX_COUNT 6
#define HXF_ICON_INSTANCE_DATA_COUNT 1

#define HXF_POINTER_VERTEX_COUNT 12

#define HXF_TEXTURE_COUNT 5

#define HXF_FACES_TOP_OFFSET     0 * HXF_CUBE_INSTANCE_COUNT
#define HXF_FACES_BACK_OFFSET    1 * HXF_CUBE_INSTANCE_COUNT
#define HXF_FACES_BOTTOM_OFFSET  2 * HXF_CUBE_INSTANCE_COUNT
#define HXF_FACES_FRONT_OFFSET   3 * HXF_CUBE_INSTANCE_COUNT
#define HXF_FACES_RIGHT_OFFSET   4 * HXF_CUBE_INSTANCE_COUNT
#define HXF_FACES_LEFT_OFFSET    5 * HXF_CUBE_INSTANCE_COUNT

typedef struct HxfCubeInstanceData {
    alignas(16) HxfVec3 position;
    alignas(4)  uint32_t textureIndex;
} HxfCubeInstanceData;

typedef struct HxfCubeVertexData {
    alignas(16) HxfVec3 position;
    alignas(8)  HxfVec2 texelCoordinate;
    alignas(4)  uint32_t textureIndex;
} HxfCubeVertexData;

typedef struct HxfIconVertexData {
    alignas(8) HxfVec2 position;
    alignas(8) HxfVec2 texelCoordinate;
} HxfIconVertexData;

typedef struct HxfIconInstanceData {
    alignas(4) uint32_t textureIndex;
} HxfIconInstanceData;

typedef struct HxfIconPushConstantData {
    alignas(4) uint32_t windowWidth;
    alignas(4) uint32_t windowHeight;
} HxfIconPushConstantData;

typedef struct HxfPointerPushConstantData {
    alignas(4) uint32_t windowWidth;
    alignas(4) uint32_t windowHeight;
} HxfPointerPushConstantData;

typedef struct HxfMvpData {
    alignas(16) HxfMat4 model;
    alignas(16) HxfMat4 view;
    alignas(16) HxfMat4 projection;
} HxfMvpData;

/**
 * @brief Contains information on the things that will be drawn.
 *
 * For example, the vertex data, the mvp matrix,
 * the buffer that holds them...
 */
typedef struct HxfDrawingData {
    VkBuffer hostBuffer; ///< Buffer on the host memory.
    VkBuffer deviceBuffer; ///< Buffer on the host memory.
    VkBuffer transferBuffer; ///< Buffer on the host memory that can transfer data to the device buffer.

    VkImage textureImage;
    VkImageView textureImageView;
    VkSampler textureSampler;

    VkImage depthImage; ///< The depth image
    VkImageView depthImageView; /// The image view of the depth image
    VkFormat depthImageFormat; ///< The format of the depth image

    HxfCubeVertexData cubesVertices[HXF_CUBE_VERTEX_DATA_COUNT];
    HxfCubeInstanceData* cubeInstances; ///< Data for each cubes faces (6 * HXF_CUBE_INSTANCE_COUNT)
    uint32_t cubesVertexIndices[HXF_CUBE_VERTEX_INDEX_COUNT];

    HxfIconVertexData iconVertices[HXF_ICON_VERTEX_DATA_COUNT];
    HxfIconInstanceData iconInstances[HXF_ICON_INSTANCE_DATA_COUNT];
    uint32_t iconVertexIndices[HXF_ICON_VERTEX_INDEX_COUNT];

    HxfMvpData mvp; ///< The model-view-projection matrices

    size_t faceFrontCount;  // Number of front faces to draw 
    size_t faceBackCount;   // Number of back faces to draw 
    size_t faceTopCount;    // Number of top faces to draw
    size_t faceBottomCount; // Number of bottom faces to draw
    size_t faceRightCount;  // Number of right faces to draw
    size_t faceLeftCount;   // Number of left faces to draw

    // Memory offsets and sizes

    VkDeviceSize cubesVerticesOffset;
    VkDeviceSize cubesVerticesSize;
    VkDeviceSize cubesVertexIndicesOffset;
    VkDeviceSize cubesVertexIndicesSize;
    VkDeviceSize cubeInstancesOffset;
    VkDeviceSize cubeInstancesSize;
    VkDeviceSize pointedCubeOffset;
    VkDeviceSize pointedCubeSize;
    VkDeviceSize iconVerticesOffset;
    VkDeviceSize iconVerticesSize;
    VkDeviceSize iconVertexIndicesOffset;
    VkDeviceSize iconVertexIndicesSize;
    VkDeviceSize iconInstancesOffset;
    VkDeviceSize iconInstancesSize;
    VkDeviceSize mvpOffset;
    VkDeviceSize mvpSize;
    VkDeviceSize depthImageOffset; ///< Offset of the depth image inside the memory
    VkDeviceSize depthImageSize; ///< Size of the depth image inside the memory
    VkDeviceSize textureImageOffset;
    VkDeviceSize textureImageSize;

    VkDeviceSize hostBufferOffset;
    VkDeviceSize deviceBufferOffset;
    VkDeviceSize transferBufferOffset;
} HxfDrawingData;

/**
 * @brief Contains the objects that are needed to use the Vulkan API.
 */
typedef struct HxfGraphicsHandler {
    const char* const appdataDirectory; ///< A reference to the appdata directory that contains all the needed files.
    const HxfKeyboardState* keyboardState; ///< A reference to the keyboard state
    const HxfCamera* camera; ///< A reference to the player’s camera
    const HxfWorld* world; ///< A reference to the world.

    HxfWindow* mainWindow; ///< The main window
    VkSurfaceKHR mainWindowSurface; ///< The VkSurfaceKHR of the main window.

    VkDeviceMemory hostMemory; ///< Memory that is available for the host
    VkDeviceMemory deviceMemory; ///< Memory that is available for the device only.

    HxfDrawingData drawingData; ///< The drawing data.

    VkInstance instance; ///< The vulkan instance.
    VkPhysicalDevice physicalDevice; ///< The base physical device.
    VkDevice device; ///< The logical device that will do the graphics operation.
    VkQueue graphicsQueue; ///< The graphics queue that execute the operation.
    uint32_t graphicsQueueFamilyIndex; ///< Index family index of the graphics queue.

    VkPhysicalDeviceLimits physicalDeviceLimits; ///< The limits of the physical device.
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties; ///< The memory properties of the physical device.

    VkSwapchainKHR swapchain; ///< The swapchain
    VkImage* swapchainImages; ///< The images of the swapchain
    VkImageView* swapchainImageView; ///< The image views of the swapchain created from the swapchain images.
    VkFramebuffer* swapchainFramebuffers; ///< The framebuffers of the swapchain.
    uint32_t swapchainImageCount; ///< The number of images of the swapchain
    VkFormat swapchainImageFormat; ///< The format of the swapchain’s images
    VkExtent2D swapchainExtent; ///< The extent of the swapchain’s images.

    VkPipelineCache pipelineCache; ///< The cache for the pipelines.
    VkPipeline cubePipeline; ///< The pipeline that draw the cubes.
    VkPipeline iconPipeline; ///< The pipeline that draw the icons.
    VkPipeline pointerPipeline; ///< The pipeline that draw the pointer.
    VkPipelineLayout cubePipelineLayout; ///< The pipeline layout of the cube pipeline.
    VkPipelineLayout iconPipelineLayout; ///< The pipeline layout of the icon pipeline.
    VkPipelineLayout pointerPipelineLayout; ///< The pipeline layout of the pointer pipeline.
    VkRenderPass renderPass; ///< The render pass.
    VkDescriptorPool cubeDescriptorPool; ///< The descriptor pool for the cubes descriptors.
    VkDescriptorPool iconDescriptorPool; ///< The descriptor pool for the icons descriptors.
    VkDescriptorSetLayout cubeDescriptorSetLayout; ///< The descriptor set layout of the cubes descriptors.
    VkDescriptorSetLayout iconDescriptorSetLayout; ///< The descriptor set layout of the icons descriptors.
    VkDescriptorSet cubeDescriptorSets[HXF_MAX_RENDERED_FRAMES]; ///< The cubes’ descriptor sets.
    VkDescriptorSet iconDescriptorSets[HXF_MAX_RENDERED_FRAMES]; ///< The icons’ descriptor sets.

    VkCommandPool commandPool; ///< The command pool for the command buffers.
    /**
     * @brief All the command buffers allocated from the commandPool.
     *
     * List of the command buffers:
     * - HXF_MAX_RENDERED_FRAMES draw command buffers
     * - 1 transfer command buffer
     */
    VkCommandBuffer commandBuffers[HXF_MAX_RENDERED_FRAMES + 1];
    VkCommandBuffer* drawCommandBuffers; ///< A pointer to the first draw command buffer.
    VkCommandBuffer* transferCommandBuffer; ///< A pointer to the transferCommandBuffer.

    VkSemaphore nextImageAvailableSemaphores[HXF_MAX_RENDERED_FRAMES]; ///< Indicates when the next image of the swapchain is available.
    VkSemaphore nextImageSubmitedSemaphores[HXF_MAX_RENDERED_FRAMES]; ///< Indicates when the next image of the swapchain was submitted to the queue.
    VkFence imageRenderedFences[HXF_MAX_RENDERED_FRAMES]; ///< Indicates when the image has been rendered.
    /**
     * @brief Fence that can be used for anything.
     *
     * It is used when transfering data from one buffer to another.
     */
    VkFence fence;

    uint32_t currentFrame; ///< The index of the frame that is currently rendered
} HxfGraphicsHandler;

/**
 * @brief Initialize the graphics handler.
 */
void hxfGraphicsInit(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Destroy the graphics handler.
 */
void hxfGraphicsDestroy(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Stop the graphics handler.
 *
 * End all processing then return.
 */
void hxfGraphicsStop(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Run a single frame of the graphics handler.
 */
void hxfGraphicsFrame(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Update the buffer that contains the cubes data.
 */
void hxfGraphicsUpdateCubeBuffer(HxfGraphicsHandler* restrict graphics);

/**
 * @brief Update the buffer that contains the icons data.
 */
void hxfGraphicsUpdateIconBuffer(HxfGraphicsHandler* restrict graphics);
