#pragma once
#include "graphicCore.h"

void createOpaqueLightingSubpass(Subpass *pOpaqueLightingSubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t opaqueLightingSubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView);
void destroyOpaqueLightingSubpass(Subpass *pOpaqueLightingSubpass, VkDevice vkDevice);
void recreateOpaqueLightingSubpassModel(Subpass *pOpaqueLightingSubpass, VkDevice vkDevice, VkBuffer globalUniformBuffer, VkBuffer lightsUniformBuffer, VkImageView depthVkImageView, VkImageView albedoVkImageView, VkImageView normalVkImageView);
