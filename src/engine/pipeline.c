#include "pipeline.h"

/*
STATIC FORWARD DECLARATION
*/

/**
 * @brief Create a VkShaderModule from a file.
 *
 * @param engine The HxfGraphicsHandler that will own the shader module.
 * @param filename The name of the file that contains the shader code.
 *
 * @return The VkShaderModule.
 */
static VkShaderModule createShaderModule(HxfGraphicsHandler* restrict engine, const char* filename);

/**
 * @brief Create the render pass.
 *
 * @param engine The HxfGraphicsHandler that will own it.
 */
static void createRenderPass(HxfGraphicsHandler* restrict engine);

/**
 * @brief Create the descriptors.
 *
 * @param engine The HxfGraphicsHandler that own them.
 */
static void createDescriptors(HxfGraphicsHandler* restrict engine);

/*
IMPLEMENTATION
*/

static VkShaderModule createShaderModule(HxfGraphicsHandler* restrict engine, const char* filename) {
    VkShaderModule shaderModule;
    void* code;
    size_t codeSize;

    if (hxfReadFile(filename, &code, &codeSize) == HXF_ERROR) {
        HXF_FATAL("Could not open a shader file");
    }

    VkShaderModuleCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pCode = code,
        .codeSize = codeSize
    };

    HXF_TRY_VK(vkCreateShaderModule(engine->device, &info, NULL, &shaderModule));

    hxfFree(code);

    return shaderModule;
}

static void createRenderPass(HxfGraphicsHandler* restrict engine) {
    VkAttachmentDescription attachmentDescriptions[] = {
        {
            .format = engine->swapchainImageFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        },
        {
            .format = engine->drawingData.depthImageFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        }
    };

    VkAttachmentReference colorAttachementReference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentReference depthAttachmentReference = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpassDescription = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachementReference,
        .pDepthStencilAttachment = &depthAttachmentReference
    };

    VkSubpassDependency dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcAccessMask = 0,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    };

    VkRenderPassCreateInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 2,
        .pAttachments = attachmentDescriptions,
        .subpassCount = 1,
        .pSubpasses = &subpassDescription,
        .dependencyCount = 1,
        .pDependencies = &dependency,
    };

    HXF_TRY_VK(vkCreateRenderPass(engine->device, &renderPassInfo, NULL, &engine->renderPass));
}

static void createDescriptors(HxfGraphicsHandler* restrict engine) {
    // Descriptor set layouts

    VkDescriptorSetLayoutBinding cubeLayoutBindings[] = {
        { // ubo
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
        },
        { // texture sampler
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        }
    };
    VkDescriptorSetLayoutBinding iconLayoutBindings[] = {
        {
            .binding = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        }
    };
    VkDescriptorSetLayoutCreateInfo cubeLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 2,
        .pBindings = cubeLayoutBindings,
    };
    VkDescriptorSetLayoutCreateInfo iconLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = iconLayoutBindings
    };
    HXF_TRY_VK(vkCreateDescriptorSetLayout(engine->device, &cubeLayoutInfo, NULL, &engine->cubeDescriptorSetLayout));
    HXF_TRY_VK(vkCreateDescriptorSetLayout(engine->device, &iconLayoutInfo, NULL, &engine->iconDescriptorSetLayout));

    // Create a descriptor pool

    VkDescriptorPoolSize cubeDescriptorPoolSizes[] = {
        { // ubo
            .descriptorCount = HXF_MAX_RENDERED_FRAMES,
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        },
        { // texture sampler
            .descriptorCount = HXF_MAX_RENDERED_FRAMES,
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
        }
    };
    VkDescriptorPoolSize iconDescriptorPoolSizes[] = {
        { // texture sampler
            .descriptorCount = HXF_MAX_RENDERED_FRAMES,
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
        }
    };

    VkDescriptorPoolCreateInfo cubeDescriptorPoolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = HXF_MAX_RENDERED_FRAMES,
        .poolSizeCount = 2,
        .pPoolSizes = cubeDescriptorPoolSizes,
    };
    VkDescriptorPoolCreateInfo iconDescriptorPoolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = HXF_MAX_RENDERED_FRAMES,
        .poolSizeCount = 1,
        .pPoolSizes = iconDescriptorPoolSizes,
    };
    HXF_TRY_VK(vkCreateDescriptorPool(engine->device, &cubeDescriptorPoolInfo, NULL, &engine->cubeDescriptorPool));
    HXF_TRY_VK(vkCreateDescriptorPool(engine->device, &iconDescriptorPoolInfo, NULL, &engine->iconDescriptorPool));

    // Copy HXF_MAX_RENDERED_FRAMES times engine->descriptorSetLayout

    VkDescriptorSetLayout cubeSetLayouts[HXF_MAX_RENDERED_FRAMES] = { 0 };
    VkDescriptorSetLayout iconSetLayouts[HXF_MAX_RENDERED_FRAMES] = { 0 };
    for (int i = 0; i != HXF_MAX_RENDERED_FRAMES; i++) {
        cubeSetLayouts[i] = engine->cubeDescriptorSetLayout;
        iconSetLayouts[i] = engine->iconDescriptorSetLayout;
    }

    // And allocate the descriptor sets from the pool

    VkDescriptorSetAllocateInfo cubeDescriptorSetInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = engine->cubeDescriptorPool,
        .descriptorSetCount = HXF_MAX_RENDERED_FRAMES,
        .pSetLayouts = cubeSetLayouts,
    };
    VkDescriptorSetAllocateInfo iconDescriptorSetInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = engine->iconDescriptorPool,
        .descriptorSetCount = HXF_MAX_RENDERED_FRAMES,
        .pSetLayouts = iconSetLayouts,
    };
    HXF_TRY_VK(vkAllocateDescriptorSets(engine->device, &cubeDescriptorSetInfo, engine->cubeDescriptorSets));
    HXF_TRY_VK(vkAllocateDescriptorSets(engine->device, &iconDescriptorSetInfo, engine->iconDescriptorSets));

    // Update the descriptor sets

    for (int i = 0; i != HXF_MAX_RENDERED_FRAMES; i++) {
        VkDescriptorBufferInfo uboBufferInfo = {
            .buffer = engine->drawingData.hostBuffer,
            .offset = engine->drawingData.mvpOffset,
            .range = engine->drawingData.mvpSize,
        };
        VkDescriptorImageInfo textureImageInfo = {
            .sampler = engine->drawingData.textureSampler,
            .imageView = engine->drawingData.textureImageView,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };
        VkWriteDescriptorSet cubeWriteDescriptorSets[] = {
            // Cube
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = engine->cubeDescriptorSets[i],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &uboBufferInfo,
            },
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = engine->cubeDescriptorSets[i],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &textureImageInfo
            },
            // Icon
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = engine->iconDescriptorSets[i],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &textureImageInfo
            }
        };
        vkUpdateDescriptorSets(engine->device, 3, cubeWriteDescriptorSets, 0, NULL);
    }
}

