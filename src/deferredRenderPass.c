#include "deferredRenderPass.h"

void createDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, const char *shadersPath, VkDevice vkDevice, GraphicImage colorGraphicImage, GraphicImage depthGraphicImage, GraphicImage albedoGraphicImage, GraphicImage normalGraphicImage, uint32_t swapchainCount, VkImageView *swapchainImageViews, VkFormat swapchainVkFormat, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer)
{
    printf("CreateDeferredRenderPass:\n");
    VkFormat colorVkFormat = colorGraphicImage.vkFormat;
    VkFormat depthVkFormat = depthGraphicImage.vkFormat;
    VkFormat albedoVkFormat = albedoGraphicImage.vkFormat;
    VkFormat normalVkFormat = normalGraphicImage.vkFormat;

    VkAttachmentDescription colorAttachmentDescription = {
        .flags = 0,
        .format = colorVkFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
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
    VkAttachmentDescription swapchainAttachmentDescription = {
        .flags = 0,
        .format = swapchainVkFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };
    VkAttachmentDescription vkAttachmentDescriptions[] = {
        colorAttachmentDescription,
        depthAttachmentDescription,
        albedoAttachmentDescription,
        normalAttachmentDescription,
        swapchainAttachmentDescription,
    };

    VkAttachmentReference geometryDepthAttachmentReference = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentReference geometryAlbedoAttachmentReference = {
        .attachment = 2,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentReference geometryNormalAttachmentReference = {
        .attachment = 3,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentReference geometryColorAttachments[] = {geometryAlbedoAttachmentReference, geometryNormalAttachmentReference};
    VkSubpassDescription geometrySubpassDescription = {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = NULL,
        .colorAttachmentCount = ARRAY_SIZE(geometryColorAttachments),
        .pColorAttachments = geometryColorAttachments,
        .pResolveAttachments = NULL,
        .pDepthStencilAttachment = &geometryDepthAttachmentReference,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = NULL,
    };

    VkAttachmentReference lightingColorAttachmentReference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentReference lightingDepthAttachmentReference = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
    };
    VkAttachmentReference lightingAlbedoAttachmentReference = {
        .attachment = 2,
        .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    VkAttachmentReference lightingNormalAttachmentReference = {
        .attachment = 3,
        .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    VkAttachmentReference lightInputAttachments[] = {
        lightingDepthAttachmentReference,
        lightingAlbedoAttachmentReference,
        lightingNormalAttachmentReference,
    };
    VkSubpassDescription ligthtingSubpassDescription = {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = ARRAY_SIZE(lightInputAttachments),
        .pInputAttachments = lightInputAttachments,
        .colorAttachmentCount = 1,
        .pColorAttachments = &lightingColorAttachmentReference,
        .pResolveAttachments = NULL,
        .pDepthStencilAttachment = NULL,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = NULL,
    };
    VkAttachmentReference colorAttachmentReference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    VkAttachmentReference swapchainAttachmentReference = {
        .attachment = 4,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    VkSubpassDescription postProcessSubpassDescription = {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 1,
        .pInputAttachments = &colorAttachmentReference,
        .colorAttachmentCount = 1,
        .pColorAttachments = &swapchainAttachmentReference,
        .pResolveAttachments = NULL,
        .pDepthStencilAttachment = NULL,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = NULL,
    };

    VkSubpassDescription vkSubpassDescriptions[] = {
        geometrySubpassDescription,
        ligthtingSubpassDescription,
        postProcessSubpassDescription};
    uint32_t subpassCount = ARRAY_SIZE(vkSubpassDescriptions);
    uint32_t dependencyCount = subpassCount - 1;
    VkSubpassDependency subpassDependencies[dependencyCount];
    for (uint32_t i = 0; i < dependencyCount; i++)
    {
        subpassDependencies[i].srcSubpass = i;
        subpassDependencies[i].dstSubpass = dependencyCount;
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
        .attachmentCount = ARRAY_SIZE(vkAttachmentDescriptions),
        .pAttachments = vkAttachmentDescriptions,
        .subpassCount = subpassCount,
        .pSubpasses = vkSubpassDescriptions,
        .dependencyCount = dependencyCount,
        .pDependencies = subpassDependencies,
    };
    VkResult result = VK_SUCCESS;
    result = vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, NULL, &pDeferredRenderPass->vkRenderPass);
    tryThrowVulkanError(result);

    pDeferredRenderPass->vkFramebufferCount = swapchainCount;
    pDeferredRenderPass->vkFramebuffers = tickernelMalloc(sizeof(VkFramebuffer) * swapchainCount);
    for (size_t i = 0; i < swapchainCount; i++)
    {
        VkImageView attachments[] = {colorGraphicImage.vkImageView, depthGraphicImage.vkImageView, albedoGraphicImage.vkImageView, normalGraphicImage.vkImageView, swapchainImageViews[i]};
        VkFramebufferCreateInfo vkFramebufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .renderPass = pDeferredRenderPass->vkRenderPass,
            .attachmentCount = ARRAY_SIZE(attachments),
            .pAttachments = attachments,
            .width = viewport.width,
            .height = viewport.height,
            .layers = 1,
        };
        VkResult result = vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pDeferredRenderPass->vkFramebuffers[i]);
        tryThrowVulkanError(result);
    }

    // uint32_t subpassIndex = 0;
    // createGeometrySubpass(&pDeferredRenderPass->geometrySubpass, shadersPath, pDeferredRenderPass->vkRenderPass, subpassIndex, vkDevice);
    // subpassIndex++;
    // createLightingSubpass(&pDeferredRenderPass->lightingSubpass, shadersPath, pDeferredRenderPass->vkRenderPass, subpassIndex, vkDevice, viewport, scissor, globalUniformBuffer, lightsUniformBuffer, depthGraphicImage.vkImageView, albedoGraphicImage.vkImageView, normalGraphicImage.vkImageView);
    // subpassIndex++;
    // createPostProcessSubpass(&pDeferredRenderPass->postProcessSubpass, shadersPath, pDeferredRenderPass->vkRenderPass, subpassIndex, vkDevice, viewport, scissor, colorGraphicImage.vkImageView);
}

void destroyDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkDevice vkDevice)
{
    // destroyPostProcessSubpass(&pDeferredRenderPass->postProcessSubpass, vkDevice);
    // destroyGeometrySubpass(&pDeferredRenderPass->geometrySubpass, vkDevice);
    // destroyLightingSubpass(&pDeferredRenderPass->lightingSubpass, vkDevice);

    for (size_t i = 0; i < pDeferredRenderPass->vkFramebufferCount; i++)
    {
        vkDestroyFramebuffer(vkDevice, pDeferredRenderPass->vkFramebuffers[i], NULL);
    }
    tickernelFree(pDeferredRenderPass->vkFramebuffers);
    vkDestroyRenderPass(vkDevice, pDeferredRenderPass->vkRenderPass, NULL);
}

void updateDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, GraphicImage depthGraphicImage, GraphicImage albedoGraphicImage, GraphicImage normalGraphicImage, VkImageView *swapchainImageViews, uint32_t width, uint32_t height, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer)
{
    for (size_t i = 0; i < pDeferredRenderPass->vkFramebufferCount; i++)
    {
        vkDestroyFramebuffer(vkDevice, pDeferredRenderPass->vkFramebuffers[i], NULL);
    }
    for (size_t i = 0; i < pDeferredRenderPass->vkFramebufferCount; i++)
    {
        VkImageView attachments[] = {colorGraphicImage.vkImageView, depthGraphicImage.vkImageView, albedoGraphicImage.vkImageView, normalGraphicImage.vkImageView, swapchainImageViews[i]};
        VkFramebufferCreateInfo vkFramebufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .renderPass = pDeferredRenderPass->vkRenderPass,
            .attachmentCount = ARRAY_SIZE(attachments),
            .pAttachments = attachments,
            .width = width,
            .height = height,
            .layers = 1,
        };
        VkResult result = vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pDeferredRenderPass->vkFramebuffers[i]);
        tryThrowVulkanError(result);
    }

    Subpass *pLightingSubpass = &pDeferredRenderPass->lightingSubpass;
    recreateLightingSubpassModel(pLightingSubpass, vkDevice, globalUniformBuffer, lightsUniformBuffer, depthGraphicImage.vkImageView, albedoGraphicImage.vkImageView, normalGraphicImage.vkImageView);
    Subpass *pPostProcessSubpass = &pDeferredRenderPass->postProcessSubpass;
    recreatePostProcessSubpassModel(pPostProcessSubpass, vkDevice, colorGraphicImage.vkImageView);
}

void recordDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, VkDevice vkDevice, uint32_t swapchainIndex)
{
    vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
    vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
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
    uint32_t clearValueCount = 5;
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
        {
            .color = {0.0f, 0.0f, 0.0f, 0.0f},
        },
    };
    VkRenderPassBeginInfo renderPassBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = NULL,
            .renderPass = pDeferredRenderPass->vkRenderPass,
            .framebuffer = pDeferredRenderPass->vkFramebuffers[swapchainIndex],
            .renderArea = renderArea,
            .clearValueCount = clearValueCount,
            .pClearValues = clearValues,
        };
    vkCmdBeginRenderPass(vkCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    recordGeometrySubpass(&pDeferredRenderPass->geometrySubpass, vkCommandBuffer);
    vkCmdNextSubpass(vkCommandBuffer, VK_SUBPASS_CONTENTS_INLINE);
    recordLightingSubpass(&pDeferredRenderPass->lightingSubpass, vkCommandBuffer);
    vkCmdNextSubpass(vkCommandBuffer, VK_SUBPASS_CONTENTS_INLINE);
    recordPostProcessSubpass(&pDeferredRenderPass->postProcessSubpass, vkCommandBuffer);
    vkCmdEndRenderPass(vkCommandBuffer);
}
