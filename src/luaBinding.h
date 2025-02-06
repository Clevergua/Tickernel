#pragma once
#include "graphic.h"
#include "luaBinding.h"
#include "lauxlib.h"
#include "lualib.h"

typedef struct
{
    lua_State *pLuaState;
    const char *assetPath;
    GraphicContext *pGraphicContext;
} LuaContext;

LuaContext *startLua(const char *assetPath, GraphicContext *pGraphicContext);
void updateLua(LuaContext *pLuaContext);
void endLua(LuaContext *pLuaContext);
