#pragma once
#include "gfx.h"
#include "tknLua.h"

typedef struct
{
    GfxContext *pGfxContext;
    LuaContext *pLuaContext;
} TknEngine;

TknEngine *createTknEnginePtr(int targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent, const char *luaPath, uint32_t luaLibraryCount, LuaLibrary *luaLibraries);
void destroyTknEnginePtr(TknEngine *pTknEngine);
void updateTknEnginePtr(TknEngine *pTknEngine, VkExtent2D swapchainExtent);