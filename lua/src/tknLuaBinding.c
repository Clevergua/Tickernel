#include "tknLuaBinding.h"

static int luaGetSupportedFormat(lua_State *pLuaState)
{
    GfxContext *pGfxContext = (GfxContext *)lua_touserdata(pLuaState, -4);
    lua_len(pLuaState, -3);
    uint32_t candidateCount = (uint32_t)lua_tointeger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    VkFormat *candidates = tknMalloc(sizeof(VkFormat) * candidateCount);
    for (uint32_t i = 0; i < candidateCount; i++)
    {
        lua_rawgeti(pLuaState, -3, i + 1);
        candidates[i] = (VkFormat)lua_tointeger(pLuaState, -1);
        lua_pop(pLuaState, 1);
    }
    VkImageTiling tiling = (VkImageTiling)lua_tointeger(pLuaState, -2);
    VkFormatFeatureFlags features = (VkFormatFeatureFlags)lua_tointeger(pLuaState, -1);
    VkFormat supportedFormat = getSupportedFormat(pGfxContext, candidateCount, candidates, tiling, features);
    tknFree(candidates);
    lua_pushinteger(pLuaState, (lua_Integer)supportedFormat);
    return 1;
}

void bindFunctions(lua_State *pLuaState)
{
    luaL_Reg regs[] = {
        {"getSupportedFormat", luaGetSupportedFormat},
        {NULL, NULL},
    };
    luaL_newlib(pLuaState, regs);
    lua_setglobal(pLuaState, "gfx");
}
