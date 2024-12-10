#include <deferredRenderPass.h>

static void CreateVkRenderPass(DeferredRenderPass *pDeferredRenderPass, VkDevice vkDevice, VkFormat colorVkFormat, VkFormat depthVkFormat, VkFormat albedoVkFormat, VkFormat normalVkFormat)
{
    VkAttachmentDescription colorAttachmentDescription = {
        .flags = 0,
        .format = colorVkFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };
    VkAttachmentDescription depthAttachmentDescription = {
        .flags = 0,
        .format = depthVkFormat,
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
        .format = albedoVkFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    VkAttachmentDescription normalAttachmentDescription = {
        .flags = 0,
        .format = normalVkFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    uint32_t attachmentCount = 4;
    VkAttachmentDescription vkAttachmentDescriptions[] = {
        colorAttachmentDescription,
        depthAttachmentDescription,
        albedoAttachmentDescription,
        normalAttachmentDescription,
    };

    VkAttachmentReference opaqueGeometryDepthAttachmentReference = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentReference opaqueGeometryAlbedoAttachmentReference = {
        .attachment = 2,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentReference opaqueGeometryNormalAttachmentReference = {
        .attachment = 3,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentReference colorAttachments[] = {opaqueGeometryAlbedoAttachmentReference, opaqueGeometryNormalAttachmentReference};
    VkSubpassDescription geometrySubpassDescription = {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = NULL,
        .colorAttachmentCount = 2,
        .pColorAttachments = colorAttachments,
        .pResolveAttachments = NULL,
        .pDepthStencilAttachment = &opaqueGeometryDepthAttachmentReference,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = NULL,
    };

    VkAttachmentReference opaqueLightingColorAttachmentReference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentReference opaqueLightingDepthAttachmentReference = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
    };
    VkAttachmentReference opaqueLightingAlbedoAttachmentReference = {
        .attachment = 2,
        .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    VkAttachmentReference opaqueLightingNormalAttachmentReference = {
        .attachment = 3,
        .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    VkSubpassDescription ligthtingSubpassDescription = {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 3,
        .pInputAttachments = (const VkAttachmentReference[]){opaqueLightingDepthAttachmentReference, opaqueLightingAlbedoAttachmentReference, opaqueLightingNormalAttachmentReference},
        .colorAttachmentCount = 1,
        .pColorAttachments = &opaqueLightingColorAttachmentReference,
        .pResolveAttachments = NULL,
        .pDepthStencilAttachment = NULL,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = NULL,
    };

    uint32_t subpassCount = 4;
    VkSubpassDescription vkSubpassDescriptions[] = {
        geometrySubpassDescription,
        ligthtingSubpassDescription,
        geometrySubpassDescription,
        ligthtingSubpassDescription,
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
    result = vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, NULL, &pDeferredRenderPass->vkRenderPass);
    TryThrowVulkanError(result);
}
static void DestroyVkRenderPass(DeferredRenderPass *pDeferredRenderPass, VkDevice vkDevice)
{
    vkDestroyRenderPass(vkDevice, pDeferredRenderPass->vkRenderPass, NULL);
}

void CreateDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, GraphicImage depthGraphicImage, GraphicImage albedoGraphicImage, GraphicImage normalGraphicImage, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer)
{
    CreateVkRenderPass(pDeferredRenderPass, vkDevice, colorGraphicImage.vkFormat, depthGraphicImage.vkFormat, albedoGraphicImage.vkFormat, normalGraphicImage.vkFormat);
    VkImageView attachments[] = {colorGraphicImage.vkImageView, depthGraphicImage.vkImageView, albedoGraphicImage.vkImageView, normalGraphicImage.vkImageView};
    VkFramebufferCreateInfo vkFramebufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .renderPass = pDeferredRenderPass->vkRenderPass,
        .attachmentCount = 4,
        .pAttachments = attachments,
        .width = viewport.width,
        .height = viewport.height,
        .layers = 1,
    };
    VkResult result = vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pDeferredRenderPass->vkFramebuffer);
    TryThrowVulkanError(result);

    uint32_t subpassIndex = 0;
    CreateOpaqueGeometrySubpass(&pDeferredRenderPass->opaqueGeometrySubpass, pDeferredRenderPass->shadersPath, pDeferredRenderPass->vkRenderPass, subpassIndex, vkDevice, viewport, scissor);
    subpassIndex++;
    CreateOpaqueLightingSubpass(&pDeferredRenderPass->opaqueLightingSubpass, pDeferredRenderPass->shadersPath, pDeferredRenderPass->vkRenderPass, subpassIndex, vkDevice, viewport, scissor, globalUniformBuffer, lightsUniformBuffer, depthGraphicImage.vkImageView, albedoGraphicImage.vkImageView, normalGraphicImage.vkImageView);
}
void DestroyDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkDevice vkDevice)
{
    DestroyOpaqueGeometrySubpass(&pDeferredRenderPass->opaqueGeometrySubpass, vkDevice);
    DestroyOpaqueLightingSubpass(&pDeferredRenderPass->opaqueLightingSubpass, vkDevice);
    vkDestroyFramebuffer(vkDevice, pDeferredRenderPass->vkFramebuffer, NULL);
    DestroyVkRenderPass(pDeferredRenderPass, vkDevice);
}

void RecordDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, uint32_t frameIndex, VkImageView colorVkImageView, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView, VkDevice vkDevice)
{
    VkCommandBufferBeginInfo vkCommandBufferBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = NULL,
            .flags = 0,
            .pInheritanceInfo = NULL,
        };
    VkResult result = vkBeginCommandBuffer(vkCommandBuffer, &vkCommandBufferBeginInfo);
    TryThrowVulkanError(result);
    vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
    VkOffset2D offset =
        {
            .x = 0,
            .y = 0,
        };
    VkRect2D renderArea =
        {
            .offset = offset,
            .extent = scissor.extent,
        };
    uint32_t clearValueCount = 4;
    VkClearValue *clearValues = (VkClearValue[]){
        {
            .color = {0.0f, 0.0f, 0.0f, 1.0f},
        },
        {
            .depthStencil = {1.0f, 0},
        },
        {
            .color = {0.0f, 0.0f, 0.0f, 1.0f},
        },
        {
            .color = {0.0f, 0.0f, 0.0f, 0.0f},
        },
    };
    VkRenderPassBeginInfo renderPassBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = NULL,
            .renderPass = pDeferredRenderPass->vkRenderPass,
            .framebuffer = pDeferredRenderPass->vkFramebuffer,
            .renderArea = renderArea,
            .clearValueCount = clearValueCount,
            .pClearValues = clearValues,
        };

    vkCmdBeginRenderPass(vkCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    Subpass *pOpaqueGeometrySubpass = &pDeferredRenderPass->opaqueGeometrySubpass;
    vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pOpaqueGeometrySubpass->vkPipeline);
    for (uint32_t modelIndex = 0; modelIndex < pOpaqueGeometrySubpass->modelCollection.length; modelIndex++)
    {
        SubpassModel *pSubpassModel = pOpaqueGeometrySubpass->modelCollection.array[modelIndex];
        if (NULL != pSubpassModel)
        {
            vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
            vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
            vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pOpaqueGeometrySubpass->vkPipelineLayout, 0, 1, &pSubpassModel->vkDescriptorSet, 0, NULL);
            VkBuffer vertexBuffers[] = {pSubpassModel->vertexBuffer, pSubpassModel->instanceBuffer};
            VkDeviceSize offsets[] = {0, 0};
            vkCmdBindVertexBuffers(vkCommandBuffer, 0, 2, vertexBuffers, offsets);
            vkCmdDraw(vkCommandBuffer, pSubpassModel->vertexCount, pSubpassModel->instanceCount, 0, 0);
        }
    }
    vkCmdNextSubpass(vkCommandBuffer, VK_SUBPASS_CONTENTS_INLINE);
    // opaqueLighting subpass
    Subpass *pOpaqueLightingSubpass = &pDeferredRenderPass->opaqueLightingSubpass;
    SubpassModel *pSubpassModel = pOpaqueLightingSubpass->modelCollection.array[0];
    vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pOpaqueLightingSubpass->vkPipeline);
    vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
    vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pOpaqueLightingSubpass->vkPipelineLayout, 0, 1, &pSubpassModel->vkDescriptorSet, 0, NULL);
    vkCmdDraw(vkCommandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(vkCommandBuffer);

    // VkRenderPassBeginInfo renderPassBeginInfo =
    //     {
    //         .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    //         .pNext = NULL,
    //         .renderPass = pDeferredRenderPass->vkRenderPass,
    //         .framebuffer = pDeferredRenderPass->vkFramebuffer,
    //         .renderArea = renderArea,
    //         .clearValueCount = clearValueCount,
    //         .pClearValues = clearValues,
    //     };
    // vkCmdBeginRenderPass(vkCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    // Subpass *pOpaqueGeometrySubpass = &pDeferredRenderPass->opaqueGeometrySubpass;
    // vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pOpaqueGeometrySubpass->vkPipeline);
    // for (uint32_t modelIndex = 0; modelIndex < pOpaqueGeometrySubpass->modelCollection.length; modelIndex++)
    // {
    //     SubpassModel *pSubpassModel = pOpaqueGeometrySubpass->modelCollection.array[modelIndex];
    //     if (NULL != pSubpassModel)
    //     {
    //         vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
    //         vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
    //         vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pOpaqueGeometrySubpass->vkPipelineLayout, 0, 1, &pSubpassModel->vkDescriptorSet, 0, NULL);
    //         VkBuffer vertexBuffers[] = {pSubpassModel->vertexBuffer, pSubpassModel->instanceBuffer};
    //         VkDeviceSize offsets[] = {0, 0};
    //         vkCmdBindVertexBuffers(vkCommandBuffer, 0, 2, vertexBuffers, offsets);
    //         vkCmdDraw(vkCommandBuffer, pSubpassModel->vertexCount, pSubpassModel->instanceCount, 0, 0);
    //     }
    // }
    // vkCmdNextSubpass(vkCommandBuffer, VK_SUBPASS_CONTENTS_INLINE);
    // // opaqueLighting subpass
    // Subpass *pOpaqueLightingSubpass = &pDeferredRenderPass->opaqueLightingSubpass;
    // SubpassModel *pSubpassModel = pOpaqueLightingSubpass->modelCollection.array[0];
    // vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pOpaqueLightingSubpass->vkPipeline);
    // vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
    // vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
    // vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pOpaqueLightingSubpass->vkPipelineLayout, 0, 1, &pSubpassModel->vkDescriptorSet, 0, NULL);
    // vkCmdDraw(vkCommandBuffer, 3, 1, 0, 0);
    // vkCmdEndRenderPass(vkCommandBuffer);

    result = vkEndCommandBuffer(vkCommandBuffer);
    TryThrowVulkanError(result);
}

void RecreateDeferredRenderPassFrameBuffer(DeferredRenderPass *pDeferredRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, GraphicImage depthGraphicImage, GraphicImage albedoGraphicImage, GraphicImage normalGraphicImage, uint32_t width, uint32_t height)
{
    VkImageView attachments[] = {colorGraphicImage.vkImageView, depthGraphicImage.vkImageView, albedoGraphicImage.vkImageView, normalGraphicImage.vkImageView};
    VkFramebufferCreateInfo vkFramebufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .renderPass = pDeferredRenderPass->vkRenderPass,
        .attachmentCount = 4,
        .pAttachments = attachments,
        .width = width,
        .height = height,
        .layers = 1,
    };
    VkResult result = vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pDeferredRenderPass->vkFramebuffer);
    TryThrowVulkanError(result);
}