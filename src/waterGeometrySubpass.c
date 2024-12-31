#include <waterGeometrySubpass.h>
static void CreateVkPipeline(Subpass *pWaterGeometrySubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t waterGeometrySubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor)
{
    VkShaderModule waterGeometryVertShaderModule;
    char waterGeometryVertShaderPath[FILENAME_MAX];
    strcpy(waterGeometryVertShaderPath, shadersPath);
    TickernelCombinePaths(waterGeometryVertShaderPath, FILENAME_MAX, "waterGeometry.vert.spv");
    CreateVkShaderModule(vkDevice, waterGeometryVertShaderPath, &waterGeometryVertShaderModule);
    VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = waterGeometryVertShaderModule,
        .pName = "main",
        .pSpecializationInfo = NULL,
    };

    VkShaderModule waterGeometryFragShaderModule;
    char waterGeometryFragShaderPath[FILENAME_MAX];
    strcpy(waterGeometryFragShaderPath, shadersPath);
    TickernelCombinePaths(waterGeometryFragShaderPath, FILENAME_MAX, "waterGeometry.frag.spv");
    CreateVkShaderModule(vkDevice, waterGeometryFragShaderPath, &waterGeometryFragShaderModule);
    VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = waterGeometryFragShaderModule,
        .pName = "main",
        .pSpecializationInfo = NULL,
    };
    uint32_t stageCount = 2;
    VkPipelineShaderStageCreateInfo *pipelineShaderStageCreateInfos = (VkPipelineShaderStageCreateInfo[]){vertShaderStageCreateInfo, fragShaderStageCreateInfo};

    uint32_t vertexBindingDescriptionCount = 2;
    VkVertexInputBindingDescription *vertexBindingDescriptions = (VkVertexInputBindingDescription[]){
        {
            .binding = 0,
            .stride = sizeof(WaterGeometrySubpassVertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        },
        {
            .binding = 1,
            .stride = sizeof(WaterGeometrySubpassInstance),
            .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE,
        },
    };
    uint32_t vertexAttributeDescriptionCount = 7;
    VkVertexInputAttributeDescription vertexAttributeDescriptions[] = {
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(WaterGeometrySubpassVertex, position),
        },
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(WaterGeometrySubpassVertex, color),
        },
        {
            .location = 2,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(WaterGeometrySubpassVertex, normal),
        },
        // Instance:
        {
            .location = 3,
            .binding = 1,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(WaterGeometrySubpassInstance, model) + sizeof(vec4) * 0,
        },
        {
            .location = 4,
            .binding = 1,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(WaterGeometrySubpassInstance, model) + sizeof(vec4) * 1,
        },
        {
            .location = 5,
            .binding = 1,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(WaterGeometrySubpassInstance, model) + sizeof(vec4) * 2,
        },
        {
            .location = 6,
            .binding = 1,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(WaterGeometrySubpassInstance, model) + sizeof(vec4) * 3,
        },
    };
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
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0,
        .depthBiasClamp = VK_FALSE,
        .depthBiasSlopeFactor = 0,
        .lineWidth = 1.0f,
    };
    VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 0,
        .pSampleMask = NULL,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
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

    VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentStates[] = {
        {
            .blendEnable = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT,
        },
        {
            .blendEnable = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT,
        }};

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 2,
        .pAttachments = pipelineColorBlendAttachmentStates,
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

    VkDescriptorSetLayoutBinding *bindings = (VkDescriptorSetLayoutBinding[]){globalUniformLayoutBinding};
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = 1,
        .pBindings = bindings,
    };
    VkResult result = vkCreateDescriptorSetLayout(vkDevice, &descriptorSetLayoutCreateInfo, NULL, &pWaterGeometrySubpass->descriptorSetLayout);
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &pWaterGeometrySubpass->descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
    };

    result = vkCreatePipelineLayout(vkDevice, &pipelineLayoutCreateInfo, NULL, &pWaterGeometrySubpass->vkPipelineLayout);
    TryThrowVulkanError(result);
    VkGraphicsPipelineCreateInfo waterGeometryPipelineCreateInfo = {
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
        .pMultisampleState = &pipelineMultisampleStateCreateInfo,
        .pDepthStencilState = &pipelineDepthStencilStateCreateInfo,
        .pColorBlendState = &colorBlendStateCreateInfo,
        .pDynamicState = &dynamicState,
        .layout = pWaterGeometrySubpass->vkPipelineLayout,
        .renderPass = vkRenderPass,
        .subpass = waterGeometrySubpassIndex,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0,
    };

    VkPipelineCache pipelineCache = NULL;
    result = vkCreateGraphicsPipelines(vkDevice, pipelineCache, 1, &waterGeometryPipelineCreateInfo, NULL, &pWaterGeometrySubpass->vkPipeline);
    TryThrowVulkanError(result);
    DestroyVkShaderModule(vkDevice, waterGeometryVertShaderModule);
    DestroyVkShaderModule(vkDevice, waterGeometryFragShaderModule);
}
static void DestroyVkPipeline(Subpass *pWaterGeometrySubpass, VkDevice vkDevice)
{
    vkDestroyDescriptorSetLayout(vkDevice, pWaterGeometrySubpass->descriptorSetLayout, NULL);
    vkDestroyPipelineLayout(vkDevice, pWaterGeometrySubpass->vkPipelineLayout, NULL);
    vkDestroyPipeline(vkDevice, pWaterGeometrySubpass->vkPipeline, NULL);
}

