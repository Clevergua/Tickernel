// #include "uiRenderPass.h"

// void createVkRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice, VkFormat swapchainVkFormat)
// {
//     VkAttachmentDescription swapchainAttachmentDescription = {
//         .flags = 0,
//         .format = swapchainVkFormat,
//         .samples = VK_SAMPLE_COUNT_1_BIT,
//         .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
//         .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
//         .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
//         .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
//         .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
//         .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
//     };
//     uint32_t attachmentCount = 1;
//     VkAttachmentDescription vkAttachmentDescriptions[] = {
//         swapchainAttachmentDescription,
//     };

//     VkAttachmentReference swapchainAttachmentReference = {
//         .attachment = 0,
//         .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
//     };

//     VkSubpassDescription subpassDescription = {
//         .flags = 0,
//         .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
//         .inputAttachmentCount = 0,
//         .pInputAttachments = NULL,
//         .colorAttachmentCount = 1,
//         .pColorAttachments = &swapchainAttachmentReference,
//         .pResolveAttachments = NULL,
//         .pDepthStencilAttachment = NULL,
//         .preserveAttachmentCount = 0,
//         .pPreserveAttachments = NULL,
//     };

//     uint32_t subpassCount = 1;
//     VkSubpassDescription vkSubpassDescriptions[] = {
//         subpassDescription,
//     };

//     VkRenderPassCreateInfo vkRenderPassCreateInfo = {
//         .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
//         .pNext = NULL,
//         .flags = 0,
//         .attachmentCount = attachmentCount,
//         .pAttachments = vkAttachmentDescriptions,
//         .subpassCount = subpassCount,
//         .pSubpasses = vkSubpassDescriptions,
//         .dependencyCount = 0,
//         .pDependencies = NULL,
//     };

//     VkResult result = VK_SUCCESS;
//     result = vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, NULL, &pUIRenderPass->vkRenderPass);
//     tryThrowVulkanError(result);
// }
// static void destroyVkRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice)
// {
//     vkDestroyRenderPass(vkDevice, pUIRenderPass->vkRenderPass, NULL);
// }

// void createUIRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice, VkFormat swapchainVkFormat, uint32_t swapchainCount, VkImageView *swapchainImageViews, VkViewport viewport, VkRect2D scissor)
// {
//     createVkRenderPass(pUIRenderPass, vkDevice, swapchainVkFormat);
//     pUIRenderPass->vkFramebufferCount = swapchainCount;
//     pUIRenderPass->vkFramebuffers = tickernelMalloc(sizeof(VkFramebuffer) * swapchainCount);
//     for (size_t i = 0; i < swapchainCount; i++)
//     {
//         VkImageView attachments[] = {swapchainImageViews[i]};
//         VkFramebufferCreateInfo vkFramebufferCreateInfo = {
//             .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
//             .pNext = NULL,
//             .flags = 0,
//             .renderPass = pUIRenderPass->vkRenderPass,
//             .attachmentCount = 1,
//             .pAttachments = attachments,
//             .width = viewport.width,
//             .height = viewport.height,
//             .layers = 1,
//         };
//         VkResult result = vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pUIRenderPass->vkFramebuffers[i]);
//         tryThrowVulkanError(result);
//     }
//     // TODO: Create subpass
    
// }
// void destroyUIRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice)
// {
//     // TODO: Destroy subpass
//     for (size_t i = 0; i < pUIRenderPass->vkFramebufferCount; i++)
//     {
//         vkDestroyFramebuffer(vkDevice, pUIRenderPass->vkFramebuffers[i], NULL);
//     }
//     tickernelFree(pUIRenderPass->vkFramebuffers);

//     destroyVkRenderPass(pUIRenderPass, vkDevice);
// }

// void updateUIRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice, VkImageView *swapchainImageViews, uint32_t width, uint32_t height, VkBuffer globalUniformBuffer)
// {
//     // Recreate framebuffer
//     for (size_t i = 0; i < pUIRenderPass->vkFramebufferCount; i++)
//     {
//         vkDestroyFramebuffer(vkDevice, pUIRenderPass->vkFramebuffers[i], NULL);
//     }
//     for (size_t i = 0; i < pUIRenderPass->vkFramebufferCount; i++)
//     {
//         VkImageView attachments[] = {swapchainImageViews[i]};
//         VkFramebufferCreateInfo vkFramebufferCreateInfo = {
//             .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
//             .pNext = NULL,
//             .flags = 0,
//             .renderPass = pUIRenderPass->vkRenderPass,
//             .attachmentCount = 1,
//             .pAttachments = attachments,
//             .width = width,
//             .height = height,
//             .layers = 1,
//         };
//         VkResult result = vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &pUIRenderPass->vkFramebuffers[i]);
//         tryThrowVulkanError(result);
//     }
    
    
// }

// void recordUIRenderPass(UIRenderPass *pUIRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, VkDevice vkDevice)
// {
// }