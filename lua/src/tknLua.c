
#include "tknLuaBinding.h"

struct TknContext
{
    lua_State *pLuaState;
    GfxContext *pGfxContext;
};

static void assertLuaType(lua_State *pLuaState, int index, int expectedType)
{
    if (lua_type(pLuaState, index) != expectedType) {
        lua_error(pLuaState);
    }
}

static void assertLuaArgCount(lua_State *pLuaState, int expected)
{
    int actual = lua_gettop(pLuaState);
    if (actual != expected) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Expected %d arguments, got %d", expected, actual);
        lua_pushstring(pLuaState, msg);
        lua_error(pLuaState);
    }
}

static int luaGetSupportedFormat(lua_State *pLuaState)
{
    int argCount = lua_gettop(pLuaState);
    assertLuaArgCount(pLuaState, 4);
    assertLuaType(pLuaState, -4, LUA_TLIGHTUSERDATA);
    assertLuaType(pLuaState, -3, LUA_TTABLE);
    assertLuaType(pLuaState, -2, LUA_TNUMBER);
    assertLuaType(pLuaState, -1, LUA_TNUMBER);
    
    GfxContext *pGfxContext = (GfxContext *)lua_touserdata(pLuaState, -4);
    
    lua_len(pLuaState, -3);
    uint32_t candidateCount = (uint32_t)lua_tointeger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    
    if (candidateCount == 0) {
        lua_pushstring(pLuaState, "Candidates table cannot be empty");
        lua_error(pLuaState);
        return 0;
    }
    
    VkFormat *candidates = tknMalloc(sizeof(VkFormat) * candidateCount);
    
    for (uint32_t i = 0; i < candidateCount; i++) {
        lua_rawgeti(pLuaState, -3, i + 1);
        if (!lua_isinteger(pLuaState, -1)) {
            tknFree(candidates);
            lua_pushstring(pLuaState, "All candidates must be integers (VkFormat values)");
            lua_error(pLuaState);
            return 0;
        }
        candidates[i] = (VkFormat)lua_tointeger(pLuaState, -1);
        lua_pop(pLuaState, 1);
    }
    
    VkImageTiling tiling = (VkImageTiling)lua_tointeger(pLuaState, -2);
    VkFormatFeatureFlags features = (VkFormatFeatureFlags)lua_tointeger(pLuaState, -1);
    
    VkFormat supportedFormat = getSupportedFormat(pGfxContext, candidateCount, candidates, tiling, features);
    
    tknFree(candidates);
    
    lua_pushinteger(pLuaState, (lua_Integer)supportedFormat);
    return 1;
}

static void bindEngineFunctions(lua_State *pLuaState)
{
    luaL_Reg regs[] = {
        {"getSupportedFormat", luaGetSupportedFormat},
        {NULL, NULL},
    };
    luaL_newlib(pLuaState, regs);
    lua_setglobal(pLuaState, "gfx");
}

static void printLuaStack(lua_State *pLuaState)
{
    int top = lua_gettop(pLuaState);
    printf("=== Lua Stack ===\n");
    printf("Stack size: %d\n", top);

    if (top == 0)
    {
        printf("Stack is empty\n");
    }
    else
    {
        for (int i = 1; i <= top; i++)
        {
            int type = lua_type(pLuaState, i);
            printf("[%d] (%s): ", i, lua_typename(pLuaState, type));

            switch (type)
            {
            case LUA_TNIL:
                printf("nil");
                break;
            case LUA_TBOOLEAN:
                printf(lua_toboolean(pLuaState, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                printf("%g", lua_tonumber(pLuaState, i));
                break;
            case LUA_TSTRING:
                printf("\"%s\"", lua_tostring(pLuaState, i));
                break;
            case LUA_TTABLE:
                printf("table: %p", lua_topointer(pLuaState, i));
                break;
            case LUA_TFUNCTION:
                printf("function: %p", lua_topointer(pLuaState, i));
                break;
            case LUA_TUSERDATA:
                printf("userdata: %p", lua_topointer(pLuaState, i));
                break;
            case LUA_TLIGHTUSERDATA:
                printf("lightuserdata: %p", lua_touserdata(pLuaState, i));
                break;
            case LUA_TTHREAD:
                printf("thread: %p", lua_topointer(pLuaState, i));
                break;
            default:
                printf("unknown");
                break;
            }
            printf("\n");
        }
    }
    printf("==================\n\n");
}

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

    bindEngineFunctions(pLuaState);
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
    assertLuaResult(pLuaState, lua_pcall(pLuaState, 0, 0, 0));
    lua_pop(pLuaState, 1);
    lua_close(pTknContext->pLuaState);

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
