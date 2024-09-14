#include <geometrySubpass.h>

static void CreateVkPipeline(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t geometrySubpassIndex = 0;
    Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];

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
            .stride = sizeof(GeometrySubpassVertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        },
    };
    uint32_t vertexAttributeDescriptionCount = 3;
    VkVertexInputAttributeDescription vertexAttributeDescriptions[] = {
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(GeometrySubpassVertex, position),
        },
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(GeometrySubpassVertex, color),
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
    VkDescriptorSetLayoutBinding modelUniformLayoutBinding = {
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = NULL,
    };
    VkDescriptorSetLayoutBinding *bindings = (VkDescriptorSetLayoutBinding[]){globalUniformLayoutBinding, modelUniformLayoutBinding};
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = 2,
        .pBindings = bindings,
    };
    VkResult result = vkCreateDescriptorSetLayout(pGraphicEngine->vkDevice, &descriptorSetLayoutCreateInfo, NULL, &pGeometrySubpass->descriptorSetLayout);
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &pGeometrySubpass->descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
    };

    result = vkCreatePipelineLayout(pGraphicEngine->vkDevice, &pipelineLayoutCreateInfo, NULL, &pGeometrySubpass->vkPipelineLayout);
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
        .layout = pGeometrySubpass->vkPipelineLayout,
        .renderPass = pDeferredRenderPass->vkRenderPass,
        .subpass = geometrySubpassIndex,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0,
    };

    VkPipelineCache pipelineCache = NULL;
    result = vkCreateGraphicsPipelines(pGraphicEngine->vkDevice, pipelineCache, 1, &geometryPipelineCreateInfo, NULL, &pGeometrySubpass->vkPipeline);
    TryThrowVulkanError(result);
    DestroyVkShaderModule(pGraphicEngine, geometryVertShaderModule);
    DestroyVkShaderModule(pGraphicEngine, geometryFragShaderModule);
}

static void DestroyVkPipeline(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t geometrySubpassIndex = 0;
    Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];
    vkDestroyDescriptorSetLayout(pGraphicEngine->vkDevice, pGeometrySubpass->descriptorSetLayout, NULL);
    vkDestroyPipelineLayout(pGraphicEngine->vkDevice, pGeometrySubpass->vkPipelineLayout, NULL);
    vkDestroyPipeline(pGraphicEngine->vkDevice, pGeometrySubpass->vkPipeline, NULL);
}

static void CreateGeometrySubpassModels(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t geometrySubpassIndex = 0;
    Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];

    pGeometrySubpass->maxModelCount = 256;
    pGeometrySubpass->modelCount = 0;
    pGeometrySubpass->subpassModels = TickernelMalloc(sizeof(SubpassModel) * pGeometrySubpass->maxModelCount);
    pGeometrySubpass->pRemovedIndexLinkedList = NULL;
}
static void DestroyGeometrySubpassModels(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t geometrySubpassIndex = 0;
    Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];
    while (NULL != pGeometrySubpass->pRemovedIndexLinkedList)
    {
        Uint32Node *pNode = pGeometrySubpass->pRemovedIndexLinkedList;
        pGeometrySubpass->pRemovedIndexLinkedList = pGeometrySubpass->pRemovedIndexLinkedList->pNext;
        TickernelFree(pNode);
    }
    TickernelFree(pGeometrySubpass->subpassModels);
}

static void CreateDescriptorPool(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t geometrySubpassIndex = 0;
    Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];
    VkDescriptorPoolSize poolSize[] = {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = pGeometrySubpass->maxModelCount * 2,
        }};
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .maxSets = pGeometrySubpass->maxModelCount,
        .poolSizeCount = 1,
        .pPoolSizes = poolSize,
    };
    VkResult result = vkCreateDescriptorPool(pGraphicEngine->vkDevice, &descriptorPoolCreateInfo, NULL, &pGeometrySubpass->vkDescriptorPool);
    TryThrowVulkanError(result);
}
static void DestroyDescriptorPool(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t geometrySubpassIndex = 0;
    Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];
    vkDestroyDescriptorPool(pGraphicEngine->vkDevice, pGeometrySubpass->vkDescriptorPool, NULL);
}

