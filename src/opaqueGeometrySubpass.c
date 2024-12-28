#include <opaqueGeometrySubpass.h>
static void CreateVkPipeline(Subpass *pOpaqueGeometrySubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t opaqueGeometrySubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor)
{
    VkShaderModule opaqueGeometryVertShaderModule;
    char opaqueGeometryVertShaderPath[FILENAME_MAX];
    strcpy(opaqueGeometryVertShaderPath, shadersPath);
    TickernelCombinePaths(opaqueGeometryVertShaderPath, FILENAME_MAX, "opaqueGeometry.vert.spv");
    CreateVkShaderModule(vkDevice, opaqueGeometryVertShaderPath, &opaqueGeometryVertShaderModule);
    VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = opaqueGeometryVertShaderModule,
        .pName = "main",
        .pSpecializationInfo = NULL,
    };

    VkShaderModule opaqueGeometryFragShaderModule;
    char opaqueGeometryFragShaderPath[FILENAME_MAX];
    strcpy(opaqueGeometryFragShaderPath, shadersPath);
    TickernelCombinePaths(opaqueGeometryFragShaderPath, FILENAME_MAX, "opaqueGeometry.frag.spv");
    CreateVkShaderModule(vkDevice, opaqueGeometryFragShaderPath, &opaqueGeometryFragShaderModule);
    VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = opaqueGeometryFragShaderModule,
        .pName = "main",
        .pSpecializationInfo = NULL,
    };
    uint32_t stageCount = 2;
    VkPipelineShaderStageCreateInfo *pipelineShaderStageCreateInfos = (VkPipelineShaderStageCreateInfo[]){vertShaderStageCreateInfo, fragShaderStageCreateInfo};

    uint32_t vertexBindingDescriptionCount = 2;
    VkVertexInputBindingDescription *vertexBindingDescriptions = (VkVertexInputBindingDescription[]){
        {
            .binding = 0,
            .stride = sizeof(OpaqueGeometrySubpassVertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        },
        {
            .binding = 1,
            .stride = sizeof(OpaqueGeometrySubpassInstance),
            .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE,
        },
    };
    uint32_t vertexAttributeDescriptionCount = 7;
    VkVertexInputAttributeDescription vertexAttributeDescriptions[] = {
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(OpaqueGeometrySubpassVertex, position),
        },
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(OpaqueGeometrySubpassVertex, color),
        },
        {
            .location = 2,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(OpaqueGeometrySubpassVertex, normal),
        },
        // Instance:
        {
            .location = 3,
            .binding = 1,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(OpaqueGeometrySubpassInstance, model) + sizeof(vec4) * 0,
        },
        {
            .location = 4,
            .binding = 1,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(OpaqueGeometrySubpassInstance, model) + sizeof(vec4) * 1,
        },
        {
            .location = 5,
            .binding = 1,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(OpaqueGeometrySubpassInstance, model) + sizeof(vec4) * 2,
        },
        {
            .location = 6,
            .binding = 1,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(OpaqueGeometrySubpassInstance, model) + sizeof(vec4) * 3,
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
            .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
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
    VkResult result = vkCreateDescriptorSetLayout(vkDevice, &descriptorSetLayoutCreateInfo, NULL, &pOpaqueGeometrySubpass->descriptorSetLayout);
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &pOpaqueGeometrySubpass->descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
    };

    result = vkCreatePipelineLayout(vkDevice, &pipelineLayoutCreateInfo, NULL, &pOpaqueGeometrySubpass->vkPipelineLayout);
    TryThrowVulkanError(result);
    VkGraphicsPipelineCreateInfo opaqueGeometryPipelineCreateInfo = {
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
        .layout = pOpaqueGeometrySubpass->vkPipelineLayout,
        .renderPass = vkRenderPass,
        .subpass = opaqueGeometrySubpassIndex,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0,
    };

    VkPipelineCache pipelineCache = NULL;
    result = vkCreateGraphicsPipelines(vkDevice, pipelineCache, 1, &opaqueGeometryPipelineCreateInfo, NULL, &pOpaqueGeometrySubpass->vkPipeline);
    TryThrowVulkanError(result);
    DestroyVkShaderModule(vkDevice, opaqueGeometryVertShaderModule);
    DestroyVkShaderModule(vkDevice, opaqueGeometryFragShaderModule);
}
static void DestroyVkPipeline(Subpass *pOpaqueGeometrySubpass, VkDevice vkDevice)
{
    vkDestroyDescriptorSetLayout(vkDevice, pOpaqueGeometrySubpass->descriptorSetLayout, NULL);
    vkDestroyPipelineLayout(vkDevice, pOpaqueGeometrySubpass->vkPipelineLayout, NULL);
    vkDestroyPipeline(vkDevice, pOpaqueGeometrySubpass->vkPipeline, NULL);
}

void CreateOpaqueGeometrySubpass(Subpass *pOpaqueGeometrySubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t opaqueGeometrySubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor)
{
    CreateVkPipeline(pOpaqueGeometrySubpass, shadersPath, vkRenderPass, opaqueGeometrySubpassIndex, vkDevice, viewport, scissor);

    pOpaqueGeometrySubpass->vkDescriptorPoolSizeCount = 1;
    pOpaqueGeometrySubpass->vkDescriptorPoolSizes = TickernelMalloc(sizeof(VkDescriptorPoolSize) * pOpaqueGeometrySubpass->vkDescriptorPoolSizeCount);
    pOpaqueGeometrySubpass->vkDescriptorPoolSizes[0] = (VkDescriptorPoolSize){
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
    };

    TickernelCreateCollection(&pOpaqueGeometrySubpass->modelCollection, sizeof(SubpassModel), 1);
}
void DestroyOpaqueGeometrySubpass(Subpass *pOpaqueGeometrySubpass, VkDevice vkDevice)
{
    for (uint32_t i = pOpaqueGeometrySubpass->modelCollection.length - 1; i != UINT32_MAX; i--)
    {
        RemoveModelFromOpaqueGeometrySubpass(pOpaqueGeometrySubpass, vkDevice, i);
    }
    TickernelDestroyCollection(&pOpaqueGeometrySubpass->modelCollection);

    TickernelFree(pOpaqueGeometrySubpass->vkDescriptorPoolSizes);
    DestroyVkPipeline(pOpaqueGeometrySubpass, vkDevice);
}

