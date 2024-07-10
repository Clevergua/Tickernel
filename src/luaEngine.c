#include <luaEngine.h>
#include <string.h>
static int top;
static void TryThrowLuaError(int luaResult)
{
    if (LUA_OK == luaResult)
    {
        // continue.
    }
    else
    {
        printf("Lua error code: %d!\n", luaResult);
        abort();
    }
}

static void AssertLuaType(int type, int targetType)
{
    if (type == targetType)
    {
        // continue.
    }
    else
    {
        printf("Lua type error: type:%d ,target:%d!\n", type, targetType);
        abort();
    }
}

void StartLua(LuaEngine *pLuaEngine, const char *assetPath)
{
    // Initialize lua assets path.
    pLuaEngine->luaAssetsPath = TKNMalloc(FILENAME_MAX);
    strcpy(pLuaEngine->luaAssetsPath, assetPath);
    TKNCombinePaths(pLuaEngine->luaAssetsPath, FILENAME_MAX, "lua");

    // New lua state
    pLuaEngine->pLuaState = luaL_newstate();
    lua_State *pLuaState = pLuaEngine->pLuaState;
    luaL_openlibs(pLuaState);

    // Set package path
    char packagePath[FILENAME_MAX];
    strcpy(packagePath, pLuaEngine->luaAssetsPath);
    TKNCombinePaths(packagePath, FILENAME_MAX, "?.lua;");
    lua_getglobal(pLuaState, "package");
    lua_pushstring(pLuaState, packagePath);
    lua_setfield(pLuaState, -2, "path");
    lua_pop(pLuaState, 1);

    // Do file main.lua
    char luaMainFilePath[FILENAME_MAX];
    strcpy(luaMainFilePath, pLuaEngine->luaAssetsPath);
    TKNCombinePaths(luaMainFilePath, FILENAME_MAX, "main.lua");
    // Put engine state on the stack
    int luaResult = luaL_dofile(pLuaState, luaMainFilePath);
    TryThrowLuaError(luaResult);

    // Call start
    int startFunctionType = lua_getfield(pLuaState, -1, "Start");
    AssertLuaType(startFunctionType, LUA_TFUNCTION);
    luaResult = lua_pcall(pLuaState, 0, 0, 0);
    TryThrowLuaError(luaResult);
}

void UpdateLua(LuaEngine *pLuaEngine)
{
    lua_State *pLuaState = pLuaEngine->pLuaState;
    
    int startFunctionType = lua_getfield(pLuaState, -1, "Update");
    AssertLuaType(startFunctionType, LUA_TFUNCTION);
    int luaResult = lua_pcall(pLuaState, 0, 0, 0);
    TryThrowLuaError(luaResult);
}

void EndLua(LuaEngine *pLuaEngine)
{
    // Call End
    lua_State *pLuaState = pLuaEngine->pLuaState;
    int startFunctionType = lua_getfield(pLuaState, -1, "End");
    AssertLuaType(startFunctionType, LUA_TFUNCTION);
    int luaResult = lua_pcall(pLuaState, 0, 0, 0);
    TryThrowLuaError(luaResult);

    //  Pop engine state off the stack
    lua_pop(pLuaState, 1);

    lua_close(pLuaEngine->pLuaState);
    TKNFree(pLuaEngine->luaAssetsPath);
}
