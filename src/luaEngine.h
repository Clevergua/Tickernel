#pragma once
#include <graphicEngine.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

typedef struct LuaEngineStruct
{
    lua_State *pLuaState;
    char *luaAssetsPath;
    GraphicEngine *pGraphicEngine;
} LuaEngine;

void StartLua(LuaEngine *pLuaEngine);
void UpdateLua(LuaEngine *pLuaEngine);
void EndLua(LuaEngine *pLuaEngine);
