#pragma once
#include "gfxPipeline.h"

GfxContext createGfxContext(int targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent);
void updateGfxContext(GfxContext *pGfxContext, VkExtent2D swapchainExtent);
void destroyGfxContext(GfxContext gfxContext);