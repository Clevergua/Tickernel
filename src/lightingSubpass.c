#include <lightingSubpass.h>

// static void CreateVkPipeline(GraphicEngine *pGraphicEngine)
// {
//     RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
//     uint32_t lightingSubpassIndex = 0;
//     Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];

//     VkShaderModule lightingVertShaderModule;
//     CreateVkShaderModule(pGraphicEngine, "../assets/shaders/lighting.vert.spv", &lightingVertShaderModule);
//     VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
//         .pNext = NULL,
//         .flags = 0,
//         .stage = VK_SHADER_STAGE_VERTEX_BIT,
//         .module = lightingVertShaderModule,
//         .pName = "main",
//         .pSpecializationInfo = NULL,
//     };

//     VkShaderModule lightingFragShaderModule;
//     CreateVkShaderModule(pGraphicEngine, "../assets/shaders/lighting.frag.spv", &lightingFragShaderModule);
//     VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
//         .pNext = NULL,
//         .flags = 0,
//         .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
//         .module = lightingFragShaderModule,
//         .pName = "main",
//         .pSpecializationInfo = NULL,
//     };
//     uint32_t stageCount = 2;
//     VkPipelineShaderStageCreateInfo *pipelineShaderStageCreateInfos = (VkPipelineShaderStageCreateInfo[]){vertShaderStageCreateInfo, fragShaderStageCreateInfo};

//     uint32_t vertexBindingDescriptionCount = 1;
//     VkVertexInputBindingDescription *vertexBindingDescriptions = (VkVertexInputBindingDescription[]){
//         {
//             .binding = 0,
//             .stride = sizeof(LightingSubpassVertex),
//             .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
//         },
//     };
//     uint32_t vertexAttributeDescriptionCount = 1;
//     VkVertexInputAttributeDescription vertexAttributeDescriptions[] = {
//         {
//             .location = 0,
//             .binding = 0,
//             .format = VK_FORMAT_R32G32B32_SFLOAT,
//             .offset = offsetof(LightingSubpassVertex, position),
//         }};
//     VkPipelineVertexInputStateCreateInfo vkPipelineVertexInputStateCreateInfo = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
//         .pNext = NULL,
//         .flags = 0,
//         .vertexBindingDescriptionCount = vertexBindingDescriptionCount,
//         .pVertexBindingDescriptions = vertexBindingDescriptions,
//         .vertexAttributeDescriptionCount = vertexAttributeDescriptionCount,
//         .pVertexAttributeDescriptions = vertexAttributeDescriptions,
//     };
//     VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
//         .pNext = NULL,
//         .flags = 0,
//         .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
//         .primitiveRestartEnable = VK_FALSE,
//     };
//     VkViewport viewport = {
//         .x = 0.0f,
//         .y = 0.0f,
//         .width = pGraphicEngine->swapchainExtent.width,
//         .height = pGraphicEngine->swapchainExtent.height,
//         .minDepth = 0.0f,
//         .maxDepth = 1.0f,
//     };
//     VkOffset2D offset = {
//         .x = 0,
//         .y = 0,
//     };
//     VkRect2D scissor = {
//         .offset = offset,
//         .extent = pGraphicEngine->swapchainExtent,
//     };
//     VkPipelineViewportStateCreateInfo pipelineViewportStateInfo = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
//         .pNext = NULL,
//         .flags = 0,
//         .viewportCount = 1,
//         .pViewports = &viewport,
//         .scissorCount = 1,
//         .pScissors = &scissor,
//     };
//     VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
//         .pNext = NULL,
//         .flags = 0,
//         .rasterizerDiscardEnable = VK_FALSE,
//         .polygonMode = VK_POLYGON_MODE_POINT,
//         .cullMode = VK_CULL_MODE_NONE,
//         .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
//         .depthBiasEnable = VK_FALSE,
//         .depthBiasConstantFactor = 0,
//         .depthBiasClamp = VK_FALSE,
//         .depthBiasSlopeFactor = 0,
//         .lineWidth = 0,
//     };
//     VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
//         .pNext = NULL,
//         .flags = 0,
//         .depthTestEnable = VK_FALSE,
//         .depthWriteEnable = VK_FALSE,
//         .depthCompareOp = VK_COMPARE_OP_ALWAYS,
//         .depthBoundsTestEnable = VK_FALSE,
//         .stencilTestEnable = VK_FALSE,
//         .front = {},
//         .back = {},
//         .minDepthBounds = 0,
//         .maxDepthBounds = 1,
//     };

