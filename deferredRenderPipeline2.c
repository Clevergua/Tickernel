#include <deferredRenderPipeline.h>

static void PrepareCurrentFrambuffer(GraphicEngine *pGraphicEngine)
{
    uint32_t attachmentCount = 3;
    VkImageView attachments[] = {pGraphicEngine->swapchainImageViews[pGraphicEngine->frameIndex], pGraphicEngine->depthGraphicImage.vkImageView, pGraphicEngine->albedoGraphicImage.vkImageView};
    if (INVALID_VKFRAMEBUFFER == pGraphicEngine->deferredRenderPass.vkFramebuffers[pGraphicEngine->frameIndex])
    {
        VkFramebufferCreateInfo vkFramebufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .renderPass = pGraphicEngine->deferredRenderPipeline.vkRenderPass,
            .attachmentCount = attachmentCount,
            .pAttachments = attachments,
            .width = pGraphicEngine->swapchainExtent.width,
            .height = pGraphicEngine->swapchainExtent.height,
            .layers = 1,
        };
        VkResult result = vkCreateFramebuffer(pGraphicEngine->vkDevice, &vkFramebufferCreateInfo, NULL, &pGraphicEngine->deferredRenderPass.vkFramebuffers[pGraphicEngine->frameIndex]);
        TryThrowVulkanError(result);
    }
}

static void CreateVkFramebuffers(GraphicEngine *pGraphicEngine)
{
    pGraphicEngine->deferredRenderPass.vkFramebuffers = TickernelMalloc(sizeof(VkFramebuffer) * pGraphicEngine->swapchainImageCount);
    pGraphicEngine->deferredRenderPass.vkFramebufferCount = pGraphicEngine->swapchainImageCount;
    for (int32_t i = 0; i < pGraphicEngine->deferredRenderPass.vkFramebufferCount; i++)
    {
        pGraphicEngine->deferredRenderPass.vkFramebuffers[i] = INVALID_VKFRAMEBUFFER;
    }
}
static void DestroyVkFramebuffers(GraphicEngine *pGraphicEngine)
{
    for (int32_t i = 0; i < pGraphicEngine->deferredRenderPass.vkFramebufferCount; i++)
    {
        if (INVALID_VKFRAMEBUFFER == pGraphicEngine->deferredRenderPass.vkFramebuffers[i])
        {
            // continue;
        }
        else
        {
            vkDestroyFramebuffer(pGraphicEngine->vkDevice, pGraphicEngine->deferredRenderPass.vkFramebuffers[i], NULL);
        }
    }
}

