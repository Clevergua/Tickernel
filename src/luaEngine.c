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
    strcpy_s(pLuaEngine->luaAssetsPath, FILENAME_MAX, assetPath);
    TKNCombinePaths(pLuaEngine->luaAssetsPath, FILENAME_MAX, "lua", NULL);

    // New lua state
    pLuaEngine->pLuaState = luaL_newstate();
    lua_State *pLuaState = pLuaEngine->pLuaState;
    luaL_openlibs(pLuaState);

    // Set package path
    char packagePath[FILENAME_MAX];
    strcpy_s(packagePath, FILENAME_MAX, pLuaEngine->luaAssetsPath);
    TKNCombinePaths(packagePath, FILENAME_MAX, "?.lua;", NULL);
    lua_getglobal(pLuaState, "package");
    lua_pushstring(pLuaState, packagePath);
    lua_setfield(pLuaState, -2, "path");
    top = lua_gettop(pLuaState);
    lua_pop(pLuaState, 1);

    // Call start function
    char luaMainFilePath[FILENAME_MAX];
    strcpy_s(luaMainFilePath, FILENAME_MAX, pLuaEngine->luaAssetsPath);
    TKNCombinePaths(luaMainFilePath, FILENAME_MAX, "main.lua", NULL);
    int luaResult = luaL_dofile(pLuaState, luaMainFilePath);
    TryThrowLuaError(luaResult);

    // Call start
    top = lua_gettop(pLuaState);
    int startFunctionType = lua_getfield(pLuaState, -1, "Start");
    AssertLuaType(startFunctionType, LUA_TFUNCTION);
    top = lua_gettop(pLuaState);
    luaResult = lua_pcall(pLuaState, 0, 0, 0);
    TryThrowLuaError(luaResult);
    top = lua_gettop(pLuaState);
    printf("After start lua stack,Keep engine state on stack.\nStack count:%d \n", lua_gettop(pLuaState));
}

void UpdateLua(LuaEngine *pLuaEngine)
{
    lua_State *pLuaState = pLuaEngine->pLuaState;
    top = lua_gettop(pLuaState);
    int startFunctionType = lua_getfield(pLuaState, -1, "Update");
    AssertLuaType(startFunctionType, LUA_TFUNCTION);
    top = lua_gettop(pLuaState);
    int luaResult = lua_pcall(pLuaState, 0, 0, 0);
    TryThrowLuaError(luaResult);
    top = lua_gettop(pLuaState);
}

void EndLua(LuaEngine *pLuaEngine)
{
    lua_State *pLuaState = pLuaEngine->pLuaState;
    top = lua_gettop(pLuaState);
    int startFunctionType = lua_getfield(pLuaState, -1, "End");
    AssertLuaType(startFunctionType, LUA_TFUNCTION);
    top = lua_gettop(pLuaState);
    int luaResult = lua_pcall(pLuaState, 0, 0, 0);
    TryThrowLuaError(luaResult);
    top = lua_gettop(pLuaState);
    lua_close(pLuaEngine->pLuaState);
    TKNFree(pLuaEngine->luaAssetsPath);
}
