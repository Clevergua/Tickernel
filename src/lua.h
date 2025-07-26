#pragma once
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/luaconf.h"
#include "lua/lualib.h"
#include "gfx.h"
typedef struct
{
    lua_State *pLuaState;
} LuaContext;

LuaContext *createLuaContextPtr();
void destroyLuaContextPtr(LuaContext *pLuaContext);
void updateLuaGameplay(LuaContext *pLuaContext);
void updateLuaGfx(LuaContext *pLuaContext, GfxContext *pGfxContext);