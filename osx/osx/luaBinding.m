#import "LuaBinding.h"
#import "graphic.h"
#import "tickernelLuaBinding.h"

@implementation LuaBinding

- (lua_State *)startLua:(NSString *)assetPath graphicContext:(GraphicContext *)graphicContext {
    // 创建 Lua 状态机
    lua_State *pLuaState = luaL_newstate();
    luaL_openlibs(pLuaState);

    // 设置 Lua 的 package.path
    const char *cAssetPath = [assetPath UTF8String];
    char packagePath[FILENAME_MAX];
    strcpy(packagePath, cAssetPath);
    tickernelCombinePaths(packagePath, FILENAME_MAX, "?.lua;");
    lua_getglobal(pLuaState, "package");
    lua_pushstring(pLuaState, packagePath);
    lua_setfield(pLuaState, -2, "path");
    lua_pop(pLuaState, 1);

    // 加载 main.lua
    char luaMainFilePath[FILENAME_MAX];
    strcpy(luaMainFilePath, cAssetPath);
    tickernelCombinePaths(luaMainFilePath, FILENAME_MAX, "main.lua");
    int luaResult = luaL_dofile(pLuaState, luaMainFilePath);
    if (luaResult != LUA_OK) {
        const char *msg = lua_tostring(pLuaState, -1);
        NSLog(@"Lua error: %s", msg);
        lua_pop(pLuaState, 1);
        return NULL;
    }

    // 注册 Lua 函数
    lua_pushlightuserdata(pLuaState, graphicContext);
    lua_setfield(pLuaState, -2, "pGraphicContext");

    lua_pushstring(pLuaState, cAssetPath);
    lua_setfield(pLuaState, -2, "assetsPath");

    lua_pushstring(pLuaState, tickernelGetPathSeparator());
    lua_setfield(pLuaState, -2, "pathSeparator");

    lua_pushcfunction(pLuaState, luaAddModelToOpaqueGeometrySubpass);
    lua_setfield(pLuaState, -2, "addModelToOpaqueGeometrySubpass");

    lua_pushcfunction(pLuaState, luaRemoveModelFromOpaqueGeometrySubpass);
    lua_setfield(pLuaState, -2, "removeModelFromOpaqueGeometrySubpass");

    lua_pushcfunction(pLuaState, luaUpdateInstancesInOpaqueGeometrySubpass);
    lua_setfield(pLuaState, -2, "updateInstancesInOpaqueGeometrySubpass");

    lua_pushcfunction(pLuaState, luaAddModelToWaterGeometrySubpass);
    lua_setfield(pLuaState, -2, "addModelToWaterGeometrySubpass");

    lua_pushcfunction(pLuaState, luaRemoveModelFromWaterGeometrySubpass);
    lua_setfield(pLuaState, -2, "removeModelFromWaterGeometrySubpass");

    lua_pushcfunction(pLuaState, luaUpdateInstancesInWaterGeometrySubpass);
    lua_setfield(pLuaState, -2, "updateInstancesInWaterGeometrySubpass");

    lua_pushcfunction(pLuaState, updateGlobalUniformBuffer);
    lua_setfield(pLuaState, -2, "updateGlobalUniformBuffer");

    lua_pushcfunction(pLuaState, updateLightsUniformBuffer);
    lua_setfield(pLuaState, -2, "updateLightsUniformBuffer");

    lua_pushcfunction(pLuaState, loadModel);
    lua_setfield(pLuaState, -2, "loadModel");

    // 调用 Lua 的 start 函数
    lua_getglobal(pLuaState, "start");
    luaResult = lua_pcall(pLuaState, 0, 0, 0);
    if (luaResult != LUA_OK) {
        const char *msg = lua_tostring(pLuaState, -1);
        NSLog(@"Lua error: %s", msg);
        lua_pop(pLuaState, 1);
        return NULL;
    }

    return pLuaState;
}

- (void)updateLua:(lua_State *)pLuaState keyCodes:(BOOL *)keyCodes keyCodesLength:(uint32_t)keyCodesLength {

    // 更新输入状态
    lua_getglobal(pLuaState, "input");
    for (uint32_t i = 0; i < keyCodesLength; i++) {
        lua_pushinteger(pLuaState, i);
        lua_pushboolean(pLuaState, keyCodes[i]);
        lua_settable(pLuaState, -3);
    }
    lua_pop(pLuaState, 1);

    // 调用 Lua 的 update 函数
    lua_getglobal(pLuaState, "update");
    int luaResult = lua_pcall(pLuaState, 0, 0, 0);
    if (luaResult != LUA_OK) {
        const char *msg = lua_tostring(pLuaState, -1);
        NSLog(@"Lua error: %s", msg);
        lua_pop(pLuaState, 1);
    }
}

- (void)endLua:(lua_State *)pLuaState {
    // 调用 Lua 的 stop 函数
    lua_getglobal(pLuaState, "stop");
    int luaResult = lua_pcall(pLuaState, 0, 0, 0);
    if (luaResult != LUA_OK) {
        const char *msg = lua_tostring(pLuaState, -1);
        NSLog(@"Lua error: %s", msg);
        lua_pop(pLuaState, 1);
    }

    // 关闭 Lua 状态机
    lua_close(pLuaState);
}

@end
