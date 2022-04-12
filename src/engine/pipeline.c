#include "pipeline.h"

/*
STATIC FORWARD DECLARATION
*/

/**
 * @brief Create a VkShaderModule from a file.
 *
 * @param engine The HxfEngine that will own the shader module.
 * @param filename The name of the file that contains the shader code.
 *
 * @return The VkShaderModule.
 */
static VkShaderModule createShaderModule(HxfEngine* restrict engine, const char* filename);

/**
 * @brief Create the render pass.
 *
 * @param engine The HxfEngine that will own it.
 */
static void createRenderPass(HxfEngine* restrict engine);

/**
 * @brief Create the descriptor set layout, descriptor pool and descriptor sets.
 *
 * @param engine The HxfEngine that own them.
 */
static void createDescriptors(HxfEngine* restrict engine);

/*
IMPLEMENTATION
*/

static VkShaderModule createShaderModule(HxfEngine* restrict engine, const char* filename) {
    VkShaderModule shaderModule;
    void* code;
    size_t codeSize;

    if (readFile(filename, &code, &codeSize) == HXF_ERROR) {
        HXF_MSG_ERROR("Could not open a shader file");
        exit(EXIT_FAILURE);
    }

    VkShaderModuleCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pCode = code,
        .codeSize = codeSize
    };

    if (vkCreateShaderModule(engine->device, &info, NULL, &shaderModule)) {
        HXF_MSG_ERROR("Could not create a shader module");
        exit(EXIT_FAILURE);
    }

    hxfFree(code);

    return shaderModule;
}

static void createRenderPass(HxfEngine* restrict engine) {
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
            .format = engine->depthImageFormat,
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

    if (vkCreateRenderPass(engine->device, &renderPassInfo, NULL, &engine->renderPass)) {
        HXF_MSG_ERROR("Could not create the render pass");
        exit(EXIT_FAILURE);
    }
}

static void createDescriptors(HxfEngine* restrict engine) {
    // Create the descriptor set layout

    VkDescriptorSetLayoutBinding layoutBindings[] = {
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
    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 2,
        .pBindings = layoutBindings,
    };
    if (vkCreateDescriptorSetLayout(engine->device, &layoutInfo, NULL, &engine->descriptorSetLayout)) {
        HXF_MSG_ERROR("Could not create the ubo descriptor set layout");
        exit(EXIT_FAILURE);
    }

    // Create a descriptor pool

    VkDescriptorPoolSize descriptorPoolSizes[] = {
        { // ubo
            .descriptorCount = HXF_MAX_RENDERED_FRAMES,
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        },
        { // texture sampler
            .descriptorCount = HXF_MAX_RENDERED_FRAMES,
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
        }
    };
    VkDescriptorPoolCreateInfo descriptorPoolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = HXF_MAX_RENDERED_FRAMES,
        .poolSizeCount = 2,
        .pPoolSizes = descriptorPoolSizes,
    };
    if (vkCreateDescriptorPool(engine->device, &descriptorPoolInfo, NULL, &engine->descriptorPool)) {
        HXF_MSG_ERROR("Could not create the descriptor pool");
        exit(EXIT_FAILURE);
    }

    // Copy HXF_MAX_RENDERED_FRAMES times engine->descriptorSetLayout

    VkDescriptorSetLayout setLayouts[HXF_MAX_RENDERED_FRAMES] = { 0 };
    for (int i = 0; i != HXF_MAX_RENDERED_FRAMES; i++) {
        setLayouts[i] = engine->descriptorSetLayout;
    }

    // And allocate the descriptor sets from the pool

    VkDescriptorSetAllocateInfo descriptorSetInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = engine->descriptorPool,
        .descriptorSetCount = HXF_MAX_RENDERED_FRAMES,
        .pSetLayouts = setLayouts,
    };
    if (vkAllocateDescriptorSets(engine->device, &descriptorSetInfo, engine->descriptorSets)) {
        HXF_MSG_ERROR("Could not allocate the descriptor sets");
        exit(EXIT_FAILURE);
    }

    // Update the descriptor sets

    for (int i = 0; i != HXF_MAX_RENDERED_FRAMES; i++) {
        VkDescriptorBufferInfo bufferInfo = {
            .buffer = engine->drawingData.hostBuffer,
            .offset = engine->drawingData.uboOffset,
            .range = engine->drawingData.uboSize,
        };
        VkDescriptorImageInfo imageInfo = {
            .sampler = engine->drawingData.textureSampler,
            .imageView = engine->drawingData.textureImageView,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };
        VkWriteDescriptorSet writeDescriptorSets[] = {
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = engine->descriptorSets[i],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &bufferInfo,
            },
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = engine->descriptorSets[i],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &imageInfo
            }
        };
        vkUpdateDescriptorSets(engine->device, 2, writeDescriptorSets, 0, NULL);
    }
}

