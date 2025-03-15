#pragma once
#include "graphicCore.h"

void createUISubpass(Subpass *pUISubpass, const char *shadersPath, VkRenderPass vkRenderPass, VkDevice vkDevice, VkViewport viewport, VkRect2D scissor);
void destroyUISubpass(Subpass *pUISubpass, VkDevice vkDevice);
void recreateUISubpassModel(Subpass *pUISubpass, VkDevice vkDevice); 