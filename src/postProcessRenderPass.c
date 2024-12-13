#include <postProcessRenderPass.h>
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
        .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
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
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
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

void CreatePostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, VkViewport viewport, VkRect2D scissor, uint32_t swapchainCount, VkImageView *swapchainImageViews)
{

    CreateVkRenderPass(pPostProcessRenderPass, vkDevice, colorGraphicImage.vkFormat, colorGraphicImage.vkFormat);
    pPostProcessRenderPass->vkFramebufferCount = swapchainCount;
    for (size_t i = 0; i < swapchainCount; i++)
    {
        VkImageView attachments[] = {swapchainImageViews[i]};
        VkFramebufferCreateInfo vkFramebufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .renderPass = pPostProcessRenderPass->vkRenderPass,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = viewport.width,
            .height = viewport.height,
            .layers = 1,
        };
        VkResult result = vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pPostProcessRenderPass->vkFramebuffers[i]);
        TryThrowVulkanError(result);
    }
    uint32_t subpassIndex = 0;
    CreatePostProcessSubpass(&pPostProcessRenderPass->postProcessSubpass, pPostProcessRenderPass->shadersPath, pPostProcessRenderPass->vkRenderPass, subpassIndex, vkDevice, viewport, scissor);
}
void DestroyPostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkDevice vkDevice)
{
    for (size_t i = 0; i < pPostProcessRenderPass->vkFramebufferCount; i++)
    {
        vkDestroyFramebuffer(vkDevice, pPostProcessRenderPass->vkFramebuffers[i], NULL);
    }
    DestroyVkRenderPass(pPostProcessRenderPass, vkDevice);
}
void UpdatePostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, uint32_t width, uint32_t height, VkImageView *swapchainImageViews)
{
    for (size_t i = 0; i < pPostProcessRenderPass->vkFramebufferCount; i++)
    {
        vkDestroyFramebuffer(vkDevice, pPostProcessRenderPass->vkFramebuffers[i], NULL);

        VkImageView attachments[] = {swapchainImageViews[i]};
        VkFramebufferCreateInfo vkFramebufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .renderPass = pPostProcessRenderPass->vkRenderPass,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = width,
            .height = height,
            .layers = 1,
        };
        VkResult result = vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pPostProcessRenderPass->vkFramebuffers[i]);
        TryThrowVulkanError(result);
    }
}
void RecordPostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, VkDevice vkDevice)
{
    
}