void createGraphicsPipeline(HxfEngine* restrict engine) {
    createRenderPass(engine);
    createDescriptors(engine);

    char vertexFileName[] = "/shaders/vertex.spv";
    char fragmentFileName[] = "/shaders/fragment.spv";
    char* vertexPath = hxfMalloc(sizeof(char) * (strlen(engine->appdataDirectory) + sizeof(vertexFileName)));
    char* fragmentPath = hxfMalloc(sizeof(char) * (strlen(engine->appdataDirectory) + sizeof(fragmentFileName)));
    strcpy(vertexPath, engine->appdataDirectory);
    strcpy(fragmentPath, engine->appdataDirectory);
    strcat(vertexPath, vertexFileName);
    strcat(fragmentPath, fragmentFileName);

    VkShaderModule vertexModule = createShaderModule(engine, vertexPath);
    VkShaderModule fragmentModule = createShaderModule(engine, fragmentPath);

    hxfFree(vertexPath);
    hxfFree(fragmentPath);

    VkPipelineShaderStageCreateInfo stages[] = {
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vertexModule,
            .pName = "main",
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragmentModule,
            .pName = "main",
        },
    };

    VkVertexInputBindingDescription cubeBindingDescriptions[] = {
        {
            .binding = 0,
            .stride = sizeof(HxfVertexData), // vertex pos + texel pos
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        },
        {
            .binding = 1,
            .stride = sizeof(HxfCubeData),
            .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE
        },
    };
    VkVertexInputAttributeDescription cubeAttributeDescriptions[] = {
        { // Vertex position
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(HxfVertexData, position),
        },
        { // Texel coordinate
            .binding = 0,
            .location = 2,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(HxfVertexData, texelCoordinate),
        },
        { // Cube position
            .binding = 1,
            .location = 1,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(HxfCubeData, cubePosition)
        },
        { // Texture index
            .binding = 1,
            .location = 3,
            .format = VK_FORMAT_R32_UINT,
            .offset = offsetof(HxfCubeData, textureIndex)
        }
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 2,
        .pVertexBindingDescriptions = cubeBindingDescriptions,
        .vertexAttributeDescriptionCount = 4,
        .pVertexAttributeDescriptions = cubeAttributeDescriptions,
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
        .lineWidth = 1.f,
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

    // Create the pipeline layout
    VkDescriptorSetLayout setLayouts[] = {
        engine->descriptorSetLayout
    };
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = { 0 };
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = setLayouts;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = NULL;

    if (vkCreatePipelineLayout(engine->device, &pipelineLayoutInfo, NULL, &engine->graphicsPipelineLayout)) {
        HXF_MSG_ERROR("Could not create the graphics pipeline layout");
        exit(EXIT_FAILURE);
    }

    // Create the graphics pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo = { 0 };
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = stages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
    pipelineInfo.pTessellationState = NULL;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pRasterizationState = &rasterizationInfo;
    pipelineInfo.pMultisampleState = &multisampleInfo;
    pipelineInfo.pDepthStencilState = &depthStencilInfo;
    pipelineInfo.pColorBlendState = &colorBlendInfo;
    pipelineInfo.pDynamicState = NULL;
    pipelineInfo.layout = engine->graphicsPipelineLayout;
    pipelineInfo.renderPass = engine->renderPass;
    pipelineInfo.subpass = 0;

    if (vkCreateGraphicsPipelines(engine->device, NULL, 1, &pipelineInfo, NULL, &engine->graphicsPipeline)) {
        HXF_MSG_ERROR("Could not create the graphics pipeline");
        exit(EXIT_FAILURE);
    }

    vkDestroyShaderModule(engine->device, fragmentModule, NULL);
    vkDestroyShaderModule(engine->device, vertexModule, NULL);
}