void createPipelines(HxfGraphicsHandler* restrict engine) {
    // Create the pipeline cache

    VkPipelineCacheCreateInfo pipelineCacheInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
        .initialDataSize = 0,
        .pInitialData = NULL,
    };
    HXF_TRY_VK(vkCreatePipelineCache(engine->device, &pipelineCacheInfo, NULL, &engine->pipelineCache));

    createRenderPass(engine);
    createDescriptors(engine);

    // Shader modules creation

    const char cubeVertex[] = "/shaders/vertexCube.spv";
    const char cubeFragment[] = "/shaders/fragmentCube.spv";
    const char iconVertex[] = "/shaders/vertexIcon.spv";
    const char iconFragment[] = "/shaders/fragmentIcon.spv";

    char* cubeVertexPath = hxfMalloc(sizeof(char) * (strlen(engine->appdataDirectory) + sizeof(cubeVertex)));
    char* cubeFragmentPath = hxfMalloc(sizeof(char) * (strlen(engine->appdataDirectory) + sizeof(cubeFragment)));
    char* iconVertexPath = hxfMalloc(sizeof(char) * (strlen(engine->appdataDirectory) + sizeof(iconVertex)));
    char* iconFragmentPath = hxfMalloc(sizeof(char) * (strlen(engine->appdataDirectory) + sizeof(iconFragment)));
    strcpy(cubeVertexPath, engine->appdataDirectory);
    strcpy(cubeFragmentPath, engine->appdataDirectory);
    strcpy(iconVertexPath, engine->appdataDirectory);
    strcpy(iconFragmentPath, engine->appdataDirectory);

    strcat(cubeVertexPath, cubeVertex);
    strcat(cubeFragmentPath, cubeFragment);
    strcat(iconVertexPath, iconVertex);
    strcat(iconFragmentPath, iconFragment);

    VkShaderModule cubeVertexModule = createShaderModule(engine, cubeVertexPath);
    VkShaderModule cubeFragmentModule = createShaderModule(engine, cubeFragmentPath);
    VkShaderModule iconVertexModule = createShaderModule(engine, iconVertexPath);
    VkShaderModule iconFragmentModule = createShaderModule(engine, iconFragmentPath);

    hxfFree(cubeVertexPath);
    hxfFree(cubeFragmentPath);
    hxfFree(iconVertexPath);
    hxfFree(iconFragmentPath);

    VkPipelineShaderStageCreateInfo cubeStages[] = {
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = cubeVertexModule,
            .pName = "main",
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = cubeFragmentModule,
            .pName = "main",
        },
    };
    VkPipelineShaderStageCreateInfo iconStages[] = {
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = iconVertexModule,
            .pName = "main",
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = iconFragmentModule,
            .pName = "main",
        },
    };

    VkVertexInputBindingDescription cubeBindingDescriptions[] = {
        {
            .binding = 0,
            .stride = sizeof(HxfCubeVertexData),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        },
        {
            .binding = 1,
            .stride = sizeof(HxfCubeInstanceData),
            .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE
        },
    };
    VkVertexInputAttributeDescription cubeAttributeDescriptions[] = {
        { // Vertex position
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(HxfCubeVertexData, position),
        },
        { // Texel coordinate
            .binding = 0,
            .location = 2,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(HxfCubeVertexData, texelCoordinate),
        },
        { // Cube position
            .binding = 1,
            .location = 1,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(HxfCubeInstanceData, position)
        },
        { // Texture index
            .binding = 1,
            .location = 3,
            .format = VK_FORMAT_R32_UINT,
            .offset = offsetof(HxfCubeInstanceData, textureIndex)
        }
    };
    VkVertexInputBindingDescription iconBindingDescriptions[] = {
        {
            .binding = 0,
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
            .stride = sizeof(HxfIconVertexData)
        },
        {
            .binding = 1,
            .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE,
            .stride = sizeof(HxfIconInstanceData)
        }
    };
    VkVertexInputAttributeDescription iconAttributeDescriptions[] = {
        {
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(HxfIconVertexData, position)
        },
        {
            .binding = 0,
            .location = 1,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(HxfIconVertexData, texelCoordinate)
        },
        {
            .binding = 1,
            .location = 2,
            .format = VK_FORMAT_R32_UINT,
            .offset = offsetof(HxfIconInstanceData, textureIndex)
        }
    };

    VkPipelineVertexInputStateCreateInfo cubeInputInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 2,
        .pVertexBindingDescriptions = cubeBindingDescriptions,
        .vertexAttributeDescriptionCount = 4,
        .pVertexAttributeDescriptions = cubeAttributeDescriptions,
    };
    VkPipelineVertexInputStateCreateInfo iconInputInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 2,
        .pVertexBindingDescriptions = iconBindingDescriptions,
        .vertexAttributeDescriptionCount = 3,
        .pVertexAttributeDescriptions = iconAttributeDescriptions,
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .width = engine->swapchainExtent.width,
        .height = engine->swapchainExtent.height,
    };

    VkRect2D scissors = {
        .extent = engine->swapchainExtent,
        .offset.x = 0,
        .offset.y = 0,
    };

    VkPipelineViewportStateCreateInfo viewportInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissors,
    };

    VkPipelineRasterizationStateCreateInfo rasterizationInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .lineWidth = 1.0f,
    };

    VkPipelineMultisampleStateCreateInfo multisampleInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
    };

    VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };
    VkPipelineColorBlendStateCreateInfo colorBlendInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
    };

    // Create the pipeline layouts

    VkDescriptorSetLayout cubeSetLayouts[] = {
        engine->cubeDescriptorSetLayout
    };
    VkPipelineLayoutCreateInfo cubePipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = cubeSetLayouts,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL
    };

    VkDescriptorSetLayout iconSetLayouts[] = {
        engine->iconDescriptorSetLayout
    };
    VkPushConstantRange iconPushConstantRanges[] = {
        {
            .offset = 0,
            .size = sizeof(HxfIconPushConstantData),
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
        }
    };
    VkPipelineLayoutCreateInfo iconPipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = iconSetLayouts,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = iconPushConstantRanges
    };

    HXF_TRY_VK(vkCreatePipelineLayout(engine->device, &cubePipelineLayoutInfo, NULL, &engine->cubePipelineLayout));
    HXF_TRY_VK(vkCreatePipelineLayout(engine->device, &iconPipelineLayoutInfo, NULL, &engine->iconPipelineLayout));

    // Create the graphics pipeline
    VkGraphicsPipelineCreateInfo pipelineInfos[] = {
        { // Cube pipeline
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = 2,
            .pStages = cubeStages,
            .pVertexInputState = &cubeInputInfo,
            .pInputAssemblyState = &inputAssemblyInfo,
            .pViewportState = &viewportInfo,
            .pRasterizationState = &rasterizationInfo,
            .pMultisampleState = &multisampleInfo,
            .pDepthStencilState = &depthStencilInfo,
            .pColorBlendState = &colorBlendInfo,
            .layout = engine->cubePipelineLayout,
            .renderPass = engine->renderPass,
        },
        { // Icon pipeline
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = 2,
            .pStages = iconStages,
            .pVertexInputState = &iconInputInfo,
            .pInputAssemblyState = &inputAssemblyInfo,
            .pViewportState = &viewportInfo,
            .pRasterizationState = &rasterizationInfo,
            .pDepthStencilState = &depthStencilInfo,
            .pMultisampleState = &multisampleInfo,
            .pColorBlendState = &colorBlendInfo,
            .layout = engine->iconPipelineLayout,
            .renderPass = engine->renderPass,
            .basePipelineIndex = 0
        }
    };

    HXF_TRY_VK(vkCreateGraphicsPipelines(engine->device, engine->pipelineCache, 2, pipelineInfos, NULL, &engine->cubePipeline));

    vkDestroyShaderModule(engine->device, cubeFragmentModule, NULL);
    vkDestroyShaderModule(engine->device, iconFragmentModule, NULL);
    vkDestroyShaderModule(engine->device, cubeVertexModule, NULL);
    vkDestroyShaderModule(engine->device, iconVertexModule, NULL);
}