static void CreateGeometryPipeline(GraphicEngine *pGraphicEngine)
{
    VkResult result = VK_SUCCESS;
    VkShaderModule geometryVertShaderModule;
    CreateVkShaderModule(pGraphicEngine, "../shaders/geometry.vert", &geometryVertShaderModule);
    VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = geometryVertShaderModule,
        .pName = "main",
        .pSpecializationInfo = NULL,
    };

    VkShaderModule geometryFragShaderModule;
    CreateVkShaderModule(pGraphicEngine, "../shaders/geometry.frag", &geometryFragShaderModule);
    VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = geometryFragShaderModule,
        .pName = "main",
        .pSpecializationInfo = NULL,
    };
    uint32_t stageCount = 2;
    VkPipelineShaderStageCreateInfo *pipelineShaderStageCreateInfos = (VkPipelineShaderStageCreateInfo[]){vertShaderStageCreateInfo, fragShaderStageCreateInfo};

    uint32_t vertexBindingDescriptionCount = 1;
    VkVertexInputBindingDescription *vertexBindingDescriptions = (VkVertexInputBindingDescription[]){
        {
            .binding = 0,
            .stride = sizeof(DeferredRenderPipelineVertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        },
    };
    uint32_t vertexAttributeDescriptionCount = 3;
    VkVertexInputAttributeDescription vertexAttributeDescriptions[] = {
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(DeferredRenderPipelineVertex, position),
        },
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(DeferredRenderPipelineVertex, color),
        }};
    VkPipelineVertexInputStateCreateInfo vkPipelineVertexInputStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .vertexBindingDescriptionCount = vertexBindingDescriptionCount,
        .pVertexBindingDescriptions = vertexBindingDescriptions,
        .vertexAttributeDescriptionCount = vertexAttributeDescriptionCount,
        .pVertexAttributeDescriptions = vertexAttributeDescriptions,
    };
    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };
    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = pGraphicEngine->swapchainExtent.width,
        .height = pGraphicEngine->swapchainExtent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    VkOffset2D offset = {
        .x = 0,
        .y = 0,
    };
    VkRect2D scissor = {
        .offset = offset,
        .extent = pGraphicEngine->swapchainExtent,
    };
    VkPipelineViewportStateCreateInfo pipelineViewportStateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };
    VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_POINT,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0,
        .depthBiasClamp = VK_FALSE,
        .depthBiasSlopeFactor = 0,
        .lineWidth = 1.0f,
    };
    VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .front = {},
        .back = {},
        .minDepthBounds = 0,
        .maxDepthBounds = 1,
    };
    VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT,
    };
    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &pipelineColorBlendAttachmentState,
        .blendConstants[0] = 0.0f,
        .blendConstants[1] = 0.0f,
        .blendConstants[2] = 0.0f,
        .blendConstants[3] = 0.0f,
    };
    uint32_t dynamicStateCount = 2;
    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
    VkPipelineDynamicStateCreateInfo dynamicState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .dynamicStateCount = dynamicStateCount,
        .pDynamicStates = dynamicStates,
    };

    VkDescriptorSetLayoutBinding globalUniformLayoutBinding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = NULL,
    };
    VkDescriptorSetLayoutBinding objectUniformLayoutBinding = {
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = NULL,
    };
    TryThrowVulkanError(result);
    VkDescriptorSetLayoutBinding *bindings = (VkDescriptorSetLayoutBinding[]){globalUniformLayoutBinding, objectUniformLayoutBinding};
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = 2,
        .pBindings = bindings,
    };
    result = vkCreateDescriptorSetLayout(pGraphicEngine->vkDevice, &descriptorSetLayoutCreateInfo, NULL, &pGraphicEngine->deferredRenderPass.vkPipelineToDescriptorSetLayout[0]);
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &pGraphicEngine->deferredRenderPipeline.vkPipelineToDescriptorSetLayout[0],
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
    };

    result = vkCreatePipelineLayout(pGraphicEngine->vkDevice, &pipelineLayoutCreateInfo, NULL, &pGraphicEngine->deferredRenderPass.vkPipelineToLayout[0]);
    TryThrowVulkanError(result);
    VkGraphicsPipelineCreateInfo geometryPipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stageCount = stageCount,
        .pStages = pipelineShaderStageCreateInfos,
        .pVertexInputState = &vkPipelineVertexInputStateCreateInfo,
        .pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo,
        .pTessellationState = NULL,
        .pViewportState = &pipelineViewportStateInfo,
        .pRasterizationState = &pipelineRasterizationStateCreateInfo,
        .pMultisampleState = NULL,
        .pDepthStencilState = &pipelineDepthStencilStateCreateInfo,
        .pColorBlendState = &colorBlendStateCreateInfo,
        .pDynamicState = &dynamicState,
        .layout = pGraphicEngine->deferredRenderPipeline.vkPipelineToLayout[0],
        .renderPass = pGraphicEngine->deferredRenderPipeline.vkRenderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0,
    };

    VkPipelineCache pipelineCache = NULL;
    result = vkCreateGraphicsPipelines(pGraphicEngine->vkDevice, pipelineCache, 1, &geometryPipelineCreateInfo, NULL, &pGraphicEngine->deferredRenderPass.vkPipelines[0]);
    TryThrowVulkanError(result);
    DestroyVkShaderModule(pGraphicEngine, geometryVertShaderModule);
    DestroyVkShaderModule(pGraphicEngine, geometryFragShaderModule);
}
static void DestroyGeometryPipeline(GraphicEngine *pGraphicEngine)
{
    vkDestroyDescriptorSetLayout(pGraphicEngine->vkDevice, pGraphicEngine->deferredRenderPass.vkPipelineToDescriptorSetLayout[0], NULL);
    vkDestroyPipelineLayout(pGraphicEngine->vkDevice, pGraphicEngine->deferredRenderPass.vkPipelineToLayout[0], NULL);
    vkDestroyPipeline(pGraphicEngine->vkDevice, pGraphicEngine->deferredRenderPass.vkPipelines[0], NULL);
}
static void CreateLightingPipeline(GraphicEngine *pGraphicEngine)
{
    VkResult result = VK_SUCCESS;
    VkShaderModule lightingVertShaderModule;
    CreateVkShaderModule(pGraphicEngine, "../shaders/lighting.vert", &lightingVertShaderModule);
    VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = lightingVertShaderModule,
        .pName = "main",
        .pSpecializationInfo = NULL,
    };

    VkShaderModule lightingFragShaderModule;
    CreateVkShaderModule(pGraphicEngine, "../shaders/lighting.frag", &lightingFragShaderModule);
    VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = lightingFragShaderModule,
        .pName = "main",
        .pSpecializationInfo = NULL,
    };
    uint32_t stageCount = 2;
    VkPipelineShaderStageCreateInfo *pipelineShaderStageCreateInfos = (VkPipelineShaderStageCreateInfo[]){vertShaderStageCreateInfo, fragShaderStageCreateInfo};

    VkPipelineVertexInputStateCreateInfo vkPipelineVertexInputStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = NULL,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = NULL,
    };

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };
    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = pGraphicEngine->swapchainExtent.width,
        .height = pGraphicEngine->swapchainExtent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    VkOffset2D offset = {
        .x = 0,
        .y = 0,
    };
    VkRect2D scissor = {
        .offset = offset,
        .extent = pGraphicEngine->swapchainExtent,
    };
    VkPipelineViewportStateCreateInfo pipelineViewportStateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };
    VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0,
        .depthBiasClamp = VK_FALSE,
        .depthBiasSlopeFactor = 0,
        .lineWidth = 1.0f,
    };

    VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .depthTestEnable = VK_FALSE,
        .depthWriteEnable = VK_FALSE,
        .depthCompareOp = VK_COMPARE_OP_ALWAYS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .front = {},
        .back = {},
        .minDepthBounds = 0,
        .maxDepthBounds = 1,
    };

    VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {
        .blendEnable = false,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT,
    };

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &pipelineColorBlendAttachmentState,
        .blendConstants[0] = 0.0f,
        .blendConstants[1] = 0.0f,
        .blendConstants[2] = 0.0f,
        .blendConstants[3] = 0.0f,
    };

    uint32_t dynamicStateCount = 2;
    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
    VkPipelineDynamicStateCreateInfo dynamicState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .dynamicStateCount = dynamicStateCount,
        .pDynamicStates = dynamicStates,
    };
    pGraphicEngine->deferredRenderPass.vkPipelineToLayout[1] = NULL;
    VkGraphicsPipelineCreateInfo lightingPipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stageCount = stageCount,
        .pStages = pipelineShaderStageCreateInfos,
        .pVertexInputState = &vkPipelineVertexInputStateCreateInfo,
        .pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo,
        .pTessellationState = NULL,
        .pViewportState = &pipelineViewportStateInfo,
        .pRasterizationState = &pipelineRasterizationStateCreateInfo,
        .pMultisampleState = NULL,
        .pDepthStencilState = &pipelineDepthStencilStateCreateInfo,
        .pColorBlendState = &colorBlendStateCreateInfo,
        .pDynamicState = &dynamicState,
        .layout = pGraphicEngine->deferredRenderPipeline.vkPipelineToLayout[1],
        .renderPass = pGraphicEngine->deferredRenderPipeline.vkRenderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0,
    };

    VkPipelineCache pipelineCache = NULL;
    result = vkCreateGraphicsPipelines(pGraphicEngine->vkDevice, pipelineCache, 1, &lightingPipelineCreateInfo, NULL, &pGraphicEngine->deferredRenderPass.vkPipelines[1]);
    TryThrowVulkanError(result);
    DestroyVkShaderModule(pGraphicEngine, lightingVertShaderModule);
    DestroyVkShaderModule(pGraphicEngine, lightingFragShaderModule);
}
static void DestroyLightingPipeline(GraphicEngine *pGraphicEngine)
{
    vkDestroyDescriptorSetLayout(pGraphicEngine->vkDevice, pGraphicEngine->deferredRenderPass.vkPipelineToDescriptorSetLayout[1], NULL);
    vkDestroyPipelineLayout(pGraphicEngine->vkDevice, pGraphicEngine->deferredRenderPass.vkPipelineToLayout[1], NULL);
    vkDestroyPipeline(pGraphicEngine->vkDevice, pGraphicEngine->deferredRenderPass.vkPipelines[1], NULL);
}

