#pragma once
#include "uiSubpass.h"

typedef struct UIRenderPass
{
    VkRenderPass vkRenderPass;
    uint32_t vkFramebufferCount;
    VkFramebuffer *vkFramebuffers;
    Subpass uiSubpass;
} UIRenderPass;

void createUIRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice, VkFormat swapchainVkFormat, uint32_t swapchainCount, VkImageView *swapchainImageViews, VkViewport viewport, VkRect2D scissor);