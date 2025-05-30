#include "geometrySubpass.h"

void createGeometrySubpass(Subpass *pGeometrySubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t geometrySubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor)
{
    pGeometrySubpass->pipelineCount = 2;
    pGeometrySubpass->pipelines = tickernelMalloc(sizeof(Pipeline) * pGeometrySubpass->pipelineCount);

    char *vertShaderPaths[] = {
        "geometry.vert.spv",
        "geometry.frag.spv",
    };

    char *fragShaderPaths[] = {
        "geometry.frag.spv",
        "geometry.vert.spv",
    };

    VkVertexInputBindingDescription vertexBindingDescriptions[] = {
        {
            .binding = 0,
            .stride = sizeof(GeometrySubpassVertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        },
        {
            .binding = 1,
            .stride = sizeof(GeometrySubpassInstance),
            .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE,
        },
    };

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
        },
        {
            .location = 2,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(GeometrySubpassVertex, normal),
        },
        // Instance:
        {
            .location = 3,
            .binding = 1,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(GeometrySubpassInstance, model) + sizeof(vec4) * 0,
        },
        {
            .location = 4,
            .binding = 1,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(GeometrySubpassInstance, model) + sizeof(vec4) * 1,
        },
        {
            .location = 5,
            .binding = 1,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(GeometrySubpassInstance, model) + sizeof(vec4) * 2,
        },
        {
            .location = 6,
            .binding = 1,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(GeometrySubpassInstance, model) + sizeof(vec4) * 3,
        },
    };
    uint32_t vertexAttributeDescriptionCount = ARRAY_SIZE(vertexAttributeDescriptions);

    VkPipelineVertexInputStateCreateInfo vkPipelineVertexInputStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .vertexBindingDescriptionCount = ARRAY_SIZE(vertexBindingDescriptions),
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
        .attachmentCount = ARRAY_SIZE(pipelineColorBlendAttachmentStates),
        .pAttachments = pipelineColorBlendAttachmentStates,
        .blendConstants[0] = 0.0f,
        .blendConstants[1] = 0.0f,
        .blendConstants[2] = 0.0f,
        .blendConstants[3] = 0.0f,
    };

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
    VkPipelineDynamicStateCreateInfo dynamicState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .dynamicStateCount = ARRAY_SIZE(dynamicStates),
        .pDynamicStates = dynamicStates,
    };

    VkDescriptorSetLayoutBinding globalUniformLayoutBinding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = NULL,
    };

    VkDescriptorSetLayoutBinding bindings[] = {globalUniformLayoutBinding};
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = ARRAY_SIZE(bindings),
        .pBindings = bindings,
    };

    for (uint32_t i = 0; i < pGeometrySubpass->pipelineCount; i++)
    {
        Pipeline *pPipeline = &pGeometrySubpass->pipelines[i];

        VkShaderModule geometryVertShaderModule;
        char geometryVertShaderPath[FILENAME_MAX];
        sprintf(geometryVertShaderPath, "%s/%s", shadersPath, vertShaderPaths[i]);
        createVkShaderModule(vkDevice, geometryVertShaderPath, &geometryVertShaderModule);
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
        char geometryFragShaderPath[FILENAME_MAX];
        sprintf(geometryFragShaderPath, "%s/%s", shadersPath, fragShaderPaths[i]);
        createVkShaderModule(vkDevice, geometryFragShaderPath, &geometryFragShaderModule);
        VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = geometryFragShaderModule,
            .pName = "main",
            .pSpecializationInfo = NULL,
        };

        VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfos[] = {vertShaderStageCreateInfo, fragShaderStageCreateInfo};
        uint32_t stageCount = ARRAY_SIZE(pipelineShaderStageCreateInfos);

        VkResult result = vkCreateDescriptorSetLayout(vkDevice, &descriptorSetLayoutCreateInfo, NULL, &pPipeline->descriptorSetLayout);
        tryThrowVulkanError(result);

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .setLayoutCount = 1,
            .pSetLayouts = &pPipeline->descriptorSetLayout,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = NULL,
        };

        result = vkCreatePipelineLayout(vkDevice, &pipelineLayoutCreateInfo, NULL, &pPipeline->vkPipelineLayout);
        tryThrowVulkanError(result);

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
            .pMultisampleState = &pipelineMultisampleStateCreateInfo,
            .pDepthStencilState = &pipelineDepthStencilStateCreateInfo,
            .pColorBlendState = &colorBlendStateCreateInfo,
            .pDynamicState = &dynamicState,
            .layout = pPipeline->vkPipelineLayout,
            .renderPass = vkRenderPass,
            .subpass = geometrySubpassIndex,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = 0,
        };

        VkPipelineCache pipelineCache = NULL;
        result = vkCreateGraphicsPipelines(vkDevice, pipelineCache, 1, &geometryPipelineCreateInfo, NULL, &pPipeline->vkPipeline);
        tryThrowVulkanError(result);
        destroyVkShaderModule(vkDevice, geometryVertShaderModule);
        destroyVkShaderModule(vkDevice, geometryFragShaderModule);

        pPipeline->vkDescriptorPoolSizeCount = 1;
        pPipeline->vkDescriptorPoolSizes = tickernelMalloc(sizeof(VkDescriptorPoolSize) * pPipeline->vkDescriptorPoolSizeCount);
        pPipeline->vkDescriptorPoolSizes[0] = (VkDescriptorPoolSize){
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
        };

        tickernelCreateDynamicArray(&pPipeline->modelDynamicArray, 1, sizeof(SubpassModel));
    }
}

