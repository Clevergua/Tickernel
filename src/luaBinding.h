#pragma once
#include <graphic.h>
#include <luaBinding.h>
#include <lauxlib.h>
#include <lualib.h>

typedef struct
{
    lua_State *pLuaState;
    char *assetPath;
    GraphicContext *pGraphicContext;
} LuaContext;

void StartLua(LuaContext *pLuaContext);
void UpdateLua(LuaContext *pLuaContext);
void EndLua(LuaContext *pLuaContext);
