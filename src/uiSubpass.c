#include "uiSubpass.h"
#include "graphicCore.h"

void createUISubpass(Subpass *pUISubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t uiSubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor)
{
    // Create shader modules
    char vertexShaderPath[FILENAME_MAX];
    char fragmentShaderPath[FILENAME_MAX];
    sprintf(vertexShaderPath, "%s/ui.vert.spv", shadersPath);
    sprintf(fragmentShaderPath, "%s/ui.frag.spv", shadersPath);

    VkShaderModule vertexShaderModule = createShaderModule(vkDevice, vertexShaderPath);
    VkShaderModule fragmentShaderModule = createShaderModule(vkDevice, fragmentShaderPath);

    // Create pipeline
    VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertexShaderModule,
        .pName = "main"
    };

    VkPipelineShaderStageCreateInfo fragmentShaderStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragmentShaderModule,
        .pName = "main"
    };

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertexShaderStageInfo, fragmentShaderStageInfo};

    VkVertexInputBindingDescription bindingDescription = {
        .binding = 0,
        .stride = sizeof(UISubpassVertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    VkVertexInputAttributeDescription attributeDescriptions[] = {
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(UISubpassVertex, position)
        },
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(UISubpassVertex, texCoord)
        },
        {
            .location = 2,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(UISubpassVertex, color)
        }
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &bindingDescription,
        .vertexAttributeDescriptionCount = 3,
        .pVertexAttributeDescriptions = attributeDescriptions
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    VkViewport viewportState = {
        .x = viewport.x,
        .y = viewport.y,
        .width = viewport.width,
        .height = viewport.height,
        .minDepth = viewport.minDepth,
        .maxDepth = viewport.maxDepth
    };

    VkRect2D scissorState = scissor;

    VkPipelineViewportStateCreateInfo viewportStateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewportState,
        .scissorCount = 1,
        .pScissors = &scissorState
    };

    VkPipelineRasterizationStateCreateInfo rasterizer = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f
    };

    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = NULL,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };

    VkPipelineColorBlendStateCreateInfo colorBlending = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
        .blendConstants[0] = 0.0f,
        .blendConstants[1] = 0.0f,
        .blendConstants[2] = 0.0f,
        .blendConstants[3] = 0.0f
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &pUISubpass->descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL
    };

    if (vkCreatePipelineLayout(vkDevice, &pipelineLayoutInfo, NULL, &pUISubpass->vkPipelineLayout) != VK_SUCCESS)
    {
        printf("Failed to create UI pipeline layout!\n");
        exit(1);
    }

    VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportStateInfo,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = NULL,
        .pColorBlendState = &colorBlending,
        .pDynamicState = NULL,
        .layout = pUISubpass->vkPipelineLayout,
        .renderPass = vkRenderPass,
        .subpass = uiSubpassIndex,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };

    if (vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &pUISubpass->vkPipeline) != VK_SUCCESS)
    {
        printf("Failed to create UI graphics pipeline!\n");
        exit(1);
    }

    vkDestroyShaderModule(vkDevice, fragmentShaderModule, NULL);
    vkDestroyShaderModule(vkDevice, vertexShaderModule, NULL);
}

void destroyUISubpass(Subpass *pUISubpass, VkDevice vkDevice)
{
    vkDestroyPipeline(vkDevice, pUISubpass->vkPipeline, NULL);
    vkDestroyPipelineLayout(vkDevice, pUISubpass->vkPipelineLayout, NULL);
    vkDestroyDescriptorSetLayout(vkDevice, pUISubpass->descriptorSetLayout, NULL);
}

SubpassModel *addModelToUISubpass(Subpass *pUISubpass, VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, VkCommandPool graphicVkCommandPool, VkQueue vkGraphicQueue, VkBuffer globalUniformBuffer, uint32_t vertexCount, UISubpassVertex *uiSubpassVertices)
{
    SubpassModel *pSubpassModel = malloc(sizeof(SubpassModel));
    pSubpassModel->vertexCount = vertexCount;
    pSubpassModel->maxInstanceCount = 0;  // UI不需要实例化
    pSubpassModel->instanceCount = 0;     // UI不需要实例化
    pSubpassModel->instanceBuffer = VK_NULL_HANDLE;
    pSubpassModel->instanceBufferMemory = VK_NULL_HANDLE;
    pSubpassModel->modelUniformBuffer = VK_NULL_HANDLE;
    pSubpassModel->modelUniformBufferMemory = VK_NULL_HANDLE;
    pSubpassModel->modelUniformBufferMapped = NULL;

    // Create vertex buffer
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(UISubpassVertex) * vertexCount,
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    if (vkCreateBuffer(vkDevice, &bufferInfo, NULL, &pSubpassModel->vertexBuffer) != VK_SUCCESS)
    {
        printf("Failed to create UI vertex buffer!\n");
        exit(1);
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vkDevice, pSubpassModel->vertexBuffer, &memRequirements);

    uint32_t memoryTypeIndex;
    findMemoryType(vkPhysicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &memoryTypeIndex);

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = memoryTypeIndex
    };

    if (vkAllocateMemory(vkDevice, &allocInfo, NULL, &pSubpassModel->vertexBufferMemory) != VK_SUCCESS)
    {
        printf("Failed to allocate UI vertex buffer memory!\n");
        exit(1);
    }

    vkBindBufferMemory(vkDevice, pSubpassModel->vertexBuffer, pSubpassModel->vertexBufferMemory, 0);

    void *data;
    vkMapMemory(vkDevice, pSubpassModel->vertexBufferMemory, 0, bufferInfo.size, 0, &data);
    memcpy(data, uiSubpassVertices, bufferInfo.size);
    vkUnmapMemory(vkDevice, pSubpassModel->vertexBufferMemory);

    return pSubpassModel;
}

void removeModelFromUISubpass(Subpass *pUISubpass, VkDevice vkDevice, SubpassModel *pSubpassModel)
{
    vkDestroyBuffer(vkDevice, pSubpassModel->vertexBuffer, NULL);
    vkFreeMemory(vkDevice, pSubpassModel->vertexBufferMemory, NULL);
    free(pSubpassModel);
}

void updateUISubpass(Subpass *pUISubpass, VkDevice vkDevice, VkBuffer globalUniformBuffer) {
    // Update descriptor sets
    VkDescriptorBufferInfo bufferInfo = {
        .buffer = globalUniformBuffer,
        .offset = 0,
        .range = VK_WHOLE_SIZE,
    };

    VkWriteDescriptorSet descriptorWrite = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = pUISubpass->vkDescriptorSet,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = &bufferInfo,
    };

    vkUpdateDescriptorSets(vkDevice, 1, &descriptorWrite, 0, NULL);
}

void recordUISubpass(Subpass *pUISubpass, VkCommandBuffer vkCommandBuffer, VkDevice vkDevice) {
    vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pUISubpass->vkPipeline);
    vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pUISubpass->vkPipelineLayout, 0, 1, &pUISubpass->vkDescriptorSet, 0, NULL);
    vkCmdDraw(vkCommandBuffer, 6, 1, 0, 0); // Assuming a quad with 6 vertices
}