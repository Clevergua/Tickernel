#include "uiRenderPass.h"

static void createVkRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice, VkFormat colorVkFormat, VkFormat depthVkFormat)
{
    VkAttachmentDescription colorAttachmentDescription = {
        .flags = 0,
        .format = colorVkFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };
    VkAttachmentDescription depthAttachmentDescription = {
        .flags = 0,
        .format = depthVkFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
    uint32_t attachmentCount = 2;
    VkAttachmentDescription vkAttachmentDescriptions[] = {
        colorAttachmentDescription,
        depthAttachmentDescription,
    };

    VkAttachmentReference colorAttachmentReference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentReference depthAttachmentReference = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
    VkSubpassDescription uiSubpassDescription = {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = NULL,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentReference,
        .pResolveAttachments = NULL,
        .pDepthStencilAttachment = &depthAttachmentReference,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = NULL,
    };

    uint32_t subpassCount = 1;
    VkSubpassDescription vkSubpassDescriptions[] = {
        uiSubpassDescription,
    };

    VkRenderPassCreateInfo vkRenderPassCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .attachmentCount = attachmentCount,
        .pAttachments = vkAttachmentDescriptions,
        .subpassCount = subpassCount,
        .pSubpasses = vkSubpassDescriptions,
        .dependencyCount = 0,
        .pDependencies = NULL,
    };
    VkResult result = vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, NULL, &pUIRenderPass->vkRenderPass);
    tryThrowVulkanError(result);
}

static void destroyVkRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice)
{
    vkDestroyRenderPass(vkDevice, pUIRenderPass->vkRenderPass, NULL);
}

void createUIRenderPass(UIRenderPass *pUIRenderPass, const char *shadersPath, VkDevice vkDevice, GraphicImage colorGraphicImage, GraphicImage depthGraphicImage, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer)
{
    printf("CreateUIRenderPass:\n");
    createVkRenderPass(pUIRenderPass, vkDevice, colorGraphicImage.vkFormat, depthGraphicImage.vkFormat);
    VkImageView attachments[] = {colorGraphicImage.vkImageView, depthGraphicImage.vkImageView};
    VkFramebufferCreateInfo vkFramebufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .renderPass = pUIRenderPass->vkRenderPass,
        .attachmentCount = 2,
        .pAttachments = attachments,
        .width = viewport.width,
        .height = viewport.height,
        .layers = 1,
    };
    VkResult result = vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pUIRenderPass->vkFramebuffer);
    tryThrowVulkanError(result);

    uint32_t subpassIndex = 0;
    createUISubpass(&pUIRenderPass->uiSubpass, shadersPath, pUIRenderPass->vkRenderPass, subpassIndex, vkDevice, viewport, scissor);
}

void destroyUIRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice)
{
    destroyUISubpass(&pUIRenderPass->uiSubpass, vkDevice);
    vkDestroyFramebuffer(vkDevice, pUIRenderPass->vkFramebuffer, NULL);
    destroyVkRenderPass(pUIRenderPass, vkDevice);
}

void updateUIRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, GraphicImage depthGraphicImage, uint32_t width, uint32_t height, VkBuffer globalUniformBuffer)
{
    vkDestroyFramebuffer(vkDevice, pUIRenderPass->vkFramebuffer, NULL);
    VkImageView attachments[] = {colorGraphicImage.vkImageView, depthGraphicImage.vkImageView};
    VkFramebufferCreateInfo vkFramebufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .renderPass = pUIRenderPass->vkRenderPass,
        .attachmentCount = 2,
        .pAttachments = attachments,
        .width = width,
        .height = height,
        .layers = 1,
    };
    VkResult result = vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pUIRenderPass->vkFramebuffer);
    tryThrowVulkanError(result);
}

void recordUIRenderPass(UIRenderPass *pUIRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, VkDevice vkDevice)
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
    uint32_t clearValueCount = 2;
    VkClearValue *clearValues = (VkClearValue[]){
        {
            .color = {0.0f, 0.0f, 0.0f, 1.0f},
        },
        {
            .depthStencil = {1.0f, 0},
        },
    };
    VkRenderPassBeginInfo renderPassBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = NULL,
        .renderPass = pUIRenderPass->vkRenderPass,
        .framebuffer = pUIRenderPass->vkFramebuffer,
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

