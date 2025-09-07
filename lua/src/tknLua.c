#include "tknLuaBinding.h"

struct TknContext
{
    lua_State *pLuaState;
    GfxContext *pGfxContext;
};

static int errorHandler(lua_State *L)
{
    const char *msg = lua_tostring(L, 1);
    if (msg == NULL)
        msg = "unknown error";
    luaL_traceback(L, L, msg, 1);
    return 1;
}

static void assertLuaResult(lua_State *pLuaState, int result)
{
    if (LUA_OK != result)
    {
        const char *fullError = lua_tostring(pLuaState, -1);
        if (fullError == NULL)
            fullError = "unknown error";
        tknError("Lua error: %s (result: %d)", fullError, result);
        lua_pop(pLuaState, 1);
    }
}

TknContext *createTknContextPtr(const char *assetsPath, uint32_t luaLibraryCount, LuaLibrary *luaLibraries, int targetSwapchainImageCount, VkSurfaceFormatKHR targetVkSurfaceFormat, VkPresentModeKHR targetVkPresentMode, VkInstance vkInstance, VkSurfaceKHR vkSurface, VkExtent2D swapchainExtent)
{
    TknContext *pTknContext = tknMalloc(sizeof(TknContext));

    char globalVertSpvPath[FILENAME_MAX];
    char globalFragSpvPath[FILENAME_MAX];
    snprintf(globalVertSpvPath, FILENAME_MAX, "%s/shaders/global.vert.spv", assetsPath);
    snprintf(globalFragSpvPath, FILENAME_MAX, "%s/shaders/global.frag.spv", assetsPath);

    const char *spvPaths[] = {
        globalVertSpvPath,
        globalFragSpvPath,
    };

    GfxContext *pGfxContext = createGfxContextPtr(targetSwapchainImageCount, targetVkSurfaceFormat, targetVkPresentMode, vkInstance, vkSurface, swapchainExtent, TKN_ARRAY_COUNT(spvPaths), spvPaths);

    lua_State *pLuaState = luaL_newstate();
    printf("DEBUG: luaL_newstate() returned %p\n", (void *)pLuaState);

    if (NULL == pLuaState)
    {
        printf("ERROR: luaL_newstate() failed!\n");
        return NULL;
    }

    printf("DEBUG: About to call luaL_openlibs with %p\n", (void *)pLuaState);
    luaL_openlibs(pLuaState);
    printf("DEBUG: luaL_openlibs completed\n");

    char packagePath[FILENAME_MAX];
    snprintf(packagePath, FILENAME_MAX, "%s/lua/?.lua", assetsPath);
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
    snprintf(tknEngineLuaPath, FILENAME_MAX, "%s/lua/tknEngine.lua", assetsPath);
    int result = luaL_dofile(pLuaState, tknEngineLuaPath);
    assertLuaResult(pLuaState, result);

    lua_getfield(pLuaState, -1, "start");
    lua_pushlightuserdata(pLuaState, pGfxContext);
    lua_pushstring(pLuaState, assetsPath);
    lua_pushcfunction(pLuaState, errorHandler);
    lua_insert(pLuaState, -4);
    assertLuaResult(pLuaState, lua_pcall(pLuaState, 2, 0, -4));
    lua_pop(pLuaState, 1);

    TknContext TknContext = {
        .pGfxContext = pGfxContext,
        .pLuaState = pLuaState,
    };
    *pTknContext = TknContext;
    return pTknContext;
}

void destroyTknContextPtr(TknContext *pTknContext)
{
    GfxContext *pGfxContext = pTknContext->pGfxContext;

    waitGfxContextPtr(pGfxContext);
    lua_State *pLuaState = pTknContext->pLuaState;
    lua_getglobal(pLuaState, "tknEngine");
    lua_getfield(pLuaState, -1, "stop");
    lua_pushlightuserdata(pLuaState, pGfxContext);
    lua_pushcfunction(pLuaState, errorHandler);
    lua_insert(pLuaState, -3);
    assertLuaResult(pLuaState, lua_pcall(pLuaState, 1, 0, -3));
    lua_pop(pLuaState, 1);
    lua_close(pTknContext->pLuaState);

    destroyGfxContextPtr(pGfxContext);

    tknFree(pTknContext);
}

void updateTknContext(TknContext *pTknContext, VkExtent2D swapchainExtent)
{
    lua_State *pLuaState = pTknContext->pLuaState;
    lua_pushcfunction(pLuaState, errorHandler);
    lua_getglobal(pLuaState, "tknEngine");
    lua_getfield(pLuaState, -1, "updateGameplay");
    assertLuaResult(pLuaState, lua_pcall(pLuaState, 0, 0, -3));

    GfxContext *pGfxContext = pTknContext->pGfxContext;
    waitGfxContextPtr(pGfxContext);

    lua_getfield(pLuaState, -1, "updateGfx");
    lua_pushlightuserdata(pLuaState, pGfxContext);
    assertLuaResult(pLuaState, lua_pcall(pLuaState, 1, 0, -3));

    updateGfxContextPtr(pGfxContext, swapchainExtent);
    lua_pop(pLuaState, 2);
}
