#include "uiSubpass.h"

static void createVkPipeline(Subpass *pUISubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t uiSubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor)
{
    VkShaderModule uiVertShaderModule;
    char uiVertShaderPath[FILENAME_MAX];
    strcpy(uiVertShaderPath, shadersPath);
    tickernelCombinePaths(uiVertShaderPath, FILENAME_MAX, "ui.vert.spv");
    createVkShaderModule(vkDevice, uiVertShaderPath, &uiVertShaderModule);
    VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = uiVertShaderModule,
        .pName = "main",
        .pSpecializationInfo = NULL,
    };

    VkShaderModule uiFragShaderModule;
    char uiFragShaderPath[FILENAME_MAX];
    strcpy(uiFragShaderPath, shadersPath);
    tickernelCombinePaths(uiFragShaderPath, FILENAME_MAX, "ui.frag.spv");
    createVkShaderModule(vkDevice, uiFragShaderPath, &uiFragShaderModule);
    VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = uiFragShaderModule,
        .pName = "main",
        .pSpecializationInfo = NULL,
    };
    uint32_t stageCount = 2;
    VkPipelineShaderStageCreateInfo *pipelineShaderStageCreateInfos = (VkPipelineShaderStageCreateInfo[]){vertShaderStageCreateInfo, fragShaderStageCreateInfo};

    uint32_t vertexBindingDescriptionCount = 1;
    VkVertexInputBindingDescription *vertexBindingDescriptions = (VkVertexInputBindingDescription[]){
        {
            .binding = 0,
            .stride = sizeof(UISubpassVertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        }
    };
    uint32_t vertexAttributeDescriptionCount = 3;
    VkVertexInputAttributeDescription vertexAttributeDescriptions[] = {
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(UISubpassVertex, position),
        },
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(UISubpassVertex, texCoord),
        },
        {
            .location = 2,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(UISubpassVertex, color),
        }
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
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
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
        .cullMode = VK_CULL_MODE_NONE,
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
        .depthTestEnable = VK_FALSE,
        .depthWriteEnable = VK_FALSE,
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
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
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

    VkDescriptorSetLayoutBinding *bindings = (VkDescriptorSetLayoutBinding[]){globalUniformLayoutBinding};
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = 1,
        .pBindings = bindings,
    };
    VkResult result = vkCreateDescriptorSetLayout(vkDevice, &descriptorSetLayoutCreateInfo, NULL, &pUISubpass->descriptorSetLayout);
    tryThrowVulkanError(result);
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &pUISubpass->descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
    };

    result = vkCreatePipelineLayout(vkDevice, &pipelineLayoutCreateInfo, NULL, &pUISubpass->vkPipelineLayout);
    tryThrowVulkanError(result);
    VkGraphicsPipelineCreateInfo uiPipelineCreateInfo = {
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
        .layout = pUISubpass->vkPipelineLayout,
        .renderPass = vkRenderPass,
        .subpass = uiSubpassIndex,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0,
    };

    VkPipelineCache pipelineCache = NULL;
    result = vkCreateGraphicsPipelines(vkDevice, pipelineCache, 1, &uiPipelineCreateInfo, NULL, &pUISubpass->vkPipeline);
    tryThrowVulkanError(result);
    destroyVkShaderModule(vkDevice, uiVertShaderModule);
    destroyVkShaderModule(vkDevice, uiFragShaderModule);
}

static void destroyVkPipeline(Subpass *pUISubpass, VkDevice vkDevice)
{
    vkDestroyDescriptorSetLayout(vkDevice, pUISubpass->descriptorSetLayout, NULL);
    vkDestroyPipelineLayout(vkDevice, pUISubpass->vkPipelineLayout, NULL);
    vkDestroyPipeline(vkDevice, pUISubpass->vkPipeline, NULL);
}

static void createUISubpassModel(Subpass *pUISubpass, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, uint32_t vertexCount, UISubpassVertex *uiSubpassVertices, uint32_t *pIndex)
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

    VkDeviceSize vertexBufferSize = sizeof(UISubpassVertex) * vertexCount;
    createBuffer(vkDevice, vkPhysicalDevice, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &subpassModel.vertexBuffer, &subpassModel.vertexBufferMemory);
    updateBufferWithStagingBuffer(vkDevice, vkPhysicalDevice, 0, vertexBufferSize, uiSubpassVertices, graphicVkCommandPool, vkGraphicQueue, subpassModel.vertexBuffer);
    
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = 1,
        .poolSizeCount = pUISubpass->vkDescriptorPoolSizeCount,
        .pPoolSizes = pUISubpass->vkDescriptorPoolSizes,
    };
    VkResult result = vkCreateDescriptorPool(vkDevice, &descriptorPoolCreateInfo, NULL, &subpassModel.vkDescriptorPool);
    tryThrowVulkanError(result);

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = subpassModel.vkDescriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &pUISubpass->descriptorSetLayout,
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
    tickernelAddToCollection(&pUISubpass->modelCollection, &subpassModel, pIndex);
}

