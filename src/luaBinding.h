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

LuaContext *StartLua(const char *assetPath, GraphicContext *pGraphicContext);
void UpdateLua(LuaContext *pLuaContext);
void EndLua(LuaContext *pLuaContext);
