#pragma once
#include "luaBinding.h"
typedef struct
{
    uint32_t frameCount;
    const char *assetsPath;
    uint32_t targetSwapchainImageCount;
    VkPresentModeKHR targetPresentMode;
    VkInstance vkInstance;
    VkSurfaceKHR vkSurface;
    GraphicContext *pGraphicContext;
    LuaContext *pLuaContext;
    bool* keyCodes;
} TickernelEngine;

TickernelEngine *tickernelStart(const char *assetsPath, uint32_t targetSwapchainImageCount, VkPresentModeKHR targetPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, uint32_t swapchainWidth, uint32_t swapchainHeight);
void tickernelUpdate(TickernelEngine *pTickernelEngine, uint32_t swapchainWidth, uint32_t swapchainHeight, bool *keyCodes);
void tickernelEnd(TickernelEngine *pTickernelEngine);