static void CreateDescriptorPool(GraphicEngine *pGraphicEngine)
{
    VkDescriptorPoolSize poolSize[] = {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = pGraphicEngine->deferredRenderPipeline.maxObjectCount * 2,
        }};
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .maxSets = pGraphicEngine->deferredRenderPipeline.maxObjectCount,
        .poolSizeCount = 1,
        .pPoolSizes = poolSize,
    };
    VkResult result = vkCreateDescriptorPool(pGraphicEngine->vkDevice, &descriptorPoolCreateInfo, NULL, &pGraphicEngine->deferredRenderPass.vkDescriptorPool);
    TryThrowVulkanError(result);
}

static void DestroyDescriptorPool(GraphicEngine *pGraphicEngine)
{
    vkDestroyDescriptorPool(pGraphicEngine->vkDevice, pGraphicEngine->deferredRenderPass.vkDescriptorPool, NULL);
}

static void CreateSubpasses(GraphicEngine *pGraphicEngine)
{
    pGraphicEngine->deferredRenderPass.vkPipelineCount = 2;
    pGraphicEngine->deferredRenderPass.vkPipelines = TickernelMalloc(sizeof(VkPipeline) * pGraphicEngine->deferredRenderPass.vkPipelineCount);
    pGraphicEngine->deferredRenderPass.vkPipelineToLayout = TickernelMalloc(sizeof(VkPipelineLayout) * pGraphicEngine->deferredRenderPass.vkPipelineCount);
    pGraphicEngine->deferredRenderPass.vkPipelineToDescriptorSetLayout = TickernelMalloc(sizeof(VkDescriptorSetLayout *) * pGraphicEngine->deferredRenderPass.vkPipelineCount);

    pGraphicEngine->deferredRenderPass.maxObjectCount = 4096;
    pGraphicEngine->deferredRenderPass.renderPipelineObjects = TickernelMalloc(sizeof(RenderPipelineModel) * pGraphicEngine->deferredRenderPass.maxObjectCount);
    CreateDescriptorPool(pGraphicEngine);
    CreateGeometryPipeline(pGraphicEngine);
    CreateLightingPipeline(pGraphicEngine);
}

