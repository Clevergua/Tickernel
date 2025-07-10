#pragma once
#include "graphicsCore.h"

void createGraphicsContext(int targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent, GraphicsContext *pGraphicsContext);
void updateGraphicsContext(GraphicsContext *pGraphicsContext, VkExtent2D swapchainExtent);
void destroyGraphicsContext(GraphicsContext graphicsContext);