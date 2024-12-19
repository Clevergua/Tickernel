#pragma once
#include <graphicEngineCore.h>

void CreatePostProcessSubpass(Subpass *pPostProcessSubpass, const char *shadersPath, VkRenderPass vkRenderPass, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor, VkImageView colorVkImageView);
void DestroyPostProcessSubpass(Subpass *pPostProcessSubpass, VkDevice vkDevice);
void RecreatePostProcessSubpassModel(Subpass *pPostProcessSubpass, VkDevice vkDevice, VkImageView colorVkImageView);