//     VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
//         .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
//         .pNext = NULL,
//         .flags = 0,
//         .bindingCount = 0,
//         .pBindings = NULL,
//     };
//     VkResult result = vkCreateDescriptorSetLayout(pGraphicEngine->vkDevice, &descriptorSetLayoutCreateInfo, NULL, &pLightingSubpass->descriptorSetLayout);
//     VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
//         .pNext = NULL,
//         .flags = 0,
//         .setLayoutCount = 1,
//         .pSetLayouts = &pLightingSubpass->descriptorSetLayout,
//         .pushConstantRangeCount = 0,
//         .pPushConstantRanges = NULL,
//     };

//     result = vkCreatePipelineLayout(pGraphicEngine->vkDevice, &pipelineLayoutCreateInfo, NULL, &pLightingSubpass->vkPipelineLayout);
//     TryThrowVulkanError(result);
//     VkGraphicsPipelineCreateInfo lightingPipelineCreateInfo = {
//         .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
//         .pNext = NULL,
//         .flags = 0,
//         .stageCount = stageCount,
//         .pStages = pipelineShaderStageCreateInfos,
//         .pVertexInputState = &vkPipelineVertexInputStateCreateInfo,
//         .pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo,
//         .pTessellationState = NULL,
//         .pViewportState = &pipelineViewportStateInfo,
//         .pRasterizationState = &pipelineRasterizationStateCreateInfo,
//         .pMultisampleState = NULL,
//         .pDepthStencilState = &pipelineDepthStencilStateCreateInfo,
//         .pColorBlendState = NULL,
//         .pDynamicState = NULL,
//         .layout = pLightingSubpass->vkPipelineLayout,
//         .renderPass = pDeferredRenderPass->vkRenderPass,
//         .subpass = lightingSubpassIndex,
//         .basePipelineHandle = VK_NULL_HANDLE,
//         .basePipelineIndex = 0,
//     };

//     VkPipelineCache pipelineCache = NULL;
//     result = vkCreateGraphicsPipelines(pGraphicEngine->vkDevice, pipelineCache, 1, &lightingPipelineCreateInfo, NULL, &pLightingSubpass->vkPipeline);
//     TryThrowVulkanError(result);
//     DestroyVkShaderModule(pGraphicEngine, lightingVertShaderModule);
//     DestroyVkShaderModule(pGraphicEngine, lightingFragShaderModule);
// }
// static void DestroyVkPipeline(GraphicEngine *pGraphicEngine)
// {
//     RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
//     uint32_t lightingSubpassIndex = 0;
//     Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];
//     vkDestroyDescriptorSetLayout(pGraphicEngine->vkDevice, pLightingSubpass->descriptorSetLayout, NULL);
//     vkDestroyPipelineLayout(pGraphicEngine->vkDevice, pLightingSubpass->vkPipelineLayout, NULL);
//     vkDestroyPipeline(pGraphicEngine->vkDevice, pLightingSubpass->vkPipeline, NULL);
// }

// static void CreateSubpassModel(GraphicEngine *pGraphicEngine, uint32_t vertexCount, LightingSubpassVertex *lightingSubpassVertices, ModelGroup *pModelGroup, uint32_t modelIndex)
// {
//     RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
//     uint32_t lightingSubpassIndex = 0;
//     Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];

//     SubpassModel *pSubpassModel = &pModelGroup->subpassModels[modelIndex];
//     pSubpassModel->vertexCount = vertexCount;
//     VkDeviceSize vertexBufferSize = sizeof(LightingSubpassVertex) * vertexCount;
//     CreateVertexBuffer(pGraphicEngine, vertexBufferSize, lightingSubpassVertices, &pSubpassModel->vertexBuffer, &pSubpassModel->vertexBufferMemory);

//     VkDeviceSize uniformBufferSize = sizeof(GlobalUniformBuffer);
//     CreateUniformBuffer(pGraphicEngine, uniformBufferSize, &pSubpassModel->modelUniformBuffer, &pSubpassModel->modelUniformBufferMemory, pSubpassModel->modelUniformBufferMapped);

//     // Create vkDescriptorSet
//     VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
//         .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
//         .pNext = NULL,
//         .descriptorPool = pModelGroup->vkDescriptorPool,
//         .descriptorSetCount = 1,
//         .pSetLayouts = &pLightingSubpass->descriptorSetLayout,
//     };
//     VkResult result = vkAllocateDescriptorSets(pGraphicEngine->vkDevice, &descriptorSetAllocateInfo, &pSubpassModel->vkDescriptorSet);

//     VkDescriptorBufferInfo globalDescriptorBufferInfo = {
//         .buffer = pGraphicEngine->globalUniformBuffer,
//         .offset = 0,
//         .range = sizeof(GlobalUniformBuffer),
//     };

