/**
 * \file vulkan.h
 * \brief Contains all the vulkan code.
 */
#pragma once

#include <vulkan/vulkan.h>
#include "../hxf.h"
#include "../window.h"

/**
 * \brief The maximum number of frames that can be processed concurently
 */
#define HXF_MAX_FRAMES_IN_FLIGHT 2

/**
 * \struct HxfVulkanInstance
 * \brief Holds the Vulkan instance and all the other objects needed for this instance.
 */
typedef struct HxfVulkanInstance {
    VkInstance instance; ///< The vulkan instance.
    VkDebugUtilsMessengerEXT debugMessenger; ///< The debug messenger.

    HxfWindow window; ///< The window where things will be displayed on.
    VkSurfaceKHR surface; ///< The surface where we will draw things.

    VkPhysicalDevice physicalDevice; ///< The physical device that will be used.
    VkDevice device; ///< The logical device that will be used to do the operations on the GPU.
    VkQueue graphicsQueue; ///< The graphics queue of the device.
    VkQueue presentQueue; ///< The present queue of the device.

    VkSwapchainKHR swapchain;
    VkImage * swapchainImages;
    VkImageView * swapchainImageViews;
    VkFramebuffer * swapchainFrameBuffers;
    uint32_t swapchainImageCount; ///< The number of images, image views and framebuffers of the swapchain.
    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;

    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    uint32_t currentFrame; ///< The index of the frame that is being processed

    VkCommandPool commandPool;
    VkCommandBuffer * commandBuffers;

    VkSemaphore * imageAvailableSemaphores;
    VkSemaphore * renderFinishedSemaphores;
    VkFence * inFlightFences;
} HxfVulkanInstance;

/**
 * \brief Initializes a Vulkan instance.
 * \param instance An HxfVulkanInstance that will also be initialized.
 */
void hxfInitVulkan(HxfVulkanInstance * instance);

/**
 * \brief Destroy the HxfVulkan.
 * \param instance The HxfVulkanInstance to destroy.
 */
void hxfDestroyVulkan(HxfVulkanInstance * instance);

/**
 * \brief Run the main loop.
 * \param instance The HxfVulkanInstance that was previously called by hxfInitVulkan.
 */
void hxfRunVulkan(HxfVulkanInstance * instance);