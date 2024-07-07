#pragma once
#include <tickernelCore.h>
#include <lauxlib.h>
#include <lualib.h>

typedef struct LuaEngineStruct
{
    lua_State *pLuaState;
    char *luaAssetsPath;
} LuaEngine;

void StartLua(LuaEngine *pLuaEngine, const char *assetPath);
void UpdateLua(LuaEngine *pLuaEngine);
void EndLua(LuaEngine *pLuaEngine);
