#include <lightingSubpass.h>

static void CreateVkPipeline(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t lightingSubpassIndex = 1;
    Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];

    VkShaderModule lightingVertShaderModule;
    char lightingVertShaderPath[FILENAME_MAX];
    strcpy(lightingVertShaderPath, pGraphicEngine->assetsPath);
    TickernelCombinePaths(lightingVertShaderPath, FILENAME_MAX, "shaders");
    TickernelCombinePaths(lightingVertShaderPath, FILENAME_MAX, "lighting.vert.spv");
    CreateVkShaderModule(pGraphicEngine->vkDevice, lightingVertShaderPath, &lightingVertShaderModule);
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
    char lightingFragShaderPath[FILENAME_MAX];
    strcpy(lightingFragShaderPath, pGraphicEngine->assetsPath);
    TickernelCombinePaths(lightingFragShaderPath, FILENAME_MAX, "shaders");
    TickernelCombinePaths(lightingFragShaderPath, FILENAME_MAX, "lighting.frag.spv");
    CreateVkShaderModule(pGraphicEngine->vkDevice, lightingFragShaderPath, &lightingFragShaderModule);
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
        .width = pGraphicEngine->width,
        .height = pGraphicEngine->height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    VkOffset2D offset = {
        .x = 0,
        .y = 0,
    };
    VkExtent2D extent;
    extent.width = pGraphicEngine->width;
    extent.height = pGraphicEngine->height;
    VkRect2D scissor = {
        .offset = offset,
        .extent = extent,
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
        .depthCompareOp = VK_COMPARE_OP_ALWAYS,
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
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = NULL,
    };
    VkDescriptorSetLayoutBinding depthAttachmentLayoutBinding = {
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = NULL,
    };
    VkDescriptorSetLayoutBinding albedoAttachmentLayoutBinding = {
        .binding = 2,
        .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = NULL,
    };
    VkDescriptorSetLayoutBinding normalAttachmentLayoutBinding = {
        .binding = 3,
        .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = NULL,
    };
    VkDescriptorSetLayoutBinding *bindings = (VkDescriptorSetLayoutBinding[]){globalUniformLayoutBinding, depthAttachmentLayoutBinding, albedoAttachmentLayoutBinding, normalAttachmentLayoutBinding};
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = 4,
        .pBindings = bindings,
    };

    VkResult result = vkCreateDescriptorSetLayout(pGraphicEngine->vkDevice, &descriptorSetLayoutCreateInfo, NULL, &pLightingSubpass->descriptorSetLayout);
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &pLightingSubpass->descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
    };

    result = vkCreatePipelineLayout(pGraphicEngine->vkDevice, &pipelineLayoutCreateInfo, NULL, &pLightingSubpass->vkPipelineLayout);
    TryThrowVulkanError(result);
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
        .pMultisampleState = &pipelineMultisampleStateCreateInfo,
        .pDepthStencilState = &pipelineDepthStencilStateCreateInfo,
        .pColorBlendState = &colorBlendStateCreateInfo,
        .pDynamicState = &dynamicState,
        .layout = pLightingSubpass->vkPipelineLayout,
        .renderPass = pDeferredRenderPass->vkRenderPass,
        .subpass = lightingSubpassIndex,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    VkPipelineCache pipelineCache = NULL;
    result = vkCreateGraphicsPipelines(pGraphicEngine->vkDevice, pipelineCache, 1, &lightingPipelineCreateInfo, NULL, &pLightingSubpass->vkPipeline);
    TryThrowVulkanError(result);
    DestroyVkShaderModule(pGraphicEngine->vkDevice, lightingVertShaderModule);
    DestroyVkShaderModule(pGraphicEngine->vkDevice, lightingFragShaderModule);
}
static void DestroyVkPipeline(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t lightingSubpassIndex = 1;
    Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];
    vkDestroyDescriptorSetLayout(pGraphicEngine->vkDevice, pLightingSubpass->descriptorSetLayout, NULL);
    vkDestroyPipelineLayout(pGraphicEngine->vkDevice, pLightingSubpass->vkPipelineLayout, NULL);
    vkDestroyPipeline(pGraphicEngine->vkDevice, pLightingSubpass->vkPipeline, NULL);
}

static void CreateLightingSubpassModel(GraphicEngine *pGraphicEngine, uint32_t index)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t lightingSubpassIndex = 1;
    Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];

    SubpassModel *pSubpassModel = &pLightingSubpass->subpassModels[index];
    pSubpassModel->vertexCount = 3;
    pSubpassModel->vertexBuffer = NULL;
    pSubpassModel->vertexBufferMemory = NULL;

    pSubpassModel->modelUniformBuffer = NULL;
    pSubpassModel->modelUniformBufferMemory = NULL;
    pSubpassModel->modelUniformBufferMapped = NULL;

    // Create vkDescriptorSet
    uint32_t poolIndex = index / pLightingSubpass->modelCountPerDescriptorPool;
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = pLightingSubpass->vkDescriptorPools[poolIndex],
        .descriptorSetCount = 1,
        .pSetLayouts = &pLightingSubpass->descriptorSetLayout,
    };
    VkResult result = vkAllocateDescriptorSets(pGraphicEngine->vkDevice, &descriptorSetAllocateInfo, &pSubpassModel->vkDescriptorSet);
    TryThrowVulkanError(result);
    VkDescriptorBufferInfo globalDescriptorBufferInfo = {
        .buffer = pGraphicEngine->globalUniformBuffer,
        .offset = 0,
        .range = sizeof(GlobalUniformBuffer),
    };
    VkDescriptorImageInfo depthVkDescriptorImageInfo = {
        .sampler = NULL,
        .imageView = pGraphicEngine->depthGraphicImage.vkImageView,
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
    };
    VkDescriptorImageInfo albedoVkDescriptorImageInfo = {
        .sampler = NULL,
        .imageView = pGraphicEngine->albedoGraphicImage.vkImageView,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    VkDescriptorImageInfo normalVkDescriptorImageInfo = {
        .sampler = NULL,
        .imageView = pGraphicEngine->normalGraphicImage.vkImageView,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    VkWriteDescriptorSet descriptorWrites[] = {
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = NULL,
            .dstSet = pSubpassModel->vkDescriptorSet,
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
            .dstSet = pSubpassModel->vkDescriptorSet,
            .dstBinding = 1,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
            .pImageInfo = &depthVkDescriptorImageInfo,
            .pBufferInfo = NULL,
            .pTexelBufferView = NULL,
        },
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = NULL,
            .dstSet = pSubpassModel->vkDescriptorSet,
            .dstBinding = 2,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
            .pImageInfo = &albedoVkDescriptorImageInfo,
            .pBufferInfo = NULL,
            .pTexelBufferView = NULL,
        },
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = NULL,
            .dstSet = pSubpassModel->vkDescriptorSet,
            .dstBinding = 3,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
            .pImageInfo = &normalVkDescriptorImageInfo,
            .pBufferInfo = NULL,
            .pTexelBufferView = NULL,
        },
    };
    vkUpdateDescriptorSets(pGraphicEngine->vkDevice, 4, descriptorWrites, 0, NULL);
    pSubpassModel->isValid = true;
}
static void DestroyLightingSubpassModel(GraphicEngine *pGraphicEngine, uint32_t index)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t lightingSubpassIndex = 1;
    Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];
    SubpassModel *pSubpassModel = &pLightingSubpass->subpassModels[index];
    pSubpassModel->isValid = false;

    uint32_t poolIndex = index / pLightingSubpass->modelCountPerDescriptorPool;
    VkResult result = vkFreeDescriptorSets(pGraphicEngine->vkDevice, pLightingSubpass->vkDescriptorPools[poolIndex], 1, &pSubpassModel->vkDescriptorSet);
    TryThrowVulkanError(result);

    DestroyVertexBuffer(pGraphicEngine, pSubpassModel->vertexBuffer, pSubpassModel->vertexBufferMemory);
}

void CreateLightingSubpass(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t lightingSubpassIndex = 1;
    Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];
    CreateVkPipeline(pGraphicEngine);

    pLightingSubpass->modelCountPerDescriptorPool = 1;
    pLightingSubpass->vkDescriptorPoolCount = 0;
    pLightingSubpass->vkDescriptorPools = NULL;

    pLightingSubpass->vkDescriptorPoolSizeCount = 2;
    pLightingSubpass->vkDescriptorPoolSizes = TickernelMalloc(sizeof(VkDescriptorPoolSize) * pLightingSubpass->vkDescriptorPoolSizeCount);
    pLightingSubpass->vkDescriptorPoolSizes[0] = (VkDescriptorPoolSize){
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = pLightingSubpass->modelCountPerDescriptorPool,
    };
    pLightingSubpass->vkDescriptorPoolSizes[1] = (VkDescriptorPoolSize){
        .type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
        .descriptorCount = pLightingSubpass->modelCountPerDescriptorPool * 3,
    };

    pLightingSubpass->subpassModelCount = 0;
    pLightingSubpass->subpassModels = NULL;
    pLightingSubpass->pRemovedIndexLinkedList = NULL;

    uint32_t index;
    AddModelToSubpass(pGraphicEngine->vkDevice, pLightingSubpass, &index);
    CreateLightingSubpassModel(pGraphicEngine, index);
}

void DestroyLightingSubpass(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t lightingSubpassIndex = 1;
    Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];

    DestroyLightingSubpassModel(pGraphicEngine, 0);
    RemoveModelFromSubpass(0, pLightingSubpass);

    for (uint32_t i = 0; i < pLightingSubpass->modelCountPerDescriptorPool; i++)
    {
        if (pLightingSubpass->subpassModels[i].isValid)
        {
            DestroyLightingSubpassModel(pGraphicEngine, i);
        }
        else
        {
            // Skip deleted
        }
    }
    TickernelFree(pLightingSubpass->subpassModels);

    for (uint32_t i = 0; i < pLightingSubpass->vkDescriptorPoolCount; i++)
    {
        vkDestroyDescriptorPool(pGraphicEngine->vkDevice, pLightingSubpass->vkDescriptorPools[i], NULL);
    }
    TickernelFree(pLightingSubpass->vkDescriptorPools);

    TickernelFree(pLightingSubpass->vkDescriptorPoolSizes);
    while (NULL != pLightingSubpass->pRemovedIndexLinkedList)
    {
        Uint32Node *pNode = pLightingSubpass->pRemovedIndexLinkedList;
        pLightingSubpass->pRemovedIndexLinkedList = pLightingSubpass->pRemovedIndexLinkedList->pNext;
        TickernelFree(pNode);
    }
    DestroyVkPipeline(pGraphicEngine);
}
void RecreateLightingSubpassModel(GraphicEngine *pGraphicEngine)
{
    DestroyLightingSubpassModel(pGraphicEngine, 0);
    CreateLightingSubpassModel(pGraphicEngine, 0);
}