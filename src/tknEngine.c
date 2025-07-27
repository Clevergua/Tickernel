#include "tknEngine.h"

TknEngine *createTknEnginePtr(int targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent, const char *luaPath, uint32_t luaLibraryCount, LuaLibrary *luaLibraries)
{
    TknEngine *pTknEngine = tknMalloc(sizeof(TknEngine));
    GfxContext *pGfxContext = createGfxContextPtr(targetSwapchainImageCount, targetVkSurfaceFormat, targetVkPresentMode, vkInstance, vkSurface, swapchainExtent);
    LuaContext *pLuaContext = createLuaContextPtr(luaPath, luaLibraryCount, luaLibraries);
    TknEngine tknEngine = {
        .pGfxContext = pGfxContext,
        .pLuaContext = pLuaContext,
    };
    return pTknEngine;
}
void destroyTknEnginePtr(TknEngine *pTknEngine)
{
    destroyLuaContextPtr(pTknEngine->pLuaContext);
    destroyGfxContextPtr(pTknEngine->pGfxContext);
    tknFree(pTknEngine);
}

void updateTknEnginePtr(TknEngine *pTknEngine, VkExtent2D swapchainExtent)
{
    LuaContext *pLuaContext = pTknEngine->pLuaContext;
    GfxContext *pGfxContext = pTknEngine->pGfxContext;
    updateLuaGameplay(pLuaContext);
    // Wait for gpu
    ASSERT_VK_SUCCESS(vkWaitForFences(pGfxContext->vkDevice, 1, &pGfxContext->renderFinishedFence, VK_TRUE, UINT64_MAX));
    updateLuaGfx(pLuaContext, pGfxContext);
    updateGfxContextPtr(pGfxContext, swapchainExtent);
}