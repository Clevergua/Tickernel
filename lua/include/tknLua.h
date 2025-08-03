#pragma once
#include "lauxlib.h"
#include "tkn.h"
typedef struct TknContext TknContext;

typedef struct
{
    const char *name;
    uint32_t luaRegCount;
    luaL_Reg *luaRegs;
} LuaLibrary;

TknContext *createTknContextPtr(const char *luaPath, uint32_t luaLibraryCount, LuaLibrary *luaLibraries, int targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent);
void destroyTknContextPtr(TknContext *pTknContext);
void updateTknContextGameplay(TknContext *pTknContext);
void updateTknContextGfx(TknContext *pTknContext, VkExtent2D swapchainExtent);