static void CreateSubpassModel(GraphicEngine *pGraphicEngine, SubpassModel *pOutputSubpassModel);
{
    pOutputSubpassModel.vertexCount = deferredRenderPipelineObject.vertexCount;

    // Create vertexBuffer
    VkDeviceSize bufferSize = sizeof(GeometrySubpassVertex) * deferredRenderPipelineObject.vertexCount;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(pGraphicEngine, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
    void *pData;
    result = vkMapMemory(pGraphicEngine->vkDevice, stagingBufferMemory, 0, bufferSize, 0, &pData);
    TryThrowVulkanError(result);
    memcpy(pData, deferredRenderPipelineObject.vertices, bufferSize);
    vkUnmapMemory(pGraphicEngine->vkDevice, stagingBufferMemory);
    CreateBuffer(pGraphicEngine, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &pOutputSubpassModel.vertexBuffer, &pOutputSubpassModel.vertexBufferMemory);
    TryThrowVulkanError(result);
    CopyVkBuffer(pGraphicEngine, stagingBuffer, pOutputSubpassModel.vertexBuffer, bufferSize);
    DestroyBuffer(pGraphicEngine->vkDevice, stagingBuffer, stagingBufferMemory);

    // Create objectUniformBuffer
    bufferSize = sizeof(DeferredRenderPipelineModelUniformBuffer);
    CreateBuffer(pGraphicEngine, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pOutputSubpassModel.objectUniformBuffer, &pOutputSubpassModel.objectUniformBufferMemory);
    vkMapMemory(pGraphicEngine->vkDevice, pOutputSubpassModel.objectUniformBufferMemory, 0, bufferSize, 0, &pOutputSubpassModel.objectUniformBufferMapped);

    // Create vkDescriptorSet
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = pGraphicEngine->deferredRenderPipeline.vkDescriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &pGraphicEngine->deferredRenderPipeline.vkPipelineToDescriptorSetLayout[0],
    };
    pOutputSubpassModel.vkPipelineToDescriptorSet = TickernelMalloc(sizeof(VkDescriptorSet) * 2);

    result = vkAllocateDescriptorSets(pGraphicEngine->vkDevice, &descriptorSetAllocateInfo, &pOutputSubpassModel.vkPipelineToDescriptorSet[0]);

    VkDescriptorBufferInfo globalDescriptorBufferInfo = {
        .buffer = pGraphicEngine->globalUniformBuffer,
        .offset = 0,
        .range = sizeof(GlobalUniformBuffer),
    };
    VkDescriptorBufferInfo objectDescriptorBufferInfo = {
        .buffer = pOutputSubpassModel.objectUniformBuffer,
        .offset = 0,
        .range = sizeof(DeferredRenderPipelineModelUniformBuffer),
    };

    VkWriteDescriptorSet descriptorWrites[2] = {
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = NULL,
            .dstSet = pOutputSubpassModel.vkPipelineToDescriptorSet[0],
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
            .dstSet = pOutputSubpassModel.vkPipelineToDescriptorSet[0],
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
}
static void DestroySubpassModel(GraphicEngine *pGraphicEngine, SubpassModel *pSubpassModel);
{
}

void CreateGeometrySubpass(GraphicEngine *pGraphicEngine)
{
    CreateVkPipeline(pGraphicEngine);
    CreateGeometrySubpassModels(pGraphicEngine);
    CreateDescriptorPool(pGraphicEngine);
}

void DestroyGeometrySubpass(GraphicEngine *pGraphicEngine)
{
    DestroyDescriptorPool(pGraphicEngine);
    DestroyGeometrySubpassModels(pGraphicEngine);
    DestroyVkPipeline(pGraphicEngine);
}

void AddModelToGeometrySubpass(GraphicEngine *pGraphicEngine, GeometrySubpassVertex *geometrySubpassVertices, uint32_t *pOutputIndex)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t geometrySubpassIndex = 0;
    Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];
    if (NULL != pGeometrySubpass->pRemovedIndexLinkedList)
    {
        uint32_t index = pGeometrySubpass->pRemovedIndexLinkedList->data;
        Uint32Node *pNode = pGeometrySubpass->pRemovedIndexLinkedList;
        pGeometrySubpass->pRemovedIndexLinkedList = pGeometrySubpass->pRemovedIndexLinkedList->pNext;
        TickernelFree(pNode);
        CreateSubpassModel(pGraphicEngine, &pGeometrySubpass->subpassModels[index]);
    }
    else if (pGeometrySubpass->modelCount < pGeometrySubpass->maxModelCount)
    {
        uint32_t index = pGeometrySubpass->modelCount;
        CreateSubpassModel(pGraphicEngine, &pGeometrySubpass->subpassModels[index]);
        pGeometrySubpass->modelCount++;
    }
    else
    {
        uint32_t oldMaxModelCount = pGeometrySubpass->maxModelCount;
        pGeometrySubpass->maxModelCount = pGeometrySubpass->maxModelCount * 2;
        SubpassModel *oldModels = pGeometrySubpass->subpassModels;
        pGeometrySubpass->subpassModels = TickernelMalloc(sizeof(SubpassModel) * pGeometrySubpass->maxModelCount);
        memcpy(pGeometrySubpass->subpassModels, oldModels, sizeof(SubpassModel) * oldMaxModelCount);
        // TODO: Recreate descriptor pool!
        uint32_t index = pGeometrySubpass->modelCount;
        CreateSubpassModel(pGraphicEngine, &pGeometrySubpass->subpassModels[index]);
        pGeometrySubpass->modelCount++;
    }
}
void RemoveModelFromGeometrySubpass(GraphicEngine *pGraphicEngine, uint32_t inputIndex)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t geometrySubpassIndex = 0;
    Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];
}