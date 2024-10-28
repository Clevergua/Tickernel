#pragma once
#include <graphicEngineCore.h>

void CreateLightingSubpass(RenderPass *pDeferredRenderPass, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView);
void DestroyLightingSubpass(RenderPass *pDeferredRenderPass, VkDevice vkDevice);
void RecreateLightingSubpassModel(RenderPass *pDeferredRenderPass, VkDevice vkDevice, VkBuffer globalUniformBuffer, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView);