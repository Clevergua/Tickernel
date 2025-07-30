#include "tknLua.h"

static void bindEngineFunctions(lua_State *pLuaState)
{
    luaL_Reg regs[] = {
        {NULL, NULL},
    };
    luaL_newlib(pLuaState, regs);
    lua_setglobal(pLuaState, "gfx");
}

LuaContext *createLuaContextPtr(const char *luaPath, uint32_t luaLibraryCount, LuaLibrary *luaLibraries)
{
    LuaContext *pLuaContext = tknMalloc(sizeof(LuaContext));

    lua_State *pLuaState = luaL_newstate();
    luaL_openlibs(pLuaState);

    char packagePath[FILENAME_MAX];
    snprintf(packagePath, FILENAME_MAX, "%s/?.lua;", luaPath);
    luaL_openlibs(pLuaContext->pLuaState);
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
    snprintf(tknEngineLuaPath, FILENAME_MAX, "%s/tknEngine.lua;", luaPath);
    luaL_execresult(pLuaState, luaL_dofile(pLuaState, tknEngineLuaPath));
    lua_getfield(pLuaState, -1, "start");
    luaL_execresult(pLuaState, lua_pcall(pLuaState, 0, 0, 0));
    lua_pop(pLuaState, 1);

    LuaContext luaContext = {
        .pLuaState = pLuaState,
    };
    *pLuaContext = luaContext;
    return pLuaContext;
}

void destroyLuaContextPtr(LuaContext *pLuaContext)
{
    lua_State *pLuaState = pLuaContext->pLuaState;
    lua_getglobal(pLuaState, "tknEngine");

    lua_getfield(pLuaState, -1, "stop");
    luaL_execresult(pLuaState, lua_pcall(pLuaState, 0, 0, 0));
    lua_pop(pLuaState, 1);

    lua_close(pLuaContext->pLuaState);
    tknFree(pLuaContext);
}

void updateLuaGameplay(LuaContext *pLuaContext)
{
    lua_State *pLuaState = pLuaContext->pLuaState;
    lua_getfield(pLuaState, -1, "updateGameplay");
    luaL_execresult(pLuaState, lua_pcall(pLuaState, 0, 0, 0));
    lua_pop(pLuaState, 1);
}

void updateLuaGfx(LuaContext *pLuaContext, GfxContext *pGfxContext)
{
    lua_State *pLuaState = pLuaContext->pLuaState;
    lua_getfield(pLuaState, -1, "updateGfx");
    lua_pushlightuserdata(pLuaState, pGfxContext);
    luaL_execresult(pLuaState, lua_pcall(pLuaState, 1, 0, 0));
    lua_pop(pLuaState, 1);
}