#include "graphic.h"
#include "lauxlib.h"
#include "lualib.h"
#include "tickernelVoxel.h"

int luaAddModelToOpaqueGeometrySubpass(lua_State *pLuaState);
int luaRemoveModelFromOpaqueGeometrySubpass(lua_State *pLuaState);
int luaUpdateInstancesInOpaqueGeometrySubpass(lua_State *pLuaState);

int luaAddModelToWaterGeometrySubpass(lua_State *pLuaState);
int luaRemoveModelFromWaterGeometrySubpass(lua_State *pLuaState);
int luaUpdateInstancesInWaterGeometrySubpass(lua_State *pLuaState);

int luaUpdateGlobalUniformBuffer(lua_State *pLuaState);
int luaUpdateLightsUniformBuffer(lua_State *pLuaState);

int luaLoadModel(lua_State *pLuaState);

// void startLua(const char *assetPath, GraphicContext *pGraphicContext)
// {
//     // New lua state
//     lua_State *pLuaState = luaL_newstate();
//     luaL_openlibs(pLuaState);

//     // Set package path
//     char packagePath[FILENAME_MAX];
//     strcpy(packagePath, assetPath);
//     tickernelCombinePaths(packagePath, FILENAME_MAX, "?.lua;");
//     lua_getglobal(pLuaState, "package");
//     lua_pushstring(pLuaState, packagePath);
//     lua_setfield(pLuaState, -2, "path");
//     lua_pop(pLuaState, 1);

//     // Do file main.lua
//     char luaMainFilePath[FILENAME_MAX];
//     strcpy(luaMainFilePath, assetPath);
//     tickernelCombinePaths(luaMainFilePath, FILENAME_MAX, "main.lua");
//     // Put engine state on the stack
//     int luaResult = luaL_dofile(pLuaState, luaMainFilePath);
//     tryThrowLuaError(pLuaState, luaResult);

//     lua_pushlightuserdata(pLuaState, pGraphicContext);
//     lua_setfield(pLuaState, -2, "pGraphicContext");

//     lua_pushstring(pLuaState, assetPath);
//     lua_setfield(pLuaState, -2, "assetsPath");

//     lua_pushstring(pLuaState, tickernelGetPathSeparator());
//     lua_setfield(pLuaState, -2, "pathSeparator");

//     lua_pushcfunction(pLuaState, luaAddModelToOpaqueGeometrySubpass);
//     lua_setfield(pLuaState, -2, "addModelToOpaqueGeometrySubpass");

//     lua_pushcfunction(pLuaState, luaRemoveModelFromOpaqueGeometrySubpass);
//     lua_setfield(pLuaState, -2, "removeModelFromOpaqueGeometrySubpass");

//     lua_pushcfunction(pLuaState, luaUpdateInstancesInOpaqueGeometrySubpass);
//     lua_setfield(pLuaState, -2, "updateInstancesInOpaqueGeometrySubpass");

//     lua_pushcfunction(pLuaState, luaAddModelToWaterGeometrySubpass);
//     lua_setfield(pLuaState, -2, "addModelToWaterGeometrySubpass");

//     lua_pushcfunction(pLuaState, luaRemoveModelFromWaterGeometrySubpass);
//     lua_setfield(pLuaState, -2, "removeModelFromWaterGeometrySubpass");

//     lua_pushcfunction(pLuaState, luaUpdateInstancesInWaterGeometrySubpass);
//     lua_setfield(pLuaState, -2, "updateInstancesInWaterGeometrySubpass");

//     lua_pushcfunction(pLuaState, updateGlobalUniformBuffer);
//     lua_setfield(pLuaState, -2, "updateGlobalUniformBuffer");

//     lua_pushcfunction(pLuaState, updateLightsUniformBuffer);
//     lua_setfield(pLuaState, -2, "updateLightsUniformBuffer");

//     lua_pushcfunction(pLuaState, loadModel);
//     lua_setfield(pLuaState, -2, "loadModel");

//     // Call start
//     int startFunctionType = lua_getfield(pLuaState, -1, "start");
//     assertLuaType(startFunctionType, LUA_TFUNCTION);
//     luaResult = lua_pcall(pLuaState, 0, 0, 0);
//     tryThrowLuaError(pLuaState, luaResult);
// }

// void updateLua(lua_State *pLuaState, bool *keyCodes, uint32_t keyCodesLength)
// {
//     // Update keyCodes
//     lua_getfield(pLuaState, -1, "input");
//     for (uint32_t i = 0; i < keyCodesLength; i++)
//     {
//         lua_pushnumber(pLuaState, i);
//         lua_pushboolean(pLuaState, keyCodes[i]);
//         lua_settable(pLuaState, -3);
//     }
//     lua_pop(pLuaState, 1);

//     // Call Update
//     int startFunctionType = lua_getfield(pLuaState, -1, "update");
//     assertLuaType(startFunctionType, LUA_TFUNCTION);

//     int luaResult = lua_pcall(pLuaState, 0, 0, 0);
//     tryThrowLuaError(pLuaState, luaResult);
// }

// void endLua(lua_State *pLuaState)
// {
//     // Call End
//     int startFunctionType = lua_getfield(pLuaState, -1, "stop");
//     assertLuaType(startFunctionType, LUA_TFUNCTION);
//     int luaResult = lua_pcall(pLuaState, 0, 0, 0);
//     tryThrowLuaError(pLuaState, luaResult);

//     //  Pop engine state off the stack
//     lua_pop(pLuaState, 1);
//     lua_close(pLuaState);
// }
