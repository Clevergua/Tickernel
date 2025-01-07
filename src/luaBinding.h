#pragma once
#include <graphic.h>
#include <luaBinding.h>
#include <lauxlib.h>
#include <lualib.h>

typedef struct LuaEngineStruct
{
    lua_State *pLuaState;
    char *shadersPath;
    GraphicContext *pGraphicContext;
} LuaEngine;

void StartLua(LuaEngine *pLuaEngine);
void UpdateLua(LuaEngine *pLuaEngine);
void EndLua(LuaEngine *pLuaEngine);
