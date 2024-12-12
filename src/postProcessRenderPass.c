#include <postProcessRenderPass.h>
static void CreateVkRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkDevice vkDevice, VkFormat colorVkFormat, VkFormat swapchainVkFormat)
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

    VkAttachmentReference opaqueLightingColorAttachmentReference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentReference opaqueLightingColorAttachmentReference = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription geometrySubpassDescription = {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = NULL,
        .colorAttachmentCount = 1,
        .pColorAttachments = colorAttachments,
        .pResolveAttachments = NULL,
        .pDepthStencilAttachment = &opaqueGeometryDepthAttachmentReference,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = NULL,
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

    uint32_t subpassCount = 2;
    VkSubpassDescription vkSubpassDescriptions[] = {
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

void CreatePostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, VkViewport viewport, VkRect2D scissor)
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
void DestroyPostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkDevice vkDevice)
{
}
void UpdatePostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, uint32_t width, uint32_t height);
void RecordPostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, VkDevice vkDevice);