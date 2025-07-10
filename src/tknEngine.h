#pragma once
#include "graphics.h"

typedef struct
{
    uint32_t frameCount;
    GraphicsContext graphicsContext;
} TknEngine;

void createTknEngine(uint32_t targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent, TknEngine **ppTknEngine);
void updateTknEngine(TknEngine *pTknEngine, VkExtent2D swapchainExtent);
void destroyTknEngine(TknEngine *pTknEngine);
