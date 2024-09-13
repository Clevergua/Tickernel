#include <deferredRenderPipeline.h>

static void PrepareCurrentFrambuffer(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    if (pGraphicEngine->hasRecreatedSwapchain)
    {
        for (uint32_t i = 0; i < pDeferredRenderPass->vkFramebufferCount; i++)
        {
            if (pDeferredRenderPass->vkFramebuffers[i] == INVALID_VKFRAMEBUFFER)
            {
                // continue;
            }
            else
            {
                vkDestroyFramebuffer(pGraphicEngine->vkDevice, pDeferredRenderPass->vkFramebuffers[i], NULL);
                pDeferredRenderPass->vkFramebuffers[i] = INVALID_VKFRAMEBUFFER;
            }
        }
    }
    uint32_t attachmentCount = 3;
    VkImageView attachments[] = {pGraphicEngine->swapchainImageViews[pGraphicEngine->frameIndex], pGraphicEngine->depthGraphicImage.vkImageView, pGraphicEngine->albedoGraphicImage.vkImageView};
    if (INVALID_VKFRAMEBUFFER == pDeferredRenderPass->vkFramebuffers[pGraphicEngine->frameIndex])
    {
        VkFramebufferCreateInfo vkFramebufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .renderPass = pDeferredRenderPass->vkRenderPass,
            .attachmentCount = attachmentCount,
            .pAttachments = attachments,
            .width = pGraphicEngine->swapchainExtent.width,
            .height = pGraphicEngine->swapchainExtent.height,
            .layers = 1,
        };
        VkResult result = vkCreateFramebuffer(pGraphicEngine->vkDevice, &vkFramebufferCreateInfo, NULL, &pDeferredRenderPass->vkFramebuffers[pGraphicEngine->frameIndex]);
        TryThrowVulkanError(result);
    }
}

