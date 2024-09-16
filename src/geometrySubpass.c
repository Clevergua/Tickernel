#include <geometrySubpass.h>

static void CreateVkPipeline(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t geometrySubpassIndex = 0;
    Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];

    VkShaderModule geometryVertShaderModule;
    CreateVkShaderModule(pGraphicEngine, "../assets/shaders/geometry.vert.spv", &geometryVertShaderModule);
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
    CreateVkShaderModule(pGraphicEngine, "../assets/shaders/geometry.frag.spv", &geometryFragShaderModule);
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
    uint32_t vertexAttributeDescriptionCount = 2;
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
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0,
        .depthBiasClamp = VK_FALSE,
        .depthBiasSlopeFactor = 0,
        .lineWidth = 0,
    };
    VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .depthTestEnable = VK_TRUE,
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

static void CreateGeometryModelGroups(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t geometrySubpassIndex = 0;
    Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];

    pGeometrySubpass->maxModelGroupCount = 1;
    pGeometrySubpass->modelCountPerGroup = 256;
    pGeometrySubpass->modelGroupCount = 0;
    pGeometrySubpass->modelGroups = TickernelMalloc(sizeof(ModelGroup) * pGeometrySubpass->maxModelGroupCount);
}
static void DestroyGeometryModelGroups(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t geometrySubpassIndex = 0;
    Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];

    for (uint32_t groupIndex = 0; groupIndex < pGeometrySubpass->modelGroupCount; groupIndex++)
    {
        ModelGroup modelGroup = pGeometrySubpass->modelGroups[groupIndex];
        DestroyModelGroup(pGraphicEngine, modelGroup);
    }
    TickernelFree(pGeometrySubpass->modelGroups);
}

void CreateGeometrySubpass(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t geometrySubpassIndex = 0;
    Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];
    CreateVkPipeline(pGraphicEngine);
    CreateGeometryModelGroups(pGraphicEngine);
    pGeometrySubpass->vkDescriptorTypeToCount = TickernelMalloc(sizeof(uint32_t) * MAX_VK_DESCRIPTOR_TPYE);
    pGeometrySubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_SAMPLER] = 0;                // 0,
    pGeometrySubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER] = 0; // 1,
    pGeometrySubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE] = 0;          // 2,
    pGeometrySubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_STORAGE_IMAGE] = 0;          // 3,
    pGeometrySubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER] = 0;   // 4,
    pGeometrySubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER] = 0;   // 5,
    pGeometrySubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER] = 2;         // 6,
    pGeometrySubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_STORAGE_BUFFER] = 0;         // 7,
    pGeometrySubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC] = 0; // 8,
    pGeometrySubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC] = 0; // 9,
    pGeometrySubpass->vkDescriptorTypeToCount[VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT] = 0;       // 10,
}
void DestroyGeometrySubpass(GraphicEngine *pGraphicEngine)
{
    DestroyGeometryModelGroups(pGraphicEngine);
    DestroyVkPipeline(pGraphicEngine);
}
void AddModelToGeometrySubpass(GraphicEngine *pGraphicEngine, uint32_t vertexCount, GeometrySubpassVertex *geometrySubpassVertices, uint32_t *pGroupIndex, uint32_t *pModelIndex)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t geometrySubpassIndex = 0;
    Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];

    VkDescriptorBufferInfo globalDescriptorBufferInfo = {
        .buffer = pGraphicEngine->globalUniformBuffer,
        .offset = 0,
        .range = sizeof(GlobalUniformBuffer),
    };
    VkDescriptorBufferInfo objectDescriptorBufferInfo = {
        .buffer = pSubpassModel->modelUniformBuffer,
        .offset = 0,
        .range = sizeof(GeometrySubpassModelUniformBuffer),
    };

    VkWriteDescriptorSet descriptorWrites[2] = {
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
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo = NULL,
            .pBufferInfo = &objectDescriptorBufferInfo,
            .pTexelBufferView = NULL,
        },
    };
    SubpassModelCreateInfo subpassModelCreateInfo = {
        .vertexCount = vertexCount,
        .vertexSize = sizeof(GeometrySubpassVertex),
        .vertices = geometrySubpassVertices,
        // .modelUniformBufferSize =,
        .vkDescriptorSetLayout = pGeometrySubpass->descriptorSetLayout,
    };

    AddModelToSubpass(pGraphicEngine->vkDevice, )
}
void RemoveModelFromGeometrySubpass(GraphicEngine *pGraphicEngine, uint32_t groupIndex, uint32_t modelIndex)
{
}
// void CreateSubpassModel(GraphicEngine *pGraphicEngine, uint32_t vertexCount, VkDeviceSize vertexSize, void *vertices, ModelGroup *pModelGroup, VkDescriptorSetLayout vkDescriptorSetLayout, uint32_t modelIndex, SubpassModel *pSubpassModel)
// {
//     pSubpassModel->vertexCount = vertexCount;
//     VkDeviceSize vertexBufferSize = vertexSize * vertexCount;
//     CreateVertexBuffer(pGraphicEngine, vertexBufferSize, vertices, &pSubpassModel->vertexBuffer, &pSubpassModel->vertexBufferMemory);

//     VkDeviceSize uniformBufferSize = sizeof(GeometrySubpassModelUniformBuffer);
//     CreateBuffer(pGraphicEngine, uniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pSubpassModel->modelUniformBuffer, &pSubpassModel->modelUniformBufferMemory);
//     vkMapMemory(pGraphicEngine->vkDevice, *&pSubpassModel->modelUniformBufferMemory, 0, uniformBufferSize, 0, pSubpassModel->modelUniformBufferMapped);
//     // Create vkDescriptorSet
//     VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
//         .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
//         .pNext = NULL,
//         .descriptorPool = pModelGroup->vkDescriptorPool,
//         .descriptorSetCount = 1,
//         .pSetLayouts = &vkDescriptorSetLayout,
//     };
//     VkResult result = vkAllocateDescriptorSets(pGraphicEngine->vkDevice, &descriptorSetAllocateInfo, &pSubpassModel->vkDescriptorSet);

//     VkDescriptorBufferInfo globalDescriptorBufferInfo = {
//         .buffer = pGraphicEngine->globalUniformBuffer,
//         .offset = 0,
//         .range = sizeof(GlobalUniformBuffer),
//     };
//     VkDescriptorBufferInfo objectDescriptorBufferInfo = {
//         .buffer = pSubpassModel->modelUniformBuffer,
//         .offset = 0,
//         .range = sizeof(GeometrySubpassModelUniformBuffer),
//     };

//     VkWriteDescriptorSet descriptorWrites[2] = {
//         {
//             .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
//             .pNext = NULL,
//             .dstSet = pSubpassModel->vkDescriptorSet,
//             .dstBinding = 0,
//             .dstArrayElement = 0,
//             .descriptorCount = 1,
//             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
//             .pImageInfo = NULL,
//             .pBufferInfo = &globalDescriptorBufferInfo,
//             .pTexelBufferView = NULL,
//         },
//         {
//             .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
//             .pNext = NULL,
//             .dstSet = pSubpassModel->vkDescriptorSet,
//             .dstBinding = 1,
//             .dstArrayElement = 0,
//             .descriptorCount = 1,
//             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
//             .pImageInfo = NULL,
//             .pBufferInfo = &objectDescriptorBufferInfo,
//             .pTexelBufferView = NULL,
//         },
//     };
//     vkUpdateDescriptorSets(pGraphicEngine->vkDevice, 2, descriptorWrites, 0, NULL);
// }

// void DestroySubpassModel(GraphicEngine *pGraphicEngine, ModelGroup *pModelGroup, uint32_t modelIndex)
// {
//     SubpassModel *pSubpassModel = &pModelGroup->subpassModels[modelIndex];
//     VkResult result = vkFreeDescriptorSets(pGraphicEngine->vkDevice, pModelGroup->vkDescriptorPool, 1, &pSubpassModel->vkDescriptorSet);
//     TryThrowVulkanError(result);
//     DestroyBuffer(pGraphicEngine->vkDevice, pSubpassModel->modelUniformBuffer, pSubpassModel->modelUniformBufferMemory);
//     DestroyVertexBuffer(pGraphicEngine, pSubpassModel->vertexBuffer, pSubpassModel->vertexBufferMemory);
// }