static void DestroySubpasses(GraphicEngine *pGraphicEngine)
{
    DestroyGeometryPipeline(pGraphicEngine);
    DestroyLightingPipeline(pGraphicEngine);
    DestroyDescriptorPool(pGraphicEngine);
    TickernelFree(pGraphicEngine->deferredRenderPass.renderPipelineObjects);

    TickernelFree(pGraphicEngine->deferredRenderPass.vkPipelineToDescriptorSetLayout);
    TickernelFree(pGraphicEngine->deferredRenderPass.vkPipelines);
    TickernelFree(pGraphicEngine->deferredRenderPass.vkPipelineToLayout);
}

void CreateDeferredRenderPipeline(GraphicEngine *pGraphicEngine)
{
    CreateVkRenderPass(pGraphicEngine);
    CreateVkFramebuffers(pGraphicEngine);
    CreateSubpasses(pGraphicEngine);
}

void DestroyDeferredRenderPipeline(GraphicEngine *pGraphicEngine)
{
    DestroySubpasses(pGraphicEngine);
    DestroyVkRenderPass(pGraphicEngine);
    DestroyVkFramebuffers(pGraphicEngine);
}

void RecordDeferredRenderPipeline(GraphicEngine *pGraphicEngine)
{
    // UpdateObjectUniformBuffer(pGraphicEngine);
    if (pGraphicEngine->hasRecreatedSwapchain)
    {
        for (uint32_t i = 0; i < pGraphicEngine->deferredRenderPass.vkFramebufferCount; i++)
        {
            if (pGraphicEngine->deferredRenderPass.vkFramebuffers[i] == INVALID_VKFRAMEBUFFER)
            {
                // continue;
            }
            else
            {
                vkDestroyFramebuffer(pGraphicEngine->vkDevice, pGraphicEngine->deferredRenderPass.vkFramebuffers[i], NULL);
                pGraphicEngine->deferredRenderPass.vkFramebuffers[i] = INVALID_VKFRAMEBUFFER;
            }
        }
    }

    VkResult result = VK_SUCCESS;
    RenderPipeline deferredRenderPipeline = pGraphicEngine->deferredRenderPass;

    VkCommandBuffer vkCommandBuffer = pGraphicEngine->graphicVkCommandBuffers[pGraphicEngine->frameIndex];
    VkCommandBufferBeginInfo vkCommandBufferBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = NULL,
            .flags = 0,
            .pInheritanceInfo = NULL,
        };
    result = vkBeginCommandBuffer(vkCommandBuffer, &vkCommandBufferBeginInfo);
    TryThrowVulkanError(result);
    VkOffset2D offset =
        {
            .x = 0,
            .y = 0,
        };
    VkRect2D renderArea =
        {
            .offset = offset,
            .extent = pGraphicEngine->swapchainExtent,
        };
    uint32_t clearValueCount = 2;
    VkClearValue *clearValues = (VkClearValue[]){
        {
            .color = {0.0f, 0.0f, 0.0f, 1.0f},
        },
        {
            .depthStencil = {1.0f, 0},
        },
    };

    PrepareCurrentFrambuffer(pGraphicEngine);
    VkRenderPassBeginInfo renderPassBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = NULL,
            .renderPass = deferredRenderPipeline.vkRenderPass,
            .framebuffer = pGraphicEngine->deferredRenderPipeline.vkFramebuffers[pGraphicEngine->frameIndex],
            .renderArea = renderArea,
            .clearValueCount = clearValueCount,
            .pClearValues = clearValues,
        };
    vkCmdBeginRenderPass(vkCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pGraphicEngine->deferredRenderPass.vkPipelines[0]);
    VkViewport viewport =
        {
            .x = 0.0f,
            .y = 0.0f,
            .width = pGraphicEngine->swapchainExtent.width,
            .height = pGraphicEngine->swapchainExtent.height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };
    vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);

    VkOffset2D scissorOffset =
        {
            .x = 0,
            .y = 0,
        };
    VkRect2D scissor =
        {
            .offset = scissorOffset,
            .extent = pGraphicEngine->swapchainExtent,
        };
    vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);

    uint32_t geometryPipelineIndex = 0;
    for (uint32_t i = 0; i < pGraphicEngine->deferredRenderPass.objectCount; i++)
    {
        RenderPipelineModel *pRenderPipelineObject = &pGraphicEngine->deferredRenderPass.renderPipelineObjects[i];
        vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pGraphicEngine->deferredRenderPass.vkPipelineToLayout[geometryPipelineIndex], 0, 1, &pRenderPipelineObject->vkPipelineToDescriptorSet[0], 0, NULL);
        VkBuffer vertexBuffers[] = {pRenderPipelineObject->vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, vertexBuffers, offsets);
        uint32_t vertexCount = pRenderPipelineObject->vertexCount;
        vkCmdDraw(vkCommandBuffer, vertexCount, 1, 0, 0);
    }

    // vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pGraphicEngine->deferredRenderPipeline.vkPipelineToLayout[1], 0, 1, , 0, NULL);
    // vkCmdDraw(vkCommandBuffer, verticesCount, 1, 0, 0);

    vkCmdEndRenderPass(vkCommandBuffer);
    result = vkEndCommandBuffer(vkCommandBuffer);
    TryThrowVulkanError(result);
}

