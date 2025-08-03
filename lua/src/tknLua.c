#include "tknLua.h"
#include "tkn.h"
#include "lualib.h"

struct TknContext
{
    lua_State *pLuaState;
    GfxContext *pGfxContext;
};

static void bindEngineFunctions(lua_State *pLuaState)
{
    luaL_Reg regs[] = {
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
    if (VK_SUCCESS == result)
    {
        // do nothing
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
    printLuaStack(pLuaState);
    char tknEngineLuaPath[FILENAME_MAX];
    snprintf(tknEngineLuaPath, FILENAME_MAX, "%s/tknEngine.lua", luaPath);
    luaL_dofile(pLuaState, tknEngineLuaPath);
    assertLuaResult(pLuaState, luaL_dofile(pLuaState, tknEngineLuaPath));
    printLuaStack(pLuaState);
    lua_getfield(pLuaState, -1, "start");
    assertLuaResult(pLuaState, lua_pcall(pLuaState, 0, 0, 0));
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
    lua_getfield(pLuaState, -1, "updateGameplay");
    assertLuaResult(pLuaState, lua_pcall(pLuaState, 0, 0, 0));
    lua_pop(pLuaState, 1);

    GfxContext *pGfxContext = pTknContext->pGfxContext;

    waitGfxContextPtr(pGfxContext);

    lua_getfield(pLuaState, -1, "updateGfx");
    lua_pushlightuserdata(pLuaState, pGfxContext);
    assertLuaResult(pLuaState, lua_pcall(pLuaState, 1, 0, 0));
    lua_pop(pLuaState, 1);

    updateGfxContextPtr(pGfxContext, swapchainExtent);
}
