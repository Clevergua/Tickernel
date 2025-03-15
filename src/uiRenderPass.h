#pragma once
#include "uiSubpass.h"

typedef struct UIRenderPassStruct
{
    VkRenderPass vkRenderPass;
    uint32_t vkFramebufferCount;
    VkFramebuffer *vkFramebuffers;
    Subpass uiSubpass;
} UIRenderPass;

void createUIRenderPass(UIRenderPass *pUIRenderPass, const char *shadersPath, VkDevice vkDevice, GraphicImage colorGraphicImage, VkViewport viewport, VkRect2D scissor, uint32_t swapchainCount, VkImageView *swapchainImageViews, VkFormat swapchainVkFormat);
void destroyUIRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice);
void updateUIRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, uint32_t width, uint32_t height, VkImageView *swapchainImageViews);
void recordUIRenderPass(UIRenderPass *pUIRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, VkDevice vkDevice, uint32_t swapchainIndex);
