#include "lightingSubpass.h"

static void createVkPipelines(Subpass *pLightingSubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t lightingSubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor)
{
    pLightingSubpass->pipelineCount = 1;
    pLightingSubpass->pipelines = tickernelMalloc(sizeof(Pipeline) * pLightingSubpass->pipelineCount);

    VkShaderModule lightingVertShaderModule;
    char lightingVertShaderPath[FILENAME_MAX];
    sprintf(lightingVertShaderPath, "%s/%s", shadersPath, "lighting.vert.spv");
    createVkShaderModule(vkDevice, lightingVertShaderPath, &lightingVertShaderModule);
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
    sprintf(lightingFragShaderPath, "%s/%s", shadersPath, "lighting.frag.spv");
    createVkShaderModule(vkDevice, lightingFragShaderPath, &lightingFragShaderModule);
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

    VkDescriptorSetLayoutBinding depthAttachmentLayoutBinding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = NULL,
    };
    VkDescriptorSetLayoutBinding albedoAttachmentLayoutBinding = {
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = NULL,
    };
    VkDescriptorSetLayoutBinding normalAttachmentLayoutBinding = {
        .binding = 2,
        .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = NULL,
    };
    VkDescriptorSetLayoutBinding globalUniformLayoutBinding = {
        .binding = 3,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = NULL,
    };
    VkDescriptorSetLayoutBinding lightsUniformLayoutBinding = {
        .binding = 4,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = NULL,
    };
    VkDescriptorSetLayoutBinding *bindings = (VkDescriptorSetLayoutBinding[]){depthAttachmentLayoutBinding, albedoAttachmentLayoutBinding, normalAttachmentLayoutBinding, globalUniformLayoutBinding, lightsUniformLayoutBinding};
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = 5,
        .pBindings = bindings,
    };

    Pipeline *pPipeline = &pLightingSubpass->pipelines[0];
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
        .layout = pPipeline->vkPipelineLayout,
        .renderPass = vkRenderPass,
        .subpass = lightingSubpassIndex,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    VkPipelineCache pipelineCache = NULL;
    result = vkCreateGraphicsPipelines(vkDevice, pipelineCache, 1, &lightingPipelineCreateInfo, NULL, &pPipeline->vkPipeline);
    tryThrowVulkanError(result);
    destroyVkShaderModule(vkDevice, lightingVertShaderModule);
    destroyVkShaderModule(vkDevice, lightingFragShaderModule);
}
static void destroyVkPipeline(Subpass *pLightingSubpass, VkDevice vkDevice)
{
    Pipeline *pPipeline = &pLightingSubpass->pipelines[0];
    vkDestroyDescriptorSetLayout(vkDevice, pPipeline->descriptorSetLayout, NULL);
    vkDestroyPipelineLayout(vkDevice, pPipeline->vkPipelineLayout, NULL);
    vkDestroyPipeline(vkDevice, pPipeline->vkPipeline, NULL);

    tickernelFree(pLightingSubpass->pipelines);
}

static void createPostProcessSubpassModel(Subpass *pLightingSubpass, VkDevice vkDevice, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView)
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

    Pipeline* pPipeline = &pLightingSubpass->pipelines[0];
    // Create vkDescriptorSet
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = 1,
        .poolSizeCount = pPipeline->vkDescriptorPoolSizeCount,
        .pPoolSizes = pPipeline->vkDescriptorPoolSizes,
    };
    VkResult result = vkCreateDescriptorPool(vkDevice, &descriptorPoolCreateInfo, NULL, &subpassModel.vkDescriptorPool);
    tryThrowVulkanError(result);
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = subpassModel.vkDescriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &pPipeline->descriptorSetLayout,
    };

    result = vkAllocateDescriptorSets(vkDevice, &descriptorSetAllocateInfo, &subpassModel.vkDescriptorSet);
    tryThrowVulkanError(result);
    VkDescriptorBufferInfo globalDescriptorBufferInfo = {
        .buffer = globalUniformBuffer,
        .offset = 0,
        .range = sizeof(GlobalUniformBuffer),
    };
    VkDescriptorBufferInfo lightsDescriptorBufferInfo = {
        .buffer = lightsUniformBuffer,
        .offset = 0,
        .range = sizeof(LightsUniformBuffer),
    };
    VkDescriptorImageInfo depthVkDescriptorImageInfo = {
        .sampler = NULL,
        .imageView = depthVkImageView,
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
    };
    VkDescriptorImageInfo albedoVkDescriptorImageInfo = {
        .sampler = NULL,
        .imageView = albedoVkImageView,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    VkDescriptorImageInfo normalVkDescriptorImageInfo = {
        .sampler = NULL,
        .imageView = normalVkImageView,
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
            .pImageInfo = &depthVkDescriptorImageInfo,
            .pBufferInfo = NULL,
            .pTexelBufferView = NULL,
        },
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = NULL,
            .dstSet = subpassModel.vkDescriptorSet,
            .dstBinding = 1,
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
            .dstSet = subpassModel.vkDescriptorSet,
            .dstBinding = 2,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
            .pImageInfo = &normalVkDescriptorImageInfo,
            .pBufferInfo = NULL,
            .pTexelBufferView = NULL,
        },
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = NULL,
            .dstSet = subpassModel.vkDescriptorSet,
            .dstBinding = 3,
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
            .dstSet = subpassModel.vkDescriptorSet,
            .dstBinding = 4,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo = NULL,
            .pBufferInfo = &lightsDescriptorBufferInfo,
            .pTexelBufferView = NULL,
        },
    };
    vkUpdateDescriptorSets(vkDevice, 5, descriptorWrites, 0, NULL);
    tickernelAddToDynamicArray(&pPipeline->modelDynamicArray, &subpassModel, pPipeline->modelDynamicArray.length);
}
static void destroyLightingSubpassModel(Subpass *pLightingSubpass, VkDevice vkDevice, uint32_t index)
{
    Pipeline *pPipeline = &pLightingSubpass->pipelines[0];
    SubpassModel *pSubpassModel = pPipeline->modelDynamicArray.array[index];
    VkResult result = vkFreeDescriptorSets(vkDevice, pSubpassModel->vkDescriptorPool, 1, &pSubpassModel->vkDescriptorSet);
    tryThrowVulkanError(result);
    vkDestroyDescriptorPool(vkDevice, pSubpassModel->vkDescriptorPool, NULL);
    tickernelRemoveAtIndexFromDynamicArray(&pPipeline->modelDynamicArray, index);
}

void createLightingSubpass(Subpass *pLightingSubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t lightingSubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView)
{
    createVkPipelines(pLightingSubpass, shadersPath, vkRenderPass, lightingSubpassIndex, vkDevice, viewport, scissor);
    Pipeline *pPipeline = &pLightingSubpass->pipelines[0];
    pPipeline->vkDescriptorPoolSizeCount = 2;
    pPipeline->vkDescriptorPoolSizes = tickernelMalloc(sizeof(VkDescriptorPoolSize) * pPipeline->vkDescriptorPoolSizeCount);
    pPipeline->vkDescriptorPoolSizes[0] = (VkDescriptorPoolSize){
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 2,
    };
    pPipeline->vkDescriptorPoolSizes[1] = (VkDescriptorPoolSize){
        .type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
        .descriptorCount = 3,
    };

    tickernelCreateDynamicArray(&pPipeline->modelDynamicArray, 1, sizeof(SubpassModel));
    createPostProcessSubpassModel(pLightingSubpass, vkDevice, globalUniformBuffer, lightsUniformBuffer, depthVkImageView, albedoVkImageView, normalVkImageView);
}

void destroyLightingSubpass(Subpass *pLightingSubpass, VkDevice vkDevice)
{
    Pipeline *pPipeline = &pLightingSubpass->pipelines[0];
    destroyLightingSubpassModel(pLightingSubpass, vkDevice, 0);
    tickernelDestroyDynamicArray(&pPipeline->modelDynamicArray);
    tickernelFree(pPipeline->vkDescriptorPoolSizes);

    destroyVkPipeline(pLightingSubpass, vkDevice);
}
void recreateLightingSubpassModel(Subpass *pLightingSubpass, VkDevice vkDevice, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView)
{
    destroyLightingSubpassModel(pLightingSubpass, vkDevice, 0);
    createPostProcessSubpassModel(pLightingSubpass, vkDevice, globalUniformBuffer, lightsUniformBuffer, depthVkImageView, albedoVkImageView, normalVkImageView);
}
