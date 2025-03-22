#include "postProcessSubpass.h"

static void createVkPipeline(Subpass *pPostProcessSubpass, const char *shadersPath, VkRenderPass vkRenderPass, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor)
{
    VkShaderModule postProcessVertShaderModule;
    char postProcessVertShaderPath[FILENAME_MAX];
    sprintf(postProcessVertShaderPath, "%s/%s", shadersPath, "postProcess.vert.spv");
    createVkShaderModule(vkDevice, postProcessVertShaderPath, &postProcessVertShaderModule);
    VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = postProcessVertShaderModule,
        .pName = "main",
        .pSpecializationInfo = NULL,
    };

    VkShaderModule postProcessFragShaderModule;
    char postProcessFragShaderPath[FILENAME_MAX];
    sprintf(postProcessFragShaderPath, "%s/%s", shadersPath, "postProcess.frag.spv");
    createVkShaderModule(vkDevice, postProcessFragShaderPath, &postProcessFragShaderModule);
    VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = postProcessFragShaderModule,
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
        .cullMode = VK_CULL_MODE_FRONT_BIT,
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

    VkDescriptorSetLayoutBinding colorAttachmentLayoutBinding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = NULL,
    };
    VkDescriptorSetLayoutBinding *bindings = (VkDescriptorSetLayoutBinding[]){colorAttachmentLayoutBinding};
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = 1,
        .pBindings = bindings,
    };

    VkResult result = vkCreateDescriptorSetLayout(vkDevice, &descriptorSetLayoutCreateInfo, NULL, &pPostProcessSubpass->descriptorSetLayout);
    tryThrowVulkanError(result);
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &pPostProcessSubpass->descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
    };

    result = vkCreatePipelineLayout(vkDevice, &pipelineLayoutCreateInfo, NULL, &pPostProcessSubpass->vkPipelineLayout);
    tryThrowVulkanError(result);
    VkGraphicsPipelineCreateInfo postProcessPipelineCreateInfo = {
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
        .layout = pPostProcessSubpass->vkPipelineLayout,
        .renderPass = vkRenderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    VkPipelineCache pipelineCache = NULL;
    result = vkCreateGraphicsPipelines(vkDevice, pipelineCache, 1, &postProcessPipelineCreateInfo, NULL, &pPostProcessSubpass->vkPipeline);
    tryThrowVulkanError(result);
    destroyVkShaderModule(vkDevice, postProcessVertShaderModule);
    destroyVkShaderModule(vkDevice, postProcessFragShaderModule);
}
static void destroyVkPipeline(Subpass *pPostProcessSubpass, VkDevice vkDevice)
{
    vkDestroyDescriptorSetLayout(vkDevice, pPostProcessSubpass->descriptorSetLayout, NULL);
    vkDestroyPipelineLayout(vkDevice, pPostProcessSubpass->vkPipelineLayout, NULL);
    vkDestroyPipeline(vkDevice, pPostProcessSubpass->vkPipeline, NULL);
}

static void createPostProcessSubpassModel(Subpass *pPostProcessSubpass, VkDevice vkDevice, VkImageView colorVkImageView)
{
    SubpassModel subpassModel = {
        .vertexCount = 0,
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
    // Create vkDescriptorSet
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = 1,
        .poolSizeCount = pPostProcessSubpass->vkDescriptorPoolSizeCount,
        .pPoolSizes = pPostProcessSubpass->vkDescriptorPoolSizes,
    };
    VkResult result = vkCreateDescriptorPool(vkDevice, &descriptorPoolCreateInfo, NULL, &subpassModel.vkDescriptorPool);
    tryThrowVulkanError(result);
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = subpassModel.vkDescriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &pPostProcessSubpass->descriptorSetLayout,
    };

    result = vkAllocateDescriptorSets(vkDevice, &descriptorSetAllocateInfo, &subpassModel.vkDescriptorSet);
    tryThrowVulkanError(result);
    VkDescriptorImageInfo colorVkDescriptorImageInfo = {
        .sampler = NULL,
        .imageView = colorVkImageView,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    VkWriteDescriptorSet descriptorWrites[] = {
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = NULL,
            .dstSet = subpassModel.vkDescriptorSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
            .pImageInfo = &colorVkDescriptorImageInfo,
            .pBufferInfo = NULL,
            .pTexelBufferView = NULL,
        },
    };
    vkUpdateDescriptorSets(vkDevice, 1, descriptorWrites, 0, NULL);
    tickernelAddToDynamicArray(&pPostProcessSubpass->modelDynamicArray, &subpassModel, pPostProcessSubpass->modelDynamicArray.length);
}
static void destroyPostProcessSubpassModel(Subpass *pPostProcessSubpass, VkDevice vkDevice, uint32_t index)
{
    SubpassModel *pSubpassModel = pPostProcessSubpass->modelDynamicArray.array[index];
    VkResult result = vkFreeDescriptorSets(vkDevice, pSubpassModel->vkDescriptorPool, 1, &pSubpassModel->vkDescriptorSet);
    tryThrowVulkanError(result);
    vkDestroyDescriptorPool(vkDevice, pSubpassModel->vkDescriptorPool, NULL);
    tickernelRemoveAtIndexFromDynamicArray(&pPostProcessSubpass->modelDynamicArray, index);
}

void createPostProcessSubpass(Subpass *pPostProcessSubpass, const char *shadersPath, VkRenderPass vkRenderPass, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor, VkImageView colorVkImageView)
{
    createVkPipeline(pPostProcessSubpass, shadersPath, vkRenderPass, vkDevice, viewport, scissor);
    pPostProcessSubpass->vkDescriptorPoolSizeCount = 1;
    pPostProcessSubpass->vkDescriptorPoolSizes = tickernelMalloc(sizeof(VkDescriptorPoolSize) * pPostProcessSubpass->vkDescriptorPoolSizeCount);
    pPostProcessSubpass->vkDescriptorPoolSizes[0] = (VkDescriptorPoolSize){
        .type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
        .descriptorCount = 1,
    };

    tickernelCreateDynamicArray(&pPostProcessSubpass->modelDynamicArray, 1, sizeof(SubpassModel));
    createPostProcessSubpassModel(pPostProcessSubpass, vkDevice, colorVkImageView);
}

void destroyPostProcessSubpass(Subpass *pPostProcessSubpass, VkDevice vkDevice)
{
    destroyPostProcessSubpassModel(pPostProcessSubpass, vkDevice, 0);
    tickernelDestroyDynamicArray(&pPostProcessSubpass->modelDynamicArray);
    tickernelFree(pPostProcessSubpass->vkDescriptorPoolSizes);

    destroyVkPipeline(pPostProcessSubpass, vkDevice);
}
void recreatePostProcessSubpassModel(Subpass *pPostProcessSubpass, VkDevice vkDevice, VkImageView colorVkImageView)
{
    destroyPostProcessSubpassModel(pPostProcessSubpass, vkDevice, 0);
    createPostProcessSubpassModel(pPostProcessSubpass, vkDevice, colorVkImageView);
}
