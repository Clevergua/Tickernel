
#include "tknLuaBinding.h"

struct TknContext
{
    lua_State *pLuaState;
    GfxContext *pGfxContext;
};


static void assertLuaResult(lua_State *pLuaState, int result)
{
    if (LUA_OK == result)
    {
        // nothing
    }
    else
    {
        printLuaStack(pLuaState);
        const char *err = lua_tostring(pLuaState, -1);
        tknError("Lua error: %s (result: %d)", err, result);
    }
}

TknContext *createTknContextPtr(const char *luaPath, uint32_t luaLibraryCount, LuaLibrary *luaLibraries, int targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent)
{
    TknContext *pTknContext = tknMalloc(sizeof(TknContext));
    GfxContext *pGfxContext = createGfxContextPtr(targetSwapchainImageCount, targetVkSurfaceFormat, targetVkPresentMode, vkInstance, vkSurface, swapchainExtent);

    lua_State *pLuaState = luaL_newstate();
    printf("DEBUG: luaL_newstate() returned %p\n", (void *)pLuaState);

    if (pLuaState == NULL)
    {
        printf("ERROR: luaL_newstate() failed!\n");
        return NULL;
    }

    printf("DEBUG: About to call luaL_openlibs with %p\n", (void *)pLuaState);
    luaL_openlibs(pLuaState);
    printf("DEBUG: luaL_openlibs completed\n");

    char packagePath[FILENAME_MAX];
    snprintf(packagePath, FILENAME_MAX, "%s/?.lua", luaPath);
    lua_getglobal(pLuaState, "package");
    lua_pushstring(pLuaState, packagePath);
    lua_setfield(pLuaState, -2, "path");
    lua_pop(pLuaState, 1);

    bindFunctions(pLuaState);
    for (uint32_t luaLibraryIndex = 0; luaLibraryIndex < luaLibraryCount; luaLibraryIndex++)
    {
        LuaLibrary luaLibrary = luaLibraries[luaLibraryIndex];
        lua_createtable(pLuaState, 0, luaLibrary.luaRegCount - 1);
        luaL_setfuncs(pLuaState, luaLibrary.luaRegs, 0);
        lua_setglobal(pLuaState, luaLibrary.name);
    }
    char tknEngineLuaPath[FILENAME_MAX];
    snprintf(tknEngineLuaPath, FILENAME_MAX, "%s/tknEngine.lua", luaPath);
    int result = luaL_dofile(pLuaState, tknEngineLuaPath);
    assertLuaResult(pLuaState, result);

    lua_getfield(pLuaState, -1, "start");
    lua_pushlightuserdata(pLuaState, pGfxContext);
    assertLuaResult(pLuaState, lua_pcall(pLuaState, 1, 0, 0));
    lua_pop(pLuaState, 1);

    TknContext TknContext = {
        .pGfxContext = pGfxContext,
        .pLuaState = pLuaState,
    };
    *pTknContext = TknContext;
    printLuaStack(pLuaState);
    return pTknContext;
}

void destroyTknContextPtr(TknContext *pTknContext)
{
    lua_State *pLuaState = pTknContext->pLuaState;
    lua_getglobal(pLuaState, "tknEngine");
    lua_getfield(pLuaState, -1, "stop");
    lua_pushlightuserdata(pLuaState, pTknContext->pGfxContext);
    assertLuaResult(pLuaState, lua_pcall(pLuaState, 1, 0, 0));
    lua_pop(pLuaState, 1);
    lua_close(pTknContext->pLuaState);
    
    GfxContext* pGfxContext = pTknContext->pGfxContext;
    destroyGfxContextPtr(pGfxContext);

    tknFree(pTknContext);
}

void updateTknContext(TknContext *pTknContext, VkExtent2D swapchainExtent)
{
    lua_State *pLuaState = pTknContext->pLuaState;
    lua_getglobal(pLuaState, "tknEngine");
    lua_getfield(pLuaState, -1, "updateGameplay");
    assertLuaResult(pLuaState, lua_pcall(pLuaState, 0, 0, 0));

    GfxContext *pGfxContext = pTknContext->pGfxContext;

    waitGfxContextPtr(pGfxContext);

    lua_getfield(pLuaState, -1, "updateGfx");
    lua_pushlightuserdata(pLuaState, pGfxContext);
    assertLuaResult(pLuaState, lua_pcall(pLuaState, 1, 0, 0));

    updateGfxContextPtr(pGfxContext, swapchainExtent);
    lua_pop(pLuaState, 1);
}
