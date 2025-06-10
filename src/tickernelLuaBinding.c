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

static GraphicContext *getGraphicContextPointer(lua_State *pLuaState)
{
    int pGraphicContextType = lua_getfield(pLuaState, LUA_REGISTRYINDEX, PGRAPHICCONTEXT_NAME);
    assertLuaType(pGraphicContextType, LUA_TLIGHTUSERDATA);
    GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 1);
    return pGraphicContext;
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

    VkFormat vkFormat;
    findSupportedFormat(getGraphicContextPointer(pLuaState), vkFormats, vkFormatCount, features, tiling, &vkFormat);

    lua_pushinteger(pLuaState, vkFormat); // return vkFormat
    return 1;
}

// function engine.createDynamicAttachment(vkFormat, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags, scaler)
//     local pAttachment
//     return pAttachment
// end
int luaCreateDynamicAttachment(lua_State *pLuaState)
{
    VkFormat vkFormat = luaL_checkinteger(pLuaState, 1);
    VkImageUsageFlags vkImageUsageFlags = luaL_checkinteger(pLuaState, 2);
    VkMemoryPropertyFlags vkMemoryPropertyFlags = luaL_checkinteger(pLuaState, 3);
    VkImageAspectFlags vkImageAspectFlags = luaL_checkinteger(pLuaState, 4);
    float scaler = luaL_checknumber(pLuaState, 5);

    Attachment *pAttachment;
    createDynamicAttachment(getGraphicContextPointer(pLuaState), vkFormat, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags, scaler, pAttachment);
    lua_pushlightuserdata(pLuaState, pAttachment); // return pAttachment
    return 1;
}

// function engine.destroyDynamicAttachment(pAttachment)
// end
int luaDestroyDynamicAttachment(lua_State *pLuaState)
{
    VkFormat vkFormat = luaL_checkinteger(pLuaState, 1);
    VkImageUsageFlags vkImageUsageFlags = luaL_checkinteger(pLuaState, 2);
    VkMemoryPropertyFlags vkMemoryPropertyFlags = luaL_checkinteger(pLuaState, 3);
    VkImageAspectFlags vkImageAspectFlags = luaL_checkinteger(pLuaState, 4);
    float scaler = luaL_checknumber(pLuaState, 5);

    Attachment *pAttachment = tickernelMalloc(sizeof(Attachment));
    createDynamicAttachment(getGraphicContextPointer(pLuaState), vkFormat, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags, scaler, pAttachment);
    lua_pushlightuserdata(pLuaState, pAttachment); // return pAttachment
    return 1;
}

// -- void createFixedAttachment(GraphicContext *pGraphicContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height, Attachment *pAttachment);
// function engine.createFixedAttachment(vkFormat, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags, width, height)
//     local pAttachment
//     return pAttachment
// end
// -- void destroyFixedAttachment(GraphicContext *pGraphicContext, Attachment *pAttachment);
// function engine.destroyFixedAttachment(pAttachment)
// end

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