#import "LuaBinding.h"

static int luaLoadAudio(lua_State *L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "pAudioBinding");
    AudioBinding *pAudioBinding = (__bridge AudioBinding *)(lua_touserdata(L, -1));
    lua_pop(L, 1);
    const char *fileName = luaL_checkstring(L, -1);
    [pAudioBinding loadAudio:[NSString stringWithUTF8String:fileName]];
    return 0;
}
static int luaUnloadAudio(lua_State *L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "pAudioBinding");
    AudioBinding *pAudioBinding = (__bridge AudioBinding *)(lua_touserdata(L, -1));
    lua_pop(L, 1);
    const char *fileName = luaL_checkstring(L, -1);
    [pAudioBinding unloadAudio:[NSString stringWithUTF8String:fileName]];
    return 0;
}
static int luaGetAudioPlayer(lua_State *L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "pAudioBinding");
    AudioBinding *pAudioBinding = (__bridge AudioBinding *)(lua_touserdata(L, -1));
    lua_pop(L, 1);
    const char *fileName = luaL_checkstring(L, -1);
    AVAudioPlayerNode *audioPlayer = [pAudioBinding getAudioPlayer:[NSString stringWithUTF8String:fileName]];
    lua_pushlightuserdata(L, (__bridge void *)audioPlayer);
    return 1;
}
static int luaReleaseAudioPlayer(lua_State *L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "pAudioBinding");
    AudioBinding *pAudioBinding = (__bridge AudioBinding *)(lua_touserdata(L, -1));
    lua_pop(L, 1);
    AVAudioPlayerNode *audioPlayer = (__bridge AVAudioPlayerNode *)(lua_touserdata(L, -1));
    [pAudioBinding releaseAudioPlayer:audioPlayer];
    return 0;
}
static int luaPlayAudio(lua_State *L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "pAudioBinding");
    AudioBinding *pAudioBinding = (__bridge AudioBinding *)(lua_touserdata(L, -1));
    lua_pop(L, 1);
    AVAudioPlayerNode *audioPlayer = (__bridge AVAudioPlayerNode *)(lua_touserdata(L, -1));
    if (audioPlayer) {
        [pAudioBinding playAudio:audioPlayer];
    }
    return 0;
}
static int luaPauseAudio(lua_State *L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "pAudioBinding");
    AudioBinding *pAudioBinding = (__bridge AudioBinding *)(lua_touserdata(L, -1));
    lua_pop(L, 1);
    AVAudioPlayerNode *audioPlayer = (__bridge AVAudioPlayerNode *)(lua_touserdata(L, -1));
    if (audioPlayer) {
        [pAudioBinding pauseAudio:audioPlayer];
    }
    return 0;
}
static int luaStopAudio(lua_State *L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "pAudioBinding");
    AudioBinding *pAudioBinding = (__bridge AudioBinding *)(lua_touserdata(L, -1));
    lua_pop(L, 1);
    AVAudioPlayerNode *audioPlayer = (__bridge AVAudioPlayerNode *)(lua_touserdata(L, -1));
    if (audioPlayer) {
        [pAudioBinding stopAudio:audioPlayer];
    }
    return 0;
}
static int luaSetAudioPosition(lua_State *L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "pAudioBinding");
    AudioBinding *pAudioBinding = (__bridge AudioBinding *)(lua_touserdata(L, -1));
    lua_pop(L, 1);
    AVAudioPlayerNode *audioPlayer = (__bridge AVAudioPlayerNode *)(lua_touserdata(L, -4));
    float x = luaL_checknumber(L, -3);
    float y = luaL_checknumber(L, -2);
    float z = luaL_checknumber(L, -1);
    if (audioPlayer) {
        [pAudioBinding setAudioPosition:audioPlayer X:x Y:y Z:z];
    }
    return 0;
}


@implementation LuaBinding
- (instancetype)init {
    self = [super init];
    if (self) {
        self.pAudioBinding = [[AudioBinding alloc] init];
    }
    return self;
}

- (void)dealloc {
    self.pAudioBinding = nil;
}

- (void)setupLua:(NSString *)assetPath graphicContext:(GraphicContext *)pGraphicContext {
    self.pLuaState = luaL_newstate();
    lua_State *pLuaState = self.pLuaState;
    luaL_openlibs(pLuaState);
    
    const char *cAssetPath = [assetPath UTF8String];
    char packagePath[FILENAME_MAX];
    snprintf(packagePath, FILENAME_MAX, "%s?.lua;", cAssetPath);
    lua_getglobal(pLuaState, "package");
    lua_pushstring(pLuaState, packagePath);
    lua_setfield(pLuaState, -2, "path");
    lua_pop(pLuaState, 1);
    
    char luaMainFilePath[FILENAME_MAX];
    snprintf(luaMainFilePath, FILENAME_MAX, "%s/main.lua", cAssetPath);
    int luaResult = luaL_dofile(pLuaState, luaMainFilePath);
    if (luaResult != LUA_OK) {
        const char *msg = lua_tostring(pLuaState, -1);
        NSLog(@"Lua error: %s", msg);
        lua_pop(pLuaState, 1);
    }
    
    lua_pushlightuserdata(pLuaState, pGraphicContext);
    lua_setfield(pLuaState, LUA_REGISTRYINDEX, "pGraphicContext");
    
    lua_pushlightuserdata(pLuaState, (__bridge void *)self.pAudioBinding);
    lua_setfield(pLuaState, LUA_REGISTRYINDEX, "pAudioBinding");
    
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
    
    lua_pushcfunction(pLuaState, luaLoadAudio);
    lua_setfield(pLuaState, -2, "loadAudio");
    
    lua_pushcfunction(pLuaState, luaUnloadAudio);
    lua_setfield(pLuaState, -2, "unloadAudio");
    
    lua_pushcfunction(pLuaState, luaPlayAudio);
    lua_setfield(pLuaState, -2, "playAudio");
    
    lua_pushcfunction(pLuaState, luaPauseAudio);
    lua_setfield(pLuaState, -2, "pauseAudio");
    
    lua_pushcfunction(pLuaState, luaStopAudio);
    lua_setfield(pLuaState, -2, "stopAudio");
    
    lua_pushcfunction(pLuaState, luaSetAudioPosition);
    lua_setfield(pLuaState, -2, "setAudioPosition");
    
    lua_pushcfunction(pLuaState, luaGetAudioPlayer);
    lua_setfield(pLuaState, -2, "getAudioPlayer");

    lua_pushcfunction(pLuaState, luaReleaseAudioPlayer);
    lua_setfield(pLuaState, -2, "releaseAudioPlayer");
    
    lua_getfield(pLuaState, -1, "start");
    luaResult = lua_pcall(pLuaState, 0, 0, 0);
    if (luaResult != LUA_OK) {
        const char *msg = lua_tostring(pLuaState, -1);
        NSLog(@"Lua error: %s", msg);
        lua_pop(pLuaState, 1);
    }
}

- (void)updateLua:(KeyCodeState *)keyCodeStates keyCodesLength:(uint32_t)keyCodesLength {
    lua_State* pLuaState = self.pLuaState;
    lua_getfield(pLuaState, -1, "input");
    for (uint32_t i = 0; i < keyCodesLength; i++) {
        lua_pushinteger(pLuaState, i);
        lua_pushinteger(pLuaState, keyCodeStates[i]);
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