void CreateWaterGeometrySubpass(Subpass *pWaterGeometrySubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t waterGeometrySubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor)
{
    CreateVkPipeline(pWaterGeometrySubpass, shadersPath, vkRenderPass, waterGeometrySubpassIndex, vkDevice, viewport, scissor);

    pWaterGeometrySubpass->vkDescriptorPoolSizeCount = 1;
    pWaterGeometrySubpass->vkDescriptorPoolSizes = TickernelMalloc(sizeof(VkDescriptorPoolSize) * pWaterGeometrySubpass->vkDescriptorPoolSizeCount);
    pWaterGeometrySubpass->vkDescriptorPoolSizes[0] = (VkDescriptorPoolSize){
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
    };

    TickernelCreateCollection(&pWaterGeometrySubpass->modelCollection, sizeof(SubpassModel), 1);
}
void DestroyWaterGeometrySubpass(Subpass *pWaterGeometrySubpass, VkDevice vkDevice)
{
    for (uint32_t i = pWaterGeometrySubpass->modelCollection.length - 1; i != UINT32_MAX; i--)
    {
        RemoveModelFromWaterGeometrySubpass(pWaterGeometrySubpass, vkDevice, i);
    }
    TickernelDestroyCollection(&pWaterGeometrySubpass->modelCollection);

    TickernelFree(pWaterGeometrySubpass->vkDescriptorPoolSizes);
    DestroyVkPipeline(pWaterGeometrySubpass, vkDevice);
}

