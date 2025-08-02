#pragma once
#include "lua.h"
#include "lauxlib.h"
#include "luaconf.h"
#include "lualib.h"
#include "tickernel.h"
typedef struct
{
    lua_State *pLuaState;
} LuaContext;

typedef struct
{
    const char *name;
    uint32_t luaRegCount;
    luaL_Reg *luaRegs;
} LuaLibrary;

LuaContext *createLuaContextPtr(const char *luaPath, uint32_t luaLibraryCount, LuaLibrary *luaLibraries);
void destroyLuaContextPtr(LuaContext *pLuaContext);
void updateLuaGameplay(LuaContext *pLuaContext);
void updateLuaGfx(LuaContext *pLuaContext, GfxContext *pGfxContext);
