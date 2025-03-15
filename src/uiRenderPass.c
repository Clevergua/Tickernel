#include "uiRenderPass.h"

static void createVkRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice, VkFormat swapchainVkFormat)
{
    VkAttachmentDescription swapchainAttachmentDescription = {
        .flags = 0,
        .format = swapchainVkFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    uint32_t attachmentCount = 1;
    VkAttachmentDescription vkAttachmentDescriptions[] = {
        swapchainAttachmentDescription,
    };

    VkAttachmentReference swapchainAttachmentReference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription uiSubpassDescription = {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = NULL,
        .colorAttachmentCount = 1,
        .pColorAttachments = &swapchainAttachmentReference,
        .pResolveAttachments = NULL,
        .pDepthStencilAttachment = NULL,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = NULL,
    };

    uint32_t subpassCount = 1;
    VkSubpassDescription vkSubpassDescriptions[] = {
        uiSubpassDescription,
    };

    VkSubpassDependency dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = 0,
    };

    VkRenderPassCreateInfo vkRenderPassCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .attachmentCount = attachmentCount,
        .pAttachments = vkAttachmentDescriptions,
        .subpassCount = subpassCount,
        .pSubpasses = vkSubpassDescriptions,
        .dependencyCount = 1,
        .pDependencies = &dependency,
    };
    VkResult result = VK_SUCCESS;
    result = vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, NULL, &pUIRenderPass->vkRenderPass);
    tryThrowVulkanError(result);
}

static void destroyVkRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice)
{
    vkDestroyRenderPass(vkDevice, pUIRenderPass->vkRenderPass, NULL);
}

void createUIRenderPass(UIRenderPass *pUIRenderPass, const char *shadersPath, VkDevice vkDevice, GraphicImage colorGraphicImage, VkViewport viewport, VkRect2D scissor, uint32_t swapchainCount, VkImageView *swapchainImageViews, VkFormat swapchainVkFormat)
{
    printf("createUIRenderPass:\n");
    createVkRenderPass(pUIRenderPass, vkDevice, swapchainVkFormat);
    
    pUIRenderPass->vkFramebufferCount = swapchainCount;
    pUIRenderPass->vkFramebuffers = tickernelMalloc(sizeof(VkFramebuffer) * swapchainCount);
    
    for (size_t i = 0; i < swapchainCount; i++)
    {
        VkImageView attachments[] = {swapchainImageViews[i]};
        VkFramebufferCreateInfo vkFramebufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .renderPass = pUIRenderPass->vkRenderPass,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = viewport.width,
            .height = viewport.height,
            .layers = 1,
        };
        VkResult result = vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pUIRenderPass->vkFramebuffers[i]);
        tryThrowVulkanError(result);
    }
    
    createUISubpass(&pUIRenderPass->uiSubpass, shadersPath, pUIRenderPass->vkRenderPass, vkDevice, viewport, scissor);
}

void destroyUIRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice)
{
    destroyUISubpass(&pUIRenderPass->uiSubpass, vkDevice);
    
    for (size_t i = 0; i < pUIRenderPass->vkFramebufferCount; i++)
    {
        vkDestroyFramebuffer(vkDevice, pUIRenderPass->vkFramebuffers[i], NULL);
    }
    tickernelFree(pUIRenderPass->vkFramebuffers);
    
    destroyVkRenderPass(pUIRenderPass, vkDevice);
}

void updateUIRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, uint32_t width, uint32_t height, VkImageView *swapchainImageViews)
{
    for (size_t i = 0; i < pUIRenderPass->vkFramebufferCount; i++)
    {
        vkDestroyFramebuffer(vkDevice, pUIRenderPass->vkFramebuffers[i], NULL);
        
        VkImageView attachments[] = {swapchainImageViews[i]};
        VkFramebufferCreateInfo vkFramebufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .renderPass = pUIRenderPass->vkRenderPass,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = width,
            .height = height,
            .layers = 1,
        };
        VkResult result = vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pUIRenderPass->vkFramebuffers[i]);
        tryThrowVulkanError(result);
    }
    
    recreateUISubpassModel(&pUIRenderPass->uiSubpass, vkDevice);
}

void recordUIRenderPass(UIRenderPass *pUIRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, VkDevice vkDevice, uint32_t swapchainIndex)
{
    vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
    VkOffset2D offset = {
        .x = 0,
        .y = 0,
    };
    VkRect2D renderArea = {
        .offset = offset,
        .extent = scissor.extent,
    };
    
    uint32_t clearValueCount = 1;
    VkClearValue clearValues[] = {
        {
            .color = {0.0f, 0.0f, 0.0f, 0.0f},
        },
    };
    
    VkRenderPassBeginInfo renderPassBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = NULL,
        .renderPass = pUIRenderPass->vkRenderPass,
        .framebuffer = pUIRenderPass->vkFramebuffers[swapchainIndex],
        .renderArea = renderArea,
        .clearValueCount = clearValueCount,
        .pClearValues = clearValues,
    };

    vkCmdBeginRenderPass(vkCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    Subpass *pUISubpass = &pUIRenderPass->uiSubpass;
    vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pUISubpass->vkPipeline);
    
    for (uint32_t modelIndex = 0; modelIndex < pUISubpass->modelCollection.length; modelIndex++)
    {
        SubpassModel *pSubpassModel = pUISubpass->modelCollection.array[modelIndex];
        if (NULL != pSubpassModel)
        {
            vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
            vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
            vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pUISubpass->vkPipelineLayout, 0, 1, &pSubpassModel->vkDescriptorSet, 0, NULL);
            
            VkBuffer vertexBuffers[] = {pSubpassModel->vertexBuffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, vertexBuffers, offsets);
            
            vkCmdDraw(vkCommandBuffer, pSubpassModel->vertexCount, 1, 0, 0);
        }
    }
    
    vkCmdEndRenderPass(vkCommandBuffer);
}