//     VkWriteDescriptorSet descriptorWrites[1] = {
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
//         }};
//     vkUpdateDescriptorSets(pGraphicEngine->vkDevice, 2, descriptorWrites, 0, NULL);
// }
// static void DestroySubpassModel(GraphicEngine *pGraphicEngine, ModelGroup *pModelGroup, uint32_t modelIndex)
// {
//     SubpassModel *pSubpassModel = &pModelGroup->subpassModels[modelIndex];
//     VkResult result = vkFreeDescriptorSets(pGraphicEngine->vkDevice, pModelGroup->vkDescriptorPool, 1, &pSubpassModel->vkDescriptorSet);
//     TryThrowVulkanError(result);
//     DestroyBuffer(pGraphicEngine->vkDevice, pSubpassModel->modelUniformBuffer, pSubpassModel->modelUniformBufferMemory);
//     DestroyVertexBuffer(pGraphicEngine, pSubpassModel->vertexBuffer, pSubpassModel->vertexBufferMemory);
// }

// static void CreateLightingModelGroup(GraphicEngine *pGraphicEngine, ModelGroup *pModelGroup)
// {
//     RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
//     uint32_t lightingSubpassIndex = 0;
//     Subpass *pLightingSubpass = &pDeferredRenderPass->subpasses[lightingSubpassIndex];
//     pModelGroup->modelCount = 0;
//     pModelGroup->subpassModels = TickernelMalloc(sizeof(SubpassModel) * pLightingSubpass->modelCountPerGroup);
//     pModelGroup->pRemovedIndexLinkedList = NULL;
//     VkDescriptorPoolSize poolSize[] = {
//         {
//             .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
//             .descriptorCount = pLightingSubpass->modelCountPerGroup * 2,
//         }};
//     VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
//         .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
//         .pNext = NULL,
//         .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
//         .maxSets = pLightingSubpass->modelCountPerGroup,
//         .poolSizeCount = 1,
//         .pPoolSizes = poolSize,
//     };
//     VkResult result = vkCreateDescriptorPool(pGraphicEngine->vkDevice, &descriptorPoolCreateInfo, NULL, &pModelGroup->vkDescriptorPool);
//     TryThrowVulkanError(result);
// }
// static void DestroyGeometryModelGroup(GraphicEngine *pGraphicEngine, ModelGroup *pModelGroup)
// {
//     RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
//     uint32_t geometrySubpassIndex = 0;
//     Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];

//     for (uint32_t modelIndex = 0; modelIndex < pModelGroup->modelCount; modelIndex++)
//     {
//         DestroySubpassModel(pGraphicEngine, pModelGroup, modelIndex);
//     }

//     vkDestroyDescriptorPool(pGraphicEngine->vkDevice, pModelGroup->vkDescriptorPool, NULL);

//     TickernelFree(pModelGroup->subpassModels);
// }

// static void CreateGeometryModelGroups(GraphicEngine *pGraphicEngine)
// {
//     RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
//     uint32_t geometrySubpassIndex = 0;
//     Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];

//     pGeometrySubpass->maxModelGroupCount = 1;
//     pGeometrySubpass->modelCountPerGroup = 256;
//     pGeometrySubpass->modelGroupCount = 0;
//     pGeometrySubpass->modelGroups = TickernelMalloc(sizeof(ModelGroup) * pGeometrySubpass->maxModelGroupCount);
// }
// static void DestroyGeometryModelGroups(GraphicEngine *pGraphicEngine)
// {
//     RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
//     uint32_t geometrySubpassIndex = 0;
//     Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];

//     for (uint32_t groupIndex = 0; groupIndex < pGeometrySubpass->modelGroupCount; groupIndex++)
//     {
//         ModelGroup *pModelGroup = &pGeometrySubpass->modelGroups[groupIndex];
//         DestroyGeometryModelGroup(pGraphicEngine, pModelGroup);
//     }
//     TickernelFree(pGeometrySubpass->modelGroups);
// }

// void CreateGeometrySubpass(GraphicEngine *pGraphicEngine)
// {
//     RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
//     CreateVkPipeline(pGraphicEngine);
//     CreateGeometryModelGroups(pGraphicEngine);
// }
// void DestroyGeometrySubpass(GraphicEngine *pGraphicEngine)
// {
//     DestroyGeometryModelGroups(pGraphicEngine);
//     DestroyVkPipeline(pGraphicEngine);
// }

// void AddModelToGeometrySubpass(GraphicEngine *pGraphicEngine, uint32_t vertexCount, GeometrySubpassVertex *geometrySubpassVertices, uint32_t *pGroupIndex, uint32_t *pModelIndex)
// {
//     RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
//     uint32_t geometrySubpassIndex = 0;
//     Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];

