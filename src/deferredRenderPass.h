#pragma once
#include <opaqueGeometrySubpass.h>
#include <opaqueLightingSubpass.h>

typedef struct DeferredRenderPassStruct
{
    char *shadersPath;

    VkRenderPass vkRenderPass;
    uint32_t vkFramebufferCount;
    VkFramebuffer *vkFramebuffers;

    Subpass opaqueGeometrySubpass;
    Subpass opaqueLightingSubpass;
} DeferredRenderPass;

void CreateDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkDevice vkDevice, VkFormat swapchainVkFormat, GraphicImage depthGraphicImage, GraphicImage albedoGraphicImage, GraphicImage normalGraphicImage, uint32_t vkFramebufferCount, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer);
void DestroyDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkDevice vkDevice);
void RecordDeferredRenderPass(DeferredRenderPass *pDeferredRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, uint32_t frameIndex, VkImageView *swapchainVkImageViews, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView, VkDevice vkDevice);