void AddModelToOpaqueGeometrySubpass(Subpass *pOpaqueGeometrySubpass, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, uint32_t vertexCount, OpaqueGeometrySubpassVertex *opaqueGeometrySubpassVertices, uint32_t *pIndex)
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
    VkDeviceSize vertexBufferSize = sizeof(OpaqueGeometrySubpassVertex) * vertexCount;
    CreateBuffer(vkDevice, vkPhysicalDevice, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &subpassModel.vertexBuffer, &subpassModel.vertexBufferMemory);
    UpdateBufferWithStagingBuffer(vkDevice, vkPhysicalDevice, 0, vertexBufferSize, opaqueGeometrySubpassVertices, graphicVkCommandPool, vkGraphicQueue, subpassModel.vertexBuffer);
    // Create vkDescriptorPool
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = 1,
        .poolSizeCount = pOpaqueGeometrySubpass->vkDescriptorPoolSizeCount,
        .pPoolSizes = pOpaqueGeometrySubpass->vkDescriptorPoolSizes,
    };
    VkResult result = vkCreateDescriptorPool(vkDevice, &descriptorPoolCreateInfo, NULL, &subpassModel.vkDescriptorPool);
    TryThrowVulkanError(result);

    // Create vkDescriptorSet
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = subpassModel.vkDescriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &pOpaqueGeometrySubpass->descriptorSetLayout,
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
    TickernelAddToCollection(&pOpaqueGeometrySubpass->modelCollection, &subpassModel, pIndex);
}
void RemoveModelFromOpaqueGeometrySubpass(Subpass *pOpaqueGeometrySubpass, VkDevice vkDevice, uint32_t index)
{
    SubpassModel *pSubpassModel = pOpaqueGeometrySubpass->modelCollection.array[index];
    if (pSubpassModel->maxInstanceCount > 0)
    {
        DestroyBuffer(vkDevice, pSubpassModel->instanceBuffer, pSubpassModel->instanceBufferMemory);
    }
    DestroyBuffer(vkDevice, pSubpassModel->modelUniformBuffer, pSubpassModel->modelUniformBufferMemory);
    DestroyBuffer(vkDevice, pSubpassModel->vertexBuffer, pSubpassModel->vertexBufferMemory);

    VkResult result = vkFreeDescriptorSets(vkDevice, pSubpassModel->vkDescriptorPool, 1, &pSubpassModel->vkDescriptorSet);
    TryThrowVulkanError(result);
    vkDestroyDescriptorPool(vkDevice, pSubpassModel->vkDescriptorPool, NULL);
    TickernelRemoveFromCollection(&pOpaqueGeometrySubpass->modelCollection, index);
}
void UpdateInstancesInOpaqueGeometrySubpass(Subpass *pOpaqueGeometrySubpass, uint32_t modelIndex, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, OpaqueGeometrySubpassInstance *opaqueGeometrySubpassInstances, uint32_t instanceCount)
{
    SubpassModel *pSubpassModel = pOpaqueGeometrySubpass->modelCollection.array[modelIndex];
    if (0 == pSubpassModel->maxInstanceCount)
    {
        pSubpassModel->maxInstanceCount = instanceCount;
        pSubpassModel->instanceCount = instanceCount;
        VkDeviceSize instanceBufferSize = sizeof(OpaqueGeometrySubpassInstance) * pSubpassModel->instanceCount;
        CreateBuffer(vkDevice, vkPhysicalDevice, instanceBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pSubpassModel->instanceBuffer, &pSubpassModel->instanceBufferMemory);
        UpdateBuffer(vkDevice, pSubpassModel->instanceBufferMemory, 0, sizeof(OpaqueGeometrySubpassInstance) * instanceCount, opaqueGeometrySubpassInstances);
    }
    else if (instanceCount <= pSubpassModel->maxInstanceCount)
    {
        pSubpassModel->instanceCount = instanceCount;
        VkDeviceSize bufferSize = sizeof(OpaqueGeometrySubpassInstance) * pSubpassModel->instanceCount;
        UpdateBuffer(vkDevice, pSubpassModel->instanceBufferMemory, 0, bufferSize, opaqueGeometrySubpassInstances);
    }
    else
    {
        DestroyBuffer(vkDevice, pSubpassModel->instanceBuffer, pSubpassModel->instanceBufferMemory);
        pSubpassModel->maxInstanceCount = instanceCount;
        pSubpassModel->instanceCount = instanceCount;
        VkDeviceSize instanceBufferSize = sizeof(OpaqueGeometrySubpassInstance) * pSubpassModel->instanceCount;
        CreateBuffer(vkDevice, vkPhysicalDevice, instanceBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pSubpassModel->instanceBuffer, &pSubpassModel->instanceBufferMemory);
        UpdateBuffer(vkDevice, pSubpassModel->instanceBufferMemory, 0, sizeof(OpaqueGeometrySubpassInstance) * instanceCount, opaqueGeometrySubpassInstances);
    }
}