void AddObject2DeferredRenderPipeline(GraphicEngine *pGraphicEngine, GeometrySubpassModel deferredRenderPipelineObject, uint32_t *pIndex)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t geometrySubpassIndex = 0;
    Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];

    if (pGeometrySubpass->modelCount < pGeometrySubpass->maxModelCount)
    {
        RenderPipelineModel renderPipelineObject;
        renderPipelineObject.vertexCount = deferredRenderPipelineObject.vertexCount;

        // Create vertexBuffer
        VkDeviceSize bufferSize = sizeof(DeferredRenderPipelineVertex) * deferredRenderPipelineObject.vertexCount;
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(pGraphicEngine, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
        void *pData;
        result = vkMapMemory(pGraphicEngine->vkDevice, stagingBufferMemory, 0, bufferSize, 0, &pData);
        TryThrowVulkanError(result);
        memcpy(pData, deferredRenderPipelineObject.vertices, bufferSize);
        vkUnmapMemory(pGraphicEngine->vkDevice, stagingBufferMemory);
        CreateBuffer(pGraphicEngine, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &renderPipelineObject.vertexBuffer, &renderPipelineObject.vertexBufferMemory);
        TryThrowVulkanError(result);
        CopyVkBuffer(pGraphicEngine, stagingBuffer, renderPipelineObject.vertexBuffer, bufferSize);
        DestroyBuffer(pGraphicEngine->vkDevice, stagingBuffer, stagingBufferMemory);

        // Create objectUniformBuffer
        bufferSize = sizeof(DeferredRenderPipelineModelUniformBuffer);
        CreateBuffer(pGraphicEngine, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &renderPipelineObject.objectUniformBuffer, &renderPipelineObject.objectUniformBufferMemory);
        vkMapMemory(pGraphicEngine->vkDevice, renderPipelineObject.objectUniformBufferMemory, 0, bufferSize, 0, &renderPipelineObject.objectUniformBufferMapped);

        // Create vkDescriptorSet
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = NULL,
            .descriptorPool = pGraphicEngine->deferredRenderPipeline.vkDescriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &pGraphicEngine->deferredRenderPipeline.vkPipelineToDescriptorSetLayout[0],
        };
        renderPipelineObject.vkPipelineToDescriptorSet = TickernelMalloc(sizeof(VkDescriptorSet) * 2);

        result = vkAllocateDescriptorSets(pGraphicEngine->vkDevice, &descriptorSetAllocateInfo, &renderPipelineObject.vkPipelineToDescriptorSet[0]);

        VkDescriptorBufferInfo globalDescriptorBufferInfo = {
            .buffer = pGraphicEngine->globalUniformBuffer,
            .offset = 0,
            .range = sizeof(GlobalUniformBuffer),
        };
        VkDescriptorBufferInfo objectDescriptorBufferInfo = {
            .buffer = renderPipelineObject.objectUniformBuffer,
            .offset = 0,
            .range = sizeof(DeferredRenderPipelineModelUniformBuffer),
        };

        VkWriteDescriptorSet descriptorWrites[2] = {
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = NULL,
                .dstSet = renderPipelineObject.vkPipelineToDescriptorSet[0],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pImageInfo = NULL,
                .pBufferInfo = &globalDescriptorBufferInfo,
                .pTexelBufferView = NULL,
            },
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = NULL,
                .dstSet = renderPipelineObject.vkPipelineToDescriptorSet[0],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pImageInfo = NULL,
                .pBufferInfo = &objectDescriptorBufferInfo,
                .pTexelBufferView = NULL,
            },
        };
        vkUpdateDescriptorSets(pGraphicEngine->vkDevice, 2, descriptorWrites, 0, NULL);
        // Add to renderPipelineObjects
        pGraphicEngine->deferredRenderPass.renderPipelineObjects[pGraphicEngine->deferredRenderPass.objectCount] = renderPipelineObject;
        pGraphicEngine->deferredRenderPass.objectCount++;
    }
    else
    {
        /* code */
    }
}

void RemoveObjectFromDeferredRenderPipeline(GraphicEngine *pGraphicEngine, uint32_t index)
{
    RenderPipelineModel *pRenderPipelineObject = &pGraphicEngine->deferredRenderPass.renderPipelineObjects[index];
    vkFreeDescriptorSets(pGraphicEngine->vkDevice, pGraphicEngine->deferredRenderPass.vkDescriptorPool, 1, &pRenderPipelineObject->vkPipelineToDescriptorSet[0]);
    TickernelFree(pRenderPipelineObject->vkPipelineToDescriptorSet);
    DestroyBuffer(pGraphicEngine->vkDevice, pRenderPipelineObject->objectUniformBuffer, pRenderPipelineObject->objectUniformBufferMemory);

    DestroyBuffer(pGraphicEngine->vkDevice, pRenderPipelineObject->vertexBuffer, pRenderPipelineObject->vertexBufferMemory);
}