void AddModelToWaterGeometrySubpass(Subpass *pWaterGeometrySubpass, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, uint32_t vertexCount, WaterGeometrySubpassVertex *waterGeometrySubpassVertices, uint32_t *pIndex)
{
    SubpassModel subpassModel = {
        .vertexCount = vertexCount,
        .vertexBuffer = NULL,
        .vertexBufferMemory = NULL,

        .maxInstanceCount = 0,
        .instanceCount = 0,
        .instanceBuffer = NULL,
        .instanceBufferMemory = NULL,

        .modelUniformBuffer = NULL,
        .modelUniformBufferMemory = NULL,
        .modelUniformBufferMapped = NULL,

        .vkDescriptorPool = NULL,
        .vkDescriptorSet = NULL,
    };
    VkDeviceSize vertexBufferSize = sizeof(WaterGeometrySubpassVertex) * vertexCount;
    CreateBuffer(vkDevice, vkPhysicalDevice, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &subpassModel.vertexBuffer, &subpassModel.vertexBufferMemory);
    UpdateBufferWithStagingBuffer(vkDevice, vkPhysicalDevice, 0, vertexBufferSize, waterGeometrySubpassVertices, graphicVkCommandPool, vkGraphicQueue, subpassModel.vertexBuffer);
    // Create vkDescriptorPool
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = 1,
        .poolSizeCount = pWaterGeometrySubpass->vkDescriptorPoolSizeCount,
        .pPoolSizes = pWaterGeometrySubpass->vkDescriptorPoolSizes,
    };
    VkResult result = vkCreateDescriptorPool(vkDevice, &descriptorPoolCreateInfo, NULL, &subpassModel.vkDescriptorPool);
    TryThrowVulkanError(result);

    // Create vkDescriptorSet
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = subpassModel.vkDescriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &pWaterGeometrySubpass->descriptorSetLayout,
    };
    result = vkAllocateDescriptorSets(vkDevice, &descriptorSetAllocateInfo, &subpassModel.vkDescriptorSet);
    TryThrowVulkanError(result);

    VkDescriptorBufferInfo globalDescriptorBufferInfo = {
        .buffer = globalUniformBuffer,
        .offset = 0,
        .range = sizeof(GlobalUniformBuffer),
    };
    VkWriteDescriptorSet descriptorWrites[1] = {
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = NULL,
            .dstSet = subpassModel.vkDescriptorSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo = NULL,
            .pBufferInfo = &globalDescriptorBufferInfo,
            .pTexelBufferView = NULL,
        },
    };
    vkUpdateDescriptorSets(vkDevice, 1, descriptorWrites, 0, NULL);
    TickernelAddToCollection(&pWaterGeometrySubpass->modelCollection, &subpassModel, pIndex);
}
void RemoveModelFromWaterGeometrySubpass(Subpass *pWaterGeometrySubpass, VkDevice vkDevice, uint32_t index)
{
    SubpassModel *pSubpassModel = pWaterGeometrySubpass->modelCollection.array[index];
    if (pSubpassModel->maxInstanceCount > 0)
    {
        DestroyBuffer(vkDevice, pSubpassModel->instanceBuffer, pSubpassModel->instanceBufferMemory);
    }
    DestroyBuffer(vkDevice, pSubpassModel->modelUniformBuffer, pSubpassModel->modelUniformBufferMemory);
    DestroyBuffer(vkDevice, pSubpassModel->vertexBuffer, pSubpassModel->vertexBufferMemory);

    VkResult result = vkFreeDescriptorSets(vkDevice, pSubpassModel->vkDescriptorPool, 1, &pSubpassModel->vkDescriptorSet);
    TryThrowVulkanError(result);
    vkDestroyDescriptorPool(vkDevice, pSubpassModel->vkDescriptorPool, NULL);
    TickernelRemoveFromCollection(&pWaterGeometrySubpass->modelCollection, index);
}
void UpdateInstancesInWaterGeometrySubpass(Subpass *pWaterGeometrySubpass, uint32_t modelIndex, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, WaterGeometrySubpassInstance *waterGeometrySubpassInstances, uint32_t instanceCount)
{
    SubpassModel *pSubpassModel = pWaterGeometrySubpass->modelCollection.array[modelIndex];
    if (0 == pSubpassModel->maxInstanceCount)
    {
        pSubpassModel->maxInstanceCount = instanceCount;
        pSubpassModel->instanceCount = instanceCount;
        VkDeviceSize instanceBufferSize = sizeof(WaterGeometrySubpassInstance) * pSubpassModel->instanceCount;
        CreateBuffer(vkDevice, vkPhysicalDevice, instanceBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pSubpassModel->instanceBuffer, &pSubpassModel->instanceBufferMemory);
        UpdateBuffer(vkDevice, pSubpassModel->instanceBufferMemory, 0, sizeof(WaterGeometrySubpassInstance) * instanceCount, waterGeometrySubpassInstances);
    }
    else if (instanceCount <= pSubpassModel->maxInstanceCount)
    {
        pSubpassModel->instanceCount = instanceCount;
        VkDeviceSize bufferSize = sizeof(WaterGeometrySubpassInstance) * pSubpassModel->instanceCount;
        UpdateBuffer(vkDevice, pSubpassModel->instanceBufferMemory, 0, bufferSize, waterGeometrySubpassInstances);
    }
    else
    {
        DestroyBuffer(vkDevice, pSubpassModel->instanceBuffer, pSubpassModel->instanceBufferMemory);
        pSubpassModel->maxInstanceCount = instanceCount;
        pSubpassModel->instanceCount = instanceCount;
        VkDeviceSize instanceBufferSize = sizeof(WaterGeometrySubpassInstance) * pSubpassModel->instanceCount;
        CreateBuffer(vkDevice, vkPhysicalDevice, instanceBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pSubpassModel->instanceBuffer, &pSubpassModel->instanceBufferMemory);
        UpdateBuffer(vkDevice, pSubpassModel->instanceBufferMemory, 0, sizeof(WaterGeometrySubpassInstance) * instanceCount, waterGeometrySubpassInstances);
    }
}