void destroyGeometrySubpass(Subpass *pGeometrySubpass, VkDevice vkDevice)
{
    for (uint32_t i = 0; i < pGeometrySubpass->pipelineCount; i++)
    {
        Pipeline *pPipeline = &pGeometrySubpass->pipelines[i];

        for (uint32_t i = pPipeline->modelDynamicArray.length - 1; i > -1; i--)
        {
            SubpassModel *pSubpassModel = pPipeline->modelDynamicArray.array[i];
            removeModelFromGeometrySubpass(pPipeline, vkDevice, pSubpassModel);
        }
        tickernelDestroyDynamicArray(&pPipeline->modelDynamicArray);

        tickernelFree(pPipeline->vkDescriptorPoolSizes);

        vkDestroyDescriptorSetLayout(vkDevice, pPipeline->descriptorSetLayout, NULL);
        vkDestroyPipelineLayout(vkDevice, pPipeline->vkPipelineLayout, NULL);
        vkDestroyPipeline(vkDevice, pPipeline->vkPipeline, NULL);
    }
    tickernelFree(pGeometrySubpass->pipelines);
}

SubpassModel *addModelToGeometrySubpass(Subpass *pGeometrySubpass, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, uint32_t vertexCount, GeometrySubpassVertex *geometrySubpassVertices)
{
    SubpassModel subpassModel = {
        .pMesh = NULL,

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
    VkDeviceSize vertexBufferSize = sizeof(GeometrySubpassVertex) * vertexCount;
    createBuffer(vkDevice, vkPhysicalDevice, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &subpassModel.vertexBuffer, &subpassModel.vertexBufferMemory);
    updateBufferWithStagingBuffer(vkDevice, vkPhysicalDevice, 0, vertexBufferSize, geometrySubpassVertices, graphicVkCommandPool, vkGraphicQueue, subpassModel.vertexBuffer);
    // Create vkDescriptorPool
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = 1,
        .poolSizeCount = pGeometrySubpass->vkDescriptorPoolSizeCount,
        .pPoolSizes = pGeometrySubpass->vkDescriptorPoolSizes,
    };
    VkResult result = vkCreateDescriptorPool(vkDevice, &descriptorPoolCreateInfo, NULL, &subpassModel.vkDescriptorPool);
    tryThrowVulkanError(result);

    // Create vkDescriptorSet
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = subpassModel.vkDescriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &pGeometrySubpass->descriptorSetLayout,
    };
    result = vkAllocateDescriptorSets(vkDevice, &descriptorSetAllocateInfo, &subpassModel.vkDescriptorSet);
    tryThrowVulkanError(result);

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
    return tickernelAddToDynamicArray(&pGeometrySubpass->modelDynamicArray, &subpassModel, pGeometrySubpass->modelDynamicArray.length);
}
void removeModelFromGeometrySubpass(Subpass *pGeometrySubpass, VkDevice vkDevice, SubpassModel *pSubpassModel)
{
    if (pSubpassModel->maxInstanceCount > 0)
    {
        destroyBuffer(vkDevice, pSubpassModel->instanceBuffer, pSubpassModel->instanceBufferMemory);
    }
    destroyBuffer(vkDevice, pSubpassModel->modelUniformBuffer, pSubpassModel->modelUniformBufferMemory);
    destroyBuffer(vkDevice, pSubpassModel->vertexBuffer, pSubpassModel->vertexBufferMemory);

    VkResult result = vkFreeDescriptorSets(vkDevice, pSubpassModel->vkDescriptorPool, 1, &pSubpassModel->vkDescriptorSet);
    tryThrowVulkanError(result);
    vkDestroyDescriptorPool(vkDevice, pSubpassModel->vkDescriptorPool, NULL);
    tickernelRemoveFromDynamicArray(&pGeometrySubpass->modelDynamicArray, pSubpassModel);
}

void updateInstancesInGeometrySubpass(Subpass *pGeometrySubpass, SubpassModel *pSubpassModel, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, GeometrySubpassInstance *geometrySubpassInstances, uint32_t instanceCount)
{
    if (0 == pSubpassModel->maxInstanceCount)
    {
        pSubpassModel->maxInstanceCount = instanceCount;
        pSubpassModel->instanceCount = instanceCount;
        VkDeviceSize instanceBufferSize = sizeof(GeometrySubpassInstance) * pSubpassModel->instanceCount;
        createBuffer(vkDevice, vkPhysicalDevice, instanceBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pSubpassModel->instanceBuffer, &pSubpassModel->instanceBufferMemory);
        updateBuffer(vkDevice, pSubpassModel->instanceBufferMemory, 0, sizeof(GeometrySubpassInstance) * instanceCount, geometrySubpassInstances);
    }
    else if (instanceCount <= pSubpassModel->maxInstanceCount)
    {
        pSubpassModel->instanceCount = instanceCount;
        VkDeviceSize bufferSize = sizeof(GeometrySubpassInstance) * pSubpassModel->instanceCount;
        updateBuffer(vkDevice, pSubpassModel->instanceBufferMemory, 0, bufferSize, geometrySubpassInstances);
    }
    else
    {
        destroyBuffer(vkDevice, pSubpassModel->instanceBuffer, pSubpassModel->instanceBufferMemory);
        pSubpassModel->maxInstanceCount = instanceCount;
        pSubpassModel->instanceCount = instanceCount;
        VkDeviceSize instanceBufferSize = sizeof(GeometrySubpassInstance) * pSubpassModel->instanceCount;
        createBuffer(vkDevice, vkPhysicalDevice, instanceBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pSubpassModel->instanceBuffer, &pSubpassModel->instanceBufferMemory);
        updateBuffer(vkDevice, pSubpassModel->instanceBufferMemory, 0, sizeof(GeometrySubpassInstance) * instanceCount, geometrySubpassInstances);
    }
}