static void destroyUISubpassModel(Subpass *pUISubpass, VkDevice vkDevice, uint32_t index)
{
    SubpassModel *pSubpassModel = pUISubpass->modelCollection.array[index];
    if (pSubpassModel->maxInstanceCount > 0)
    {
        destroyBuffer(vkDevice, pSubpassModel->instanceBuffer, pSubpassModel->instanceBufferMemory);
    }
    destroyBuffer(vkDevice, pSubpassModel->modelUniformBuffer, pSubpassModel->modelUniformBufferMemory);
    destroyBuffer(vkDevice, pSubpassModel->vertexBuffer, pSubpassModel->vertexBufferMemory);

    VkResult result = vkFreeDescriptorSets(vkDevice, pSubpassModel->vkDescriptorPool, 1, &pSubpassModel->vkDescriptorSet);
    tryThrowVulkanError(result);
    vkDestroyDescriptorPool(vkDevice, pSubpassModel->vkDescriptorPool, NULL);
    tickernelRemoveFromCollection(&pUISubpass->modelCollection, index);
}

void createUISubpass(Subpass *pUISubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t uiSubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor)
{
    createVkPipeline(pUISubpass, shadersPath, vkRenderPass, uiSubpassIndex, vkDevice, viewport, scissor);

    pUISubpass->vkDescriptorPoolSizeCount = 1;
    pUISubpass->vkDescriptorPoolSizes = tickernelMalloc(sizeof(VkDescriptorPoolSize) * pUISubpass->vkDescriptorPoolSizeCount);
    pUISubpass->vkDescriptorPoolSizes[0] = (VkDescriptorPoolSize){
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
    };

    tickernelCreateCollection(&pUISubpass->modelCollection, sizeof(SubpassModel), 1);
}

void destroyUISubpass(Subpass *pUISubpass, VkDevice vkDevice)
{
    for (uint32_t i = pUISubpass->modelCollection.length - 1; i != UINT32_MAX; i--)
    {
        destroyUISubpassModel(pUISubpass, vkDevice, i);
    }
    tickernelDestroyCollection(&pUISubpass->modelCollection);

    tickernelFree(pUISubpass->vkDescriptorPoolSizes);
    destroyVkPipeline(pUISubpass, vkDevice);
}

void addModelToUISubpass(Subpass *pUISubpass, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, uint32_t vertexCount, UISubpassVertex *uiSubpassVertices, uint32_t *pIndex)
{
    createUISubpassModel(pUISubpass, vkDevice, vkPhysicalDevice, graphicVkCommandPool, vkGraphicQueue, globalUniformBuffer, vertexCount, uiSubpassVertices, pIndex);
}

void removeModelFromUISubpass(Subpass *pUISubpass, VkDevice vkDevice, uint32_t index)
{
    destroyUISubpassModel(pUISubpass, vkDevice, index);
}

void updateModelInUISubpass(Subpass *pUISubpass, uint32_t modelIndex, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, UISubpassVertex *uiSubpassVertices, uint32_t vertexCount)
{
    SubpassModel *pSubpassModel = pUISubpass->modelCollection.array[modelIndex];
    if (vertexCount != pSubpassModel->vertexCount)
    {
        destroyBuffer(vkDevice, pSubpassModel->vertexBuffer, pSubpassModel->vertexBufferMemory);
        pSubpassModel->vertexCount = vertexCount;
        VkDeviceSize vertexBufferSize = sizeof(UISubpassVertex) * vertexCount;
        createBuffer(vkDevice, vkPhysicalDevice, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &pSubpassModel->vertexBuffer, &pSubpassModel->vertexBufferMemory);
    }
    updateBufferWithStagingBuffer(vkDevice, vkPhysicalDevice, 0, sizeof(UISubpassVertex) * vertexCount, uiSubpassVertices, graphicVkCommandPool, vkGraphicQueue, pSubpassModel->vertexBuffer);
} 