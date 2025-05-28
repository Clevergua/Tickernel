#pragma once
#include "geometrySubpass.h"
#include "lightingSubpass.h"
#include "postProcessSubpass.h"
typedef struct DeferredRenderPassStruct
{
    VkRenderPass vkRenderPass;
    uint32_t vkFramebufferCount;
    VkFramebuffer *vkFramebuffers;

    Subpass geometrySubpass;
    Subpass lightingSubpass;
    Subpass postProcessSubpass;
} DeferredRenderPass;

void createDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, const char *shadersPath, VkDevice vkDevice, GraphicImage colorGraphicImage, GraphicImage depthGraphicImage, GraphicImage albedoGraphicImage, GraphicImage normalGraphicImage, uint32_t swapchainCount, VkImageView *swapchainImageViews, VkFormat swapchainVkFormat, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer);
void destroyDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkDevice vkDevice);
void updateDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkDevice vkDevice, GraphicImage colorGraphicImage, GraphicImage depthGraphicImage, GraphicImage albedoGraphicImage, GraphicImage normalGraphicImage, VkImageView *swapchainImageViews, uint32_t width, uint32_t height, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer);
void recordDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, VkDevice vkDevice, uint32_t swapchainIndex);
