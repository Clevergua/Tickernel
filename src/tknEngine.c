#include "tknEngine.h"

TknEngine *createTknEnginePtr(int targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent, const char *assetsPath, uint32_t luaLibraryCount, LuaLibrary *luaLibraries)
{
    char luaPath[FILENAME_MAX];
    snprintf(luaPath, FILENAME_MAX, "%s/%s", assetsPath, "/lua");
    GfxContext *pGfxContext = createGfxContextPtr(targetSwapchainImageCount, targetVkSurfaceFormat, targetVkPresentMode, vkInstance, vkSurface, swapchainExtent);
    LuaContext *pLuaContext = createLuaContextPtr(luaPath, luaLibraryCount, luaLibraries);
    TknEngine tknEngine = {
        .pGfxContext = pGfxContext,
        .pLuaContext = pLuaContext,
    };

    TknEngine *pTknEngine = tknMalloc(sizeof(TknEngine));
    *pTknEngine = tknEngine;
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
    assertVkResult(vkWaitForFences(pGfxContext->vkDevice, 1, &pGfxContext->renderFinishedFence, VK_TRUE, UINT64_MAX));
    updateLuaGfx(pLuaContext, pGfxContext);
    updateGfxContextPtr(pGfxContext, swapchainExtent);
}