static void CreateVkRenderPass(GraphicEngine *pGraphicEngine)
{
    VkAttachmentDescription colorAttachmentDescription = {
        .flags = 0,
        .format = pGraphicEngine->surfaceFormat.format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };
    VkAttachmentDescription depthAttachmentDescription = {
        .flags = 0,
        .format = pGraphicEngine->depthGraphicImage.vkFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentDescription albedoAttachmentDescription = {
        .flags = 0,
        .format = pGraphicEngine->albedoGraphicImage.vkFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    uint32_t attachmentCount = 3;
    VkAttachmentDescription vkAttachmentDescriptions[] = {
        colorAttachmentDescription,
        depthAttachmentDescription,
        albedoAttachmentDescription,
    };

    VkAttachmentReference geometryDepthAttachmentReference = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentReference geometryAlbedoAttachmentReference = {
        .attachment = 2,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    VkSubpassDescription geometrySubpassDescription = {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = NULL,
        .colorAttachmentCount = 1,
        .pColorAttachments = &geometryAlbedoAttachmentReference,
        .pResolveAttachments = NULL,
        .pDepthStencilAttachment = &geometryDepthAttachmentReference,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = NULL,
    };

    VkAttachmentReference lightColorAttachmentReference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentReference lightDepthAttachmentReference = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
    };
    VkAttachmentReference lightAlbedoAttachmentReference = {
        .attachment = 2,
        .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    VkSubpassDescription ligthSubpassDescription = {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 2,
        .pInputAttachments = (const VkAttachmentReference[]){lightDepthAttachmentReference, lightAlbedoAttachmentReference},
        .colorAttachmentCount = 1,
        .pColorAttachments = &lightColorAttachmentReference,
        .pResolveAttachments = NULL,
        .pDepthStencilAttachment = &geometryDepthAttachmentReference,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = NULL,
    };
    uint32_t subpassCount = 2;
    VkSubpassDescription vkSubpassDescriptions[] = {
        geometrySubpassDescription,
        ligthSubpassDescription,
    };
    uint32_t dependencyCount = subpassCount - 1;
    VkSubpassDependency subpassDependencies[dependencyCount];
    for (uint32_t i = 0; i < dependencyCount; i++)
    {
        subpassDependencies[i].srcSubpass = i;
        subpassDependencies[i].dstSubpass = i + 1;
        subpassDependencies[i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependencies[i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        subpassDependencies[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        subpassDependencies[i].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
        subpassDependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    }

    VkRenderPassCreateInfo vkRenderPassCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .attachmentCount = attachmentCount,
        .pAttachments = vkAttachmentDescriptions,
        .subpassCount = subpassCount,
        .pSubpasses = vkSubpassDescriptions,
        .dependencyCount = dependencyCount,
        .pDependencies = subpassDependencies,
    };
    VkResult result = VK_SUCCESS;
    result = vkCreateRenderPass(pGraphicEngine->vkDevice, &vkRenderPassCreateInfo, NULL, &pGraphicEngine->deferredRenderPass.vkRenderPass);
    TryThrowVulkanError(result);
}

static void DestroyVkRenderPass(GraphicEngine *pGraphicEngine)
{
    vkDestroyRenderPass(pGraphicEngine->vkDevice, pGraphicEngine->deferredRenderPass.vkRenderPass, NULL);
}

static void CreateVkFramebuffers(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    pDeferredRenderPass->vkFramebuffers = TickernelMalloc(sizeof(VkFramebuffer) * pGraphicEngine->swapchainImageCount);
    pDeferredRenderPass->vkFramebufferCount = pGraphicEngine->swapchainImageCount;
    for (int32_t i = 0; i < pGraphicEngine->deferredRenderPass.vkFramebufferCount; i++)
    {
        pDeferredRenderPass->vkFramebuffers[i] = INVALID_VKFRAMEBUFFER;
    }
}

static void DestroyVkFramebuffers(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    for (int32_t i = 0; i < pDeferredRenderPass->vkFramebufferCount; i++)
    {
        if (INVALID_VKFRAMEBUFFER == pDeferredRenderPass->vkFramebuffers[i])
        {
            // continue;
        }
        else
        {
            vkDestroyFramebuffer(pGraphicEngine->vkDevice, pDeferredRenderPass->vkFramebuffers[i], NULL);
        }
    }
}

static void CreateDescriptorPool(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    uint32_t descriptorCount = 0;
    for (uint32_t i = 0; i < pDeferredRenderPass->subpassCount; i++)
    {
        Subpass subpass = pDeferredRenderPass->subpasses[i];
        descriptorCount += subpass.maxObjectCount;
    }

    VkDescriptorPoolSize poolSize[] = {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = descriptorCount,
        }};
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .maxSets = descriptorCount,
        .poolSizeCount = 1,
        .pPoolSizes = poolSize,
    };
    VkResult result = vkCreateDescriptorPool(pGraphicEngine->vkDevice, &descriptorPoolCreateInfo, NULL, &pGraphicEngine->deferredRenderPass.vkDescriptorPool);
    TryThrowVulkanError(result);
}

static void DestroyDescriptorPool(GraphicEngine *pGraphicEngine)
{
    vkDestroyDescriptorPool(pGraphicEngine->vkDevice, pGraphicEngine->deferredRenderPass.vkDescriptorPool, NULL);
}
static void CreateSubpasses(GraphicEngine *pGraphicEngine)
{
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    pDeferredRenderPass->subpassCount = 2;
    pDeferredRenderPass->subpasses = TickernelMalloc(sizeof(Subpass) * pDeferredRenderPass->subpassCount);

    // pGraphicEngine->deferredRenderPass.vkPipelineCount = 2;
    // pGraphicEngine->deferredRenderPass.vkPipelines = TickernelMalloc(sizeof(VkPipeline) * pGraphicEngine->deferredRenderPass.vkPipelineCount);
    // pGraphicEngine->deferredRenderPass.vkPipelineToLayout = TickernelMalloc(sizeof(VkPipelineLayout) * pGraphicEngine->deferredRenderPass.vkPipelineCount);
    // pGraphicEngine->deferredRenderPass.vkPipelineToDescriptorSetLayout = TickernelMalloc(sizeof(VkDescriptorSetLayout *) * pGraphicEngine->deferredRenderPass.vkPipelineCount);

    // pGraphicEngine->deferredRenderPass.maxObjectCount = 4096;
    // pGraphicEngine->deferredRenderPass.renderPipelineObjects = TickernelMalloc(sizeof(RenderPipelineObject) * pGraphicEngine->deferredRenderPass.maxObjectCount);
    CreateDescriptorPool(pGraphicEngine);
    CreateGeometryPipeline(pGraphicEngine);
    CreateLightingPipeline(pGraphicEngine);
}

static void DestroySubpasses(GraphicEngine *pGraphicEngine)
{
    DestroyGeometryPipeline(pGraphicEngine);
    DestroyLightingPipeline(pGraphicEngine);
    DestroyDescriptorPool(pGraphicEngine);
    // TickernelFree(pGraphicEngine->deferredRenderPass.renderPipelineObjects);
    // TickernelFree(pGraphicEngine->deferredRenderPass.vkPipelineToLayout);
    // TickernelFree(pGraphicEngine->deferredRenderPass.vkPipelineToDescriptorSetLayout);
    TickernelFree(pGraphicEngine->deferredRenderPass.subpasses);
}

void CreateDeferredRenderPipeline(GraphicEngine *pGraphicEngine)
{
    CreateVkRenderPass(pGraphicEngine);
    CreateVkFramebuffers(pGraphicEngine);
    CreateSubpasses(pGraphicEngine);
}

void DestroyDeferredRenderPipeline(GraphicEngine *pGraphicEngine)
{
    DestroySubpasses(pGraphicEngine);
    DestroyVkFramebuffers(pGraphicEngine);
    DestroyVkRenderPass(pGraphicEngine);
}

void RecordDeferredRenderPipeline(GraphicEngine *pGraphicEngine)
{
    PrepareCurrentFrambuffer(pGraphicEngine);
    VkCommandBuffer vkCommandBuffer = pGraphicEngine->graphicVkCommandBuffers[pGraphicEngine->frameIndex];
    VkCommandBufferBeginInfo vkCommandBufferBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = NULL,
            .flags = 0,
            .pInheritanceInfo = NULL,
        };
    VkResult result = vkBeginCommandBuffer(vkCommandBuffer, &vkCommandBufferBeginInfo);
    TryThrowVulkanError(result);
    VkOffset2D offset =
        {
            .x = 0,
            .y = 0,
        };
    VkRect2D renderArea =
        {
            .offset = offset,
            .extent = pGraphicEngine->swapchainExtent,
        };
    uint32_t clearValueCount = 2;
    VkClearValue *clearValues = (VkClearValue[]){
        {
            .color = {0.0f, 0.0f, 0.0f, 1.0f},
        },
        {
            .depthStencil = {1.0f, 0},
        },
    };
    RenderPass *pDeferredRenderPass = &pGraphicEngine->deferredRenderPass;
    VkRenderPassBeginInfo renderPassBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = NULL,
            .renderPass = pDeferredRenderPass->vkRenderPass,
            .framebuffer = pDeferredRenderPass->vkFramebuffers[pGraphicEngine->frameIndex],
            .renderArea = renderArea,
            .clearValueCount = clearValueCount,
            .pClearValues = clearValues,
        };
    vkCmdBeginRenderPass(vkCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    for (uint32_t i = 0; i < pDeferredRenderPass->subpassCount; i++)
    {
        Subpass *pSubpass = &pDeferredRenderPass->subpasses[i];
        vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pSubpass->vkPipeline);
        uint32_t geometryPipelineIndex = 0;
        for (uint32_t i = 0; i < pSubpass->objectCount; i++)
        {
            RenderPipelineObject *pRenderPipelineObject = &pSubpass->renderPipelineObjects[i];
            vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pSubpass->vkPipelineLayout, 0, 1, &pRenderPipelineObject->vkDescriptorSet, 0, NULL);
            VkBuffer vertexBuffers[] = {pRenderPipelineObject->vertexBuffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, vertexBuffers, offsets);
            uint32_t vertexCount = pRenderPipelineObject->vertexCount;
            vkCmdDraw(vkCommandBuffer, vertexCount, 1, 0, 0);
        }
    }

    vkCmdEndRenderPass(vkCommandBuffer);
    result = vkEndCommandBuffer(vkCommandBuffer);
    TryThrowVulkanError(result);
}