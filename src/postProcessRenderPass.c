#include "postProcessRenderPass.h"
static void CreateVkRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkDevice vkDevice, VkFormat colorVkFormat, VkFormat swapchainVkFormat)
{
    VkAttachmentDescription colorAttachmentDescription = {
        .flags = 0,
        .format = colorVkFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
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
    uint32_t attachmentCount = 2;
    VkAttachmentDescription vkAttachmentDescriptions[] = {
        colorAttachmentDescription,
        swapchainAttachmentDescription,
    };

    VkAttachmentReference colorAttachmentReference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    VkAttachmentReference swapchainAttachmentReference = {
        .attachment = 1,
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

    uint32_t subpassCount = 1;
    VkSubpassDescription vkSubpassDescriptions[] = {
        postProcessSubpassDescription,
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
    VkResult result = VK_SUCCESS;
    result = vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, NULL, &pPostProcessRenderPass->vkRenderPass);
    TryThrowVulkanError(result);
}
static void DestroyVkRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkDevice vkDevice)
{
    vkDestroyRenderPass(vkDevice, pPostProcessRenderPass->vkRenderPass, NULL);
}

void CreatePostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, char *shadersPath, VkDevice vkDevice, GraphicImage colorGraphicImage, VkViewport viewport, VkRect2D scissor, uint32_t swapchainCount, VkImageView *swapchainImageViews, VkFormat swapchainVkFormat)
{
    printf("CreatePostProcessRenderPass:\n");
    CreateVkRenderPass(pPostProcessRenderPass, vkDevice, colorGraphicImage.vkFormat, swapchainVkFormat);
    pPostProcessRenderPass->vkFramebufferCount = swapchainCount;
    pPostProcessRenderPass->vkFramebuffers = TickernelMalloc(sizeof(VkFramebuffer) * swapchainCount);
    for (size_t i = 0; i < swapchainCount; i++)
    {
        VkImageView attachments[] = {colorGraphicImage.vkImageView, swapchainImageViews[i]};
        VkFramebufferCreateInfo vkFramebufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .renderPass = pPostProcessRenderPass->vkRenderPass,
            .attachmentCount = 2,
            .pAttachments = attachments,
            .width = viewport.width,
            .height = viewport.height,
            .layers = 1,
        };
        VkResult result = vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pPostProcessRenderPass->vkFramebuffers[i]);
        TryThrowVulkanError(result);
    }
    CreatePostProcessSubpass(&pPostProcessRenderPass->postProcessSubpass, shadersPath, pPostProcessRenderPass->vkRenderPass, vkDevice, viewport, scissor, colorGraphicImage.vkImageView);
}
void DestroyPostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkDevice vkDevice)
{
    DestroyPostProcessSubpass(&pPostProcessRenderPass->postProcessSubpass, vkDevice);
    for (size_t i = 0; i < pPostProcessRenderPass->vkFramebufferCount; i++)
    {
        vkDestroyFramebuffer(vkDevice, pPostProcessRenderPass->vkFramebuffers[i], NULL);
    }
    TickernelFree(pPostProcessRenderPass->vkFramebuffers);
    DestroyVkRenderPass(pPostProcessRenderPass, vkDevice);
}
void UpdatePostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, uint32_t width, uint32_t height, VkImageView *swapchainImageViews)
{
    for (size_t i = 0; i < pPostProcessRenderPass->vkFramebufferCount; i++)
    {
        vkDestroyFramebuffer(vkDevice, pPostProcessRenderPass->vkFramebuffers[i], NULL);

        VkImageView attachments[] = {colorGraphicImage.vkImageView, swapchainImageViews[i]};
        VkFramebufferCreateInfo vkFramebufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .renderPass = pPostProcessRenderPass->vkRenderPass,
            .attachmentCount = 2,
            .pAttachments = attachments,
            .width = width,
            .height = height,
            .layers = 1,
        };
        VkResult result = vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pPostProcessRenderPass->vkFramebuffers[i]);
        TryThrowVulkanError(result);
    }
    RecreatePostProcessSubpassModel(&pPostProcessRenderPass->postProcessSubpass, vkDevice, colorGraphicImage.vkImageView);
}
void RecordPostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, VkDevice vkDevice, uint32_t frameIndex)
{
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
    uint32_t clearValueCount = 2;
    VkClearValue *clearValues = (VkClearValue[]){
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
            .renderPass = pPostProcessRenderPass->vkRenderPass,
            .framebuffer = pPostProcessRenderPass->vkFramebuffers[frameIndex],
            .renderArea = renderArea,
            .clearValueCount = clearValueCount,
            .pClearValues = clearValues,
        };

    vkCmdBeginRenderPass(vkCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    Subpass *pPostProcessSubpass = &pPostProcessRenderPass->postProcessSubpass;
    SubpassModel *pSubpassModel = pPostProcessSubpass->modelCollection.array[0];
    vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPostProcessSubpass->vkPipeline);
    vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
    vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPostProcessSubpass->vkPipelineLayout, 0, 1, &pSubpassModel->vkDescriptorSet, 0, NULL);
    vkCmdDraw(vkCommandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(vkCommandBuffer);
}
