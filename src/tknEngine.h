#pragma once
#include "graphics.h"

typedef struct
{
    uint32_t frameCount;
    GraphicsContext graphicsContext;
} TknEngine;

void createTknEngine(uint32_t targetSwapchainImageCount, VkPresentModeKHR targetPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent, TknEngine **ppTknEngine);
void updateTknEngine(TknEngine *pTknEngine, VkExtent2D swapchainExtent);
void destroyTknEngine(TknEngine *pTknEngine);
