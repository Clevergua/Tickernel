#import "LuaBinding.h"

@implementation LuaBinding
- (int)luaLoadAudio:(lua_State *)pLuaState {
    const char *identifier = luaL_checkstring(pLuaState, 1);
    const char *fileType = luaL_checkstring(pLuaState, 2);
    [self.pAudioBinding loadAudio:[NSString stringWithUTF8String:identifier] fileType:[NSString stringWithUTF8String:fileType]];
    return 0;
}

- (int)luaPlayAudio:(lua_State *)pLuaState {
    const char *identifier = luaL_checkstring(pLuaState, 1);
    [self.pAudioBinding playAudio:[NSString stringWithUTF8String:identifier]];
    return 0;
}

- (int)luaPauseAudio:(lua_State *)pLuaState {
    const char *identifier = luaL_checkstring(pLuaState, 1);
    [self.pAudioBinding pauseAudio:[NSString stringWithUTF8String:identifier]];
    return 0;
}

- (int)luaStopAudio:(lua_State *)pLuaState {
    const char *identifier = luaL_checkstring(pLuaState, 1);
    [self.pAudioBinding stopAudio:[NSString stringWithUTF8String:identifier]];
    return 0;
}

- (int)luaUnloadAudio:(lua_State *)pLuaState {
    const char *identifier = luaL_checkstring(pLuaState, 1);
    [self.pAudioBinding unloadAudio:[NSString stringWithUTF8String:identifier]];
    return 0;
}

- (int)luaSet3DPositionForAudio:(lua_State *)pLuaState {
    const char *identifier = luaL_checkstring(pLuaState, 1);
    float x = luaL_checknumber(pLuaState, 2);
    float y = luaL_checknumber(pLuaState, 3);
    float z = luaL_checknumber(pLuaState, 4);
    [self.pAudioBinding set3DPositionForAudio:[NSString stringWithUTF8String:identifier] X:x Y:y Z:z];
    return 0;
}

- (void)setupLua:(NSString *)assetPath graphicContext:(GraphicContext *)pGraphicContext {
    self.pLuaState = luaL_newstate();
    lua_State *pLuaState = self.pLuaState;
    luaL_openlibs(pLuaState);
    
    const char *cAssetPath = [assetPath UTF8String];
    char packagePath[FILENAME_MAX];
    strcpy(packagePath, cAssetPath);
    tickernelCombinePaths(packagePath, FILENAME_MAX, "?.lua;");
    lua_getglobal(pLuaState, "package");
    lua_pushstring(pLuaState, packagePath);
    lua_setfield(pLuaState, -2, "path");
    lua_pop(pLuaState, 1);
    
    char luaMainFilePath[FILENAME_MAX];
    strcpy(luaMainFilePath, cAssetPath);
    tickernelCombinePaths(luaMainFilePath, FILENAME_MAX, "main.lua");
    int luaResult = luaL_dofile(pLuaState, luaMainFilePath);
    if (luaResult != LUA_OK) {
        const char *msg = lua_tostring(pLuaState, -1);
        NSLog(@"Lua error: %s", msg);
        lua_pop(pLuaState, 1);
    }
    
    lua_pushlightuserdata(pLuaState, pGraphicContext);
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
    
    lua_pushcfunction(pLuaState, luaUpdateGlobalUniformBuffer);
    lua_setfield(pLuaState, -2, "updateGlobalUniformBuffer");
    
    lua_pushcfunction(pLuaState, luaUpdateLightsUniformBuffer);
    lua_setfield(pLuaState, -2, "updateLightsUniformBuffer");
    
    lua_pushcfunction(pLuaState, luaLoadModel);
    lua_setfield(pLuaState, -2, "loadModel");
    
//    self.pAudioBinding = [[AudioBinding alloc] init];
//    lua_pushcfunction(pLuaState, self.pAudioBinding loadA);
//    lua_setfield(pLuaState, -2, "loadModel");

//    lua_pushcfunction(pLuaState, luaLoadModel);
//    lua_setfield(pLuaState, -2, "loadModel");

//    lua_pushcfunction(pLuaState, luaLoadModel);
//    lua_setfield(pLuaState, -2, "loadModel");

//    lua_pushcfunction(pLuaState, luaLoadModel);
//    lua_setfield(pLuaState, -2, "loadModel");
    
    lua_getfield(pLuaState, -1, "start");
    luaResult = lua_pcall(pLuaState, 0, 0, 0);
    if (luaResult != LUA_OK) {
        const char *msg = lua_tostring(pLuaState, -1);
        NSLog(@"Lua error: %s", msg);
        lua_pop(pLuaState, 1);
    }
}

- (void)updateLua:(BOOL *)keyCodes keyCodesLength:(uint32_t)keyCodesLength {
    lua_State* pLuaState = self.pLuaState;
//    uint32_t i = lua_gettop(pLuaState);
    lua_getfield(pLuaState, -1, "input");
    for (uint32_t i = 0; i < keyCodesLength; i++) {
        lua_pushinteger(pLuaState, i);
        lua_pushboolean(pLuaState, keyCodes[i]);
        lua_settable(pLuaState, -3);
    }
    lua_pop(pLuaState, 1);
    
    lua_getfield(pLuaState, -1,"update");
    int luaResult = lua_pcall(pLuaState, 0, 0, 0);
    if (luaResult != LUA_OK) {
        const char *msg = lua_tostring(pLuaState, -1);
        NSLog(@"Lua error: %s", msg);
        lua_pop(pLuaState, 1);
    }
}

- (void)teardownLua {
    lua_State * pLuaState = self.pLuaState;
    lua_getfield(pLuaState, -1,"stop");
    int luaResult = lua_pcall(pLuaState, 0, 0, 0);
    if (luaResult != LUA_OK) {
        const char *msg = lua_tostring(pLuaState, -1);
        NSLog(@"Lua error: %s", msg);
        lua_pop(pLuaState, 1);
    }
    
    lua_close(pLuaState);
}

@end
