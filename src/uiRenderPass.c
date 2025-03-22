#include "uiRenderPass.h"

void createUIRenderPass(UIRenderPass *pUIRenderPass, const char *shadersPath, VkDevice vkDevice, GraphicImage colorGraphicImage, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer)
{
    // Create render pass
    VkAttachmentDescription colorAttachment = {
        .format = colorGraphicImage.vkFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference colorAttachmentRef = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef
    };

    VkSubpassDependency dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
    };

    VkRenderPassCreateInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &colorAttachment,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency
    };

    if (vkCreateRenderPass(vkDevice, &renderPassInfo, NULL, &pUIRenderPass->vkRenderPass) != VK_SUCCESS)
    {
        printf("Failed to create UI render pass!\n");
        exit(1);
    }

    // Create framebuffer
    VkFramebufferCreateInfo framebufferInfo = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = pUIRenderPass->vkRenderPass,
        .attachmentCount = 1,
        .pAttachments = &colorGraphicImage.vkImageView,
        .width = scissor.extent.width,
        .height = scissor.extent.height,
        .layers = 1
    };

    if (vkCreateFramebuffer(vkDevice, &framebufferInfo, NULL, &pUIRenderPass->vkFramebuffer) != VK_SUCCESS)
    {
        printf("Failed to create UI framebuffer!\n");
        exit(1);
    }

    // Create UI subpass
    createUISubpass(&pUIRenderPass->uiSubpass, shadersPath, pUIRenderPass->vkRenderPass, 0, vkDevice, viewport, scissor, globalUniformBuffer);
}

void destroyUIRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice)
{
    destroyUISubpass(&pUIRenderPass->uiSubpass, vkDevice);
    vkDestroyFramebuffer(vkDevice, pUIRenderPass->vkFramebuffer, NULL);
    vkDestroyRenderPass(vkDevice, pUIRenderPass->vkRenderPass, NULL);
}

void updateUIRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, uint32_t width, uint32_t height, VkBuffer globalUniformBuffer)
{
    // Update UI subpass
    updateUISubpass(&pUIRenderPass->uiSubpass, vkDevice, globalUniformBuffer);
}

void recordUIRenderPass(UIRenderPass *pUIRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, VkDevice vkDevice)
{
    VkRenderPassBeginInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = pUIRenderPass->vkRenderPass,
        .framebuffer = pUIRenderPass->vkFramebuffer,
        .renderArea.offset = {0, 0},
        .renderArea.extent = {viewport.width, viewport.height}
    };

    vkCmdBeginRenderPass(vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);

    // Record UI subpass commands
    recordUISubpass(&pUIRenderPass->uiSubpass, vkCommandBuffer, vkDevice);

    vkCmdEndRenderPass(vkCommandBuffer);
}