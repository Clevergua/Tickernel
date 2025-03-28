#pragma once
#include "graphicCore.h"

void createPostProcessSubpass(Subpass *pPostProcessSubpass, const char *shadersPath, VkRenderPass vkRenderPass, uint32_t postProcessSubpassIndex, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor, VkImageView colorVkImageView);
void destroyPostProcessSubpass(Subpass *pPostProcessSubpass, VkDevice vkDevice);
void recreatePostProcessSubpassModel(Subpass *pPostProcessSubpass, VkDevice vkDevice, VkImageView colorVkImageView);
