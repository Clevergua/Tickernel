#pragma once
#include "uiSubpass.h"

typedef struct {
    VkRenderPass vkRenderPass;
    VkFramebuffer vkFramebuffer;
    Subpass uiSubpass;
} UIRenderPass;

void createUIRenderPass(UIRenderPass *pUIRenderPass, const char *shadersPath, VkDevice vkDevice, GraphicImage colorGraphicImage, GraphicImage depthGraphicImage, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer);
void destroyUIRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice);
void updateUIRenderPass(UIRenderPass *pUIRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, GraphicImage depthGraphicImage, uint32_t width, uint32_t height, VkBuffer globalUniformBuffer);
void recordUIRenderPass(UIRenderPass *pUIRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, VkDevice vkDevice);
