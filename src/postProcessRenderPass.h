#pragma once
#include "postProcessSubpass.h"

typedef struct PostProcessRenderPassStruct
{
    VkRenderPass vkRenderPass;
    uint32_t vkFramebufferCount;
    VkFramebuffer *vkFramebuffers;
    Subpass postProcessSubpass;
} PostProcessRenderPass;

void createPostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass,const char *shadersPath, VkDevice vkDevice, GraphicImage colorGraphicImage, VkViewport viewport, VkRect2D scissor, uint32_t swapchainCount, VkImageView *swapchainImageViews, VkFormat swapchainVkFormat);
void destroyPostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkDevice vkDevice);
void updatePostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, uint32_t width, uint32_t height, VkImageView *swapchainImageViews);
void recordPostProcessRenderPass(PostProcessRenderPass *pPostProcessRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, VkDevice vkDevice, uint32_t swapchainIndex);
