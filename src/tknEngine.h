#pragma once
#include "gfx.h"
#include "lua.h"

typedef struct
{
    GfxContext *pGfxContext;
    LuaContext *pLuaContext;
} TknEngine;

TknEngine *createTknEnginePtr(int targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent);
void destroyTknEnginePtr(TknEngine *pTknEngine);
void updateTknEnginePtr(TknEngine *pTknEngine, VkExtent2D swapchainExtent);