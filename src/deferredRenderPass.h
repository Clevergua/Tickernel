#pragma once
#include <geometrySubpass.h>
#include <lightingSubpass.h>

void CreateDeferredRenderPass(RenderPass *pDeferredRenderPass, VkDevice vkDevice, VkFormat swapchainVkFormat, GraphicImage depthGraphicImage, GraphicImage albedoGraphicImage, GraphicImage normalGraphicImage, uint32_t vkFramebufferCount, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer);
void DestroyDeferredRenderPass(RenderPass *pDeferredRenderPass, VkDevice vkDevice);
void RecordDeferredRenderPass(RenderPass *pDeferredRenderPass, VkCommandBuffer vkCommandBuffer, VkViewport viewport, VkRect2D scissor, uint32_t frameIndex, VkImageView *swapchainVkImageViews, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView, VkDevice vkDevice);