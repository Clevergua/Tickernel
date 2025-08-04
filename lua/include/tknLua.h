#ifndef tkn_lua_h
#define tkn_lua_h

#include "lauxlib.h"
#include "vulkan/vulkan.h"

typedef struct TknContext TknContext;

typedef struct
{
    const char *name;
    uint32_t luaRegCount;
    luaL_Reg *luaRegs;
} LuaLibrary;

TknContext *createTknContextPtr(const char *luaPath, uint32_t luaLibraryCount, LuaLibrary *luaLibraries, int targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent);
void destroyTknContextPtr(TknContext *pTknContext);
void updateTknContext(TknContext *pTknContext, VkExtent2D swapchainExtent);
#endif