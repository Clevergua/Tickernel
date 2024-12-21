#pragma once
#include <graphicEngineCore.h>

void CreateTransparentLightingSubpass(Subpass *pTransparentLightingSubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t transparentLightingSubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView);
void DestroyTransparentLightingSubpass(Subpass *pTransparentLightingSubpass, VkDevice vkDevice);
void RecreateTransparentLightingSubpassModel(Subpass *pTransparentLightingSubpass, VkDevice vkDevice, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView);