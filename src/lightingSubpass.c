#include <lightingSubpass.h>

static void CreateVkPipeline(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t lightingSubpassIndex = 1;
    Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];

    VkShaderModule lightingVertShaderModule;
    CreateVkShaderModule(pGraphicEngine, "../assets/shaders/lighting.vert.spv", &lightingVertShaderModule);
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
    CreateVkShaderModule(pGraphicEngine, "../assets/shaders/lighting.frag.spv", &lightingFragShaderModule);
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

    uint32_t vertexBindingDescriptionCount = 1;
    VkVertexInputBindingDescription *vertexBindingDescriptions = (VkVertexInputBindingDescription[]){
        {
            .binding = 0,
            .stride = sizeof(LightingSubpassVertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        },
    };
    uint32_t vertexAttributeDescriptionCount = 1;
    VkVertexInputAttributeDescription vertexAttributeDescriptions[] = {
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(LightingSubpassVertex, position),
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
    VkDescriptorSetLayoutBinding *bindings = (VkDescriptorSetLayoutBinding[]){globalUniformLayoutBinding, depthAttachmentLayoutBinding, albedoAttachmentLayoutBinding};
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = 3,
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
        .pDynamicState = NULL,
        .layout = pLightingSubpass->vkPipelineLayout,
        .renderPass = pDeferredRenderPass->vkRenderPass,
        .subpass = lightingSubpassIndex,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    VkPipelineCache pipelineCache = NULL;
    result = vkCreateGraphicsPipelines(pGraphicEngine->vkDevice, pipelineCache, 1, &lightingPipelineCreateInfo, NULL, &pLightingSubpass->vkPipeline);
    TryThrowVulkanError(result);
    DestroyVkShaderModule(pGraphicEngine, lightingVertShaderModule);
    DestroyVkShaderModule(pGraphicEngine, lightingFragShaderModule);
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

static void CreateLightingSubpassModel(GraphicEngine *pGraphicEngine, uint32_t vertexCount, LightingSubpassVertex *lightingSubpassVertices, uint32_t groupIndex, uint32_t modelIndex)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t lightingSubpassIndex = 1;
    Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];

    SubpassModel *pSubpassModel = &pLightingSubpass->modelGroups[groupIndex].subpassModels[modelIndex];
    pSubpassModel->vertexCount = vertexCount;
    VkDeviceSize vertexBufferSize = sizeof(LightingSubpassVertex) * vertexCount;
    CreateVertexBuffer(pGraphicEngine, vertexBufferSize, lightingSubpassVertices, &pSubpassModel->vertexBuffer, &pSubpassModel->vertexBufferMemory);

    // Create vkDescriptorSet
    ModelGroup *pModelGroup = &pLightingSubpass->modelGroups[groupIndex];
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = pModelGroup->vkDescriptorPool,
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
    };
    vkUpdateDescriptorSets(pGraphicEngine->vkDevice, 3, descriptorWrites, 0, NULL);
    pSubpassModel->isValid = true;
}
static void DestroyLightingSubpassModel(GraphicEngine *pGraphicEngine, uint32_t groupIndex, uint32_t modelIndex)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t lightingSubpassIndex = 1;
    Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];
    ModelGroup *pModelGroup = &pLightingSubpass->modelGroups[groupIndex];
    SubpassModel *pSubpassModel = &pModelGroup->subpassModels[modelIndex];
    pSubpassModel->isValid = false;
    VkResult result = vkFreeDescriptorSets(pGraphicEngine->vkDevice, pModelGroup->vkDescriptorPool, 1, &pSubpassModel->vkDescriptorSet);
    TryThrowVulkanError(result);
    DestroyBuffer(pGraphicEngine->vkDevice, pSubpassModel->modelUniformBuffer, pSubpassModel->modelUniformBufferMemory);
    DestroyVertexBuffer(pGraphicEngine, pSubpassModel->vertexBuffer, pSubpassModel->vertexBufferMemory);
}

static void CreateLightingModelGroups(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t lightingSubpassIndex = 1;
    Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];

    pLightingSubpass->maxModelGroupCount = 1;
    pLightingSubpass->modelCountPerGroup = 1;
    pLightingSubpass->modelGroupCount = 0;
    pLightingSubpass->modelGroups = TickernelMalloc(sizeof(ModelGroup) * pLightingSubpass->maxModelGroupCount);
}
static void DestroyLightingModelGroups(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t lightingSubpassIndex = 1;
    Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];
    for (uint32_t groupIndex = 0; groupIndex < pLightingSubpass->modelGroupCount; groupIndex++)
    {
        ModelGroup *pModelGroup = &pLightingSubpass->modelGroups[groupIndex];
        ModelGroup modelGroup = *pModelGroup;
        bool removedIndices[modelGroup.modelCount];
        for (uint32_t i = 0; i < modelGroup.modelCount; i++)
        {
            removedIndices[i] = false;
        }
        Uint32Node *node = modelGroup.pRemovedIndexLinkedList;
        while (NULL != node)
        {
            removedIndices[node->data] = true;
            node = node->pNext;
        }
        for (uint32_t modelIndex = 0; modelIndex < modelGroup.modelCount; modelIndex++)
        {
            SubpassModel *pSubpassModel = &pModelGroup->subpassModels[modelIndex];
            if (pSubpassModel->isValid)
            {
                DestroyLightingSubpassModel(pGraphicEngine, groupIndex, modelIndex);
            }
            else
            {
                // Has removed.
            }
        }
        DestroyModelGroup(pGraphicEngine, *pModelGroup);
    }
    TickernelFree(pLightingSubpass->modelGroups);
}

void CreateLightingSubpass(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t lightingSubpassIndex = 1;
    Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];
    CreateVkPipeline(pGraphicEngine);
    CreateLightingModelGroups(pGraphicEngine);
    pLightingSubpass->vkDescriptorTypeToCount = TickernelMalloc(sizeof(uint32_t) * MAX_VK_DESCRIPTOR_TPYE);
    pLightingSubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_SAMPLER] = 0;                // 0,
    pLightingSubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER] = 0; // 1,
    pLightingSubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE] = 0;          // 2,
    pLightingSubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_STORAGE_IMAGE] = 0;          // 3,
    pLightingSubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER] = 0;   // 4,
    pLightingSubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER] = 0;   // 5,
    pLightingSubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER] = 1;         // 6,
    pLightingSubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_STORAGE_BUFFER] = 0;         // 7,
    pLightingSubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC] = 0; // 8,
    pLightingSubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC] = 0; // 9,
    pLightingSubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT] = 2;       // 10,
}
void DestroyLightingSubpass(GraphicEngine *pGraphicEngine)
{
    DestroyLightingModelGroups(pGraphicEngine);
    DestroyVkPipeline(pGraphicEngine);
}

void AddModelToLightingSubpass(GraphicEngine *pGraphicEngine, uint32_t vertexCount, LightingSubpassVertex *lightingSubpassVertices, uint32_t *pGroupIndex, uint32_t *pModelIndex)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t lightingSubpassIndex = 1;
    Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];

    AddModelToSubpass(pGraphicEngine, pLightingSubpass, pGroupIndex, pModelIndex);
    CreateLightingSubpassModel(pGraphicEngine, vertexCount, lightingSubpassVertices, *pGroupIndex, *pModelIndex);
}
void RemoveModelFromLightingSubpass(GraphicEngine *pGraphicEngine, uint32_t groupIndex, uint32_t modelIndex)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t lightingSubpassIndex = 1;
    Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];
    DestroyLightingSubpassModel(pGraphicEngine, groupIndex, modelIndex);
    RemoveModelFromSubpass(pGraphicEngine, groupIndex, modelIndex, pLightingSubpass);
}