//     for (uint32_t groupIndex = 0; groupIndex < pGeometrySubpass->modelGroupCount; groupIndex++)
//     {
//         ModelGroup *pModelGroup = &pGeometrySubpass->modelGroups[groupIndex];
//         if (NULL != pModelGroup->pRemovedIndexLinkedList)
//         {
//             uint32_t modelIndex = pModelGroup->pRemovedIndexLinkedList->data;
//             Uint32Node *pNode = pModelGroup->pRemovedIndexLinkedList;
//             pModelGroup->pRemovedIndexLinkedList = pModelGroup->pRemovedIndexLinkedList->pNext;
//             TickernelFree(pNode);
//             CreateSubpassModel(pGraphicEngine, vertexCount, geometrySubpassVertices, pModelGroup, modelIndex);
//             *pGroupIndex = groupIndex;
//             *pModelIndex = modelIndex;
//             return;
//         }
//         else if (pModelGroup->modelCount < pGeometrySubpass->modelCountPerGroup)
//         {
//             uint32_t modelIndex = pModelGroup->modelCount;
//             CreateSubpassModel(pGraphicEngine, vertexCount, geometrySubpassVertices, pModelGroup, modelIndex);
//             pModelGroup->modelCount++;
//             *pGroupIndex = groupIndex;
//             *pModelIndex = modelIndex;
//             return;
//         }
//         else
//         {
//             // continue;
//         }
//     }

//     // Create group & create model
//     if (pGeometrySubpass->modelGroupCount < pGeometrySubpass->maxModelGroupCount)
//     {
//         uint32_t groupIndex = pGeometrySubpass->modelGroupCount;
//         // create model group
//         CreateLightingModelGroup(pGraphicEngine, &pGeometrySubpass->modelGroups[groupIndex]);
//         pGeometrySubpass->modelGroupCount++;
//         // create model
//         ModelGroup *pModelGroup = &pGeometrySubpass->modelGroups[groupIndex];
//         uint32_t modelIndex = pModelGroup->modelCount;
//         CreateSubpassModel(pGraphicEngine, vertexCount, geometrySubpassVertices, pModelGroup, modelIndex);
//         pModelGroup->modelCount++;

//         *pGroupIndex = groupIndex;
//         *pModelIndex = modelIndex;
//         return;
//     }
//     else
//     {
//         uint32_t oldCount = pGeometrySubpass->maxModelGroupCount;
//         ModelGroup *oldModelGroups = pGeometrySubpass->modelGroups;
//         pGeometrySubpass->maxModelGroupCount = pGeometrySubpass->maxModelGroupCount * 2;
//         pGeometrySubpass->modelGroups = TickernelMalloc(sizeof(ModelGroup) * pGeometrySubpass->maxModelGroupCount);
//         memcpy(pGeometrySubpass->modelGroups, oldModelGroups, oldCount * sizeof(ModelGroup));
//         TickernelFree(oldModelGroups);
//         // create model group
//         uint32_t groupIndex = pGeometrySubpass->modelGroupCount;
//         CreateLightingModelGroup(pGraphicEngine, &pGeometrySubpass->modelGroups[groupIndex]);
//         pGeometrySubpass->modelGroupCount++;
//         // create model
//         ModelGroup *pModelGroup = &pGeometrySubpass->modelGroups[groupIndex];
//         uint32_t modelIndex = pModelGroup->modelCount;
//         CreateSubpassModel(pGraphicEngine, vertexCount, geometrySubpassVertices, pModelGroup, modelIndex);
//         pModelGroup->modelCount++;

//         *pGroupIndex = groupIndex;
//         *pModelIndex = modelIndex;
//         return;
//     }
// }
// void RemoveModelFromGeometrySubpass(GraphicEngine *pGraphicEngine, uint32_t groupIndex, uint32_t modelIndex)
// {
//     RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
//     uint32_t geometrySubpassIndex = 0;
//     Subpass *pGeometrySubpass = &pDeferredRenderPass->subpasses[geometrySubpassIndex];

//     ModelGroup *pModelGroup = &pGeometrySubpass->modelGroups[groupIndex];

//     Uint32Node *newNode = TickernelMalloc(sizeof(Uint32Node));
//     newNode->data = modelIndex;
//     newNode->pNext = pModelGroup->pRemovedIndexLinkedList;
//     pModelGroup->pRemovedIndexLinkedList = newNode;
//     DestroySubpassModel(pGraphicEngine, pModelGroup, modelIndex);
// }

void CreateLightingSubpass(GraphicEngine *pGraphicEngine) {}
void DestroyLightingSubpass(GraphicEngine *pGraphicEngine) {}