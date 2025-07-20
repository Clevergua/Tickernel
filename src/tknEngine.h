#pragma once
#include "gfx.h"

typedef struct
{
    uint32_t frameCount;
    GfxContext gfxContext;
} TknEngine;

TknEngine *createTknEnginePtr(uint32_t targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent);
void updateTknEnginePtr(TknEngine *pTknEngine, VkExtent2D swapchainExtent);
void destroyTknEnginePtr(TknEngine *pTknEngine);
