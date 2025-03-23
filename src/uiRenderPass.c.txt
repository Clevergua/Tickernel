#include "uiRenderPass.h"

void createVkRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice, VkFormat colorVkFormat, VkFormat swapchainVkFormat)
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

    VkSubpassDescription subpassDescription = {
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
        subpassDescription,
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
    result = vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, NULL, &pUIRenderPass->vkRenderPass);
    tryThrowVulkanError(result);
}
static void destroyVkRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice)
{
    vkDestroyRenderPass(vkDevice, pUIRenderPass->vkRenderPass, NULL);
}

void createUIRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice, VkFormat colorVkFormat, VkFormat swapchainVkFormat)
{
    createVkRenderPass(pUIRenderPass, vkDevice, colorVkFormat, swapchainVkFormat);

}
void destroyUIRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice)
{

    destroyVkRenderPass(pUIRenderPass, vkDevice);
}

void updateUIRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, uint32_t width, uint32_t height, VkBuffer globalUniformBuffer)
{

}

void recordUIRenderPass(UIRenderPass *pUIRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, VkDevice vkDevice)
{
    
}