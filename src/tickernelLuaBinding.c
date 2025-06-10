#include "tickernelLuaBinding.h"

#define PGRAPHICCONTEXT_NAME "pGraphicContext"

static void assertLuaType(int type, int targetType)
{
    if (type == targetType)
    {
        // continue.
    }
    else
    {
        tickernelError("Lua type error: type:%d ,target:%d!\n", type, targetType);
    }
}

// function engine.findSupportedFormat(vkformats, features, tiling)
//     local vkFormat = 0
//     return vkFormat
// end
int luaFindSupportedFormat(lua_State *pLuaState)
{
    lua_len(pLuaState, 1);
    lua_Integer vkFormatCount = luaL_checkinteger(pLuaState, -1); // vkformats
    lua_pop(pLuaState, 1);
    VkFormat vkFormats[vkFormatCount];
    for (uint32_t i = 0; i < vkFormatCount; i++)
    {
        int formatType = lua_geti(pLuaState, 1, i + 1);
        assertLuaType(formatType, LUA_TNUMBER);
        vkFormats[i] = lua_tointeger(pLuaState, -1);
    }
    VkFormatFeatureFlags features = luaL_checkinteger(pLuaState, 2);
    VkImageTiling tiling = luaL_checkinteger(pLuaState, 3);

    int pGraphicContextType = lua_getfield(pLuaState, LUA_REGISTRYINDEX, PGRAPHICCONTEXT_NAME);
    assertLuaType(pGraphicContextType, LUA_TLIGHTUSERDATA);
    GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 1);

    VkFormat vkFormat = 0;
    findSupportedFormat(pGraphicContext, vkFormats, vkFormatCount, features, tiling, &vkFormat);

    lua_pushinteger(pLuaState, vkFormat); // return vkFormat
    return 1;
}

int luaCreateRenderPass(lua_State *pLuaState)
{

    return 1;
}

int luaDestroyRenderPass(lua_State *pLuaState)
{

    return 0;
}

int luaCreateAttachment(lua_State *pLuaState)
{

    return 1;
}

int luaDestroyAttachment(lua_State *pLuaState)
{

    return 0;
}