#include "tknLuaBinding.h"

static int luaGetSupportedFormat(lua_State *pLuaState)
{
    printLuaStack(pLuaState);
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

static int luaCreateDynamicAttachmentPtr(lua_State *pLuaState)
{
    printLuaStack(pLuaState);
    GfxContext *pGfxContext = (GfxContext *)lua_touserdata(pLuaState, -6);
    VkFormat vkFormat = (VkFormat)lua_tointeger(pLuaState, -5);
    VkImageUsageFlags vkImageUsageFlags = (VkImageUsageFlags)lua_tointeger(pLuaState, -4);
    VkMemoryPropertyFlags vkMemoryPropertyFlags = (VkMemoryPropertyFlags)lua_tointeger(pLuaState, -3);
    VkImageAspectFlags vkImageAspectFlags = (VkImageAspectFlags)lua_tointeger(pLuaState, -2);
    float scaler = (float)lua_tonumber(pLuaState, -1);
    Attachment *attachment = createDynamicAttachmentPtr(pGfxContext, vkFormat, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags, scaler);
    lua_pushlightuserdata(pLuaState, attachment);
    return 1;
}

static int luaCreateFixedAttachmentPtr(lua_State *pLuaState)
{
     printLuaStack(pLuaState);
    GfxContext *pGfxContext = (GfxContext *)lua_touserdata(pLuaState, -7);
    VkFormat vkFormat = (VkFormat)lua_tointeger(pLuaState, -6);
    VkImageUsageFlags vkImageUsageFlags = (VkImageUsageFlags)lua_tointeger(pLuaState, -5);
    VkMemoryPropertyFlags vkMemoryPropertyFlags = (VkMemoryPropertyFlags)lua_tointeger(pLuaState, -4);
    VkImageAspectFlags vkImageAspectFlags = (VkImageAspectFlags)lua_tointeger(pLuaState, -3);
    uint32_t width = (uint32_t)lua_tointeger(pLuaState, -2);
    uint32_t height = (uint32_t)lua_tointeger(pLuaState, -1);
    Attachment *attachment = createFixedAttachmentPtr(pGfxContext, vkFormat, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags, width, height);
    lua_pushlightuserdata(pLuaState, attachment);
    return 1;
}

static int luaGetSwapchainAttachmentPtr(lua_State *pLuaState)
{
    printLuaStack(pLuaState);
    GfxContext *pGfxContext = (GfxContext *)lua_touserdata(pLuaState, -1);
    Attachment *attachment = getSwapchainAttachmentPtr(pGfxContext);
    lua_pushlightuserdata(pLuaState, attachment);
    return 1;
}

static int luaDestroyDynamicAttachmentPtr(lua_State *pLuaState)
{
    printLuaStack(pLuaState);
    GfxContext *pGfxContext = (GfxContext *)lua_touserdata(pLuaState, -2);
    Attachment *pAttachment = (Attachment *)lua_touserdata(pLuaState, -1);
    destroyDynamicAttachmentPtr(pGfxContext, pAttachment);
    return 0;
}

static int luaDestroyFixedAttachmentPtr(lua_State *pLuaState)
{
    printLuaStack(pLuaState);
    GfxContext *pGfxContext = (GfxContext *)lua_touserdata(pLuaState, -2);
    Attachment *pAttachment = (Attachment *)lua_touserdata(pLuaState, -1);
    destroyFixedAttachmentPtr(pGfxContext, pAttachment);
    return 0;
}

void bindFunctions(lua_State *pLuaState)
{
    luaL_Reg regs[] = {
        {"getSupportedFormat", luaGetSupportedFormat},
        {"createDynamicAttachmentPtr", luaCreateDynamicAttachmentPtr},
        {"createFixedAttachmentPtr", luaCreateFixedAttachmentPtr},
        {"getSwapchainAttachmentPtr", luaGetSwapchainAttachmentPtr},
        {"destroyDynamicAttachmentPtr", luaDestroyDynamicAttachmentPtr},
        {"destroyFixedAttachmentPtr", luaDestroyFixedAttachmentPtr},
        {NULL, NULL},
    };
    luaL_newlib(pLuaState, regs);
    lua_setglobal(pLuaState, "gfx");
}

void printLuaStack(lua_State *pLuaState)
{
    int top = lua_gettop(pLuaState);
    printf("=== Lua Stack ===\n");
    printf("Stack size: %d\n", top);
    if (top == 0)
    {
        printf("Stack is empty\n");
    }
    else
    {
        for (int i = 1; i <= top; i++)
        {
            int type = lua_type(pLuaState, i);
            printf("[%d] (%s): ", i, lua_typename(pLuaState, type));
            switch (type)
            {
            case LUA_TNIL:
                printf("nil");
                break;
            case LUA_TBOOLEAN:
                printf(lua_toboolean(pLuaState, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                printf("%g", lua_tonumber(pLuaState, i));
                break;
            case LUA_TSTRING:
                printf("\"%s\"", lua_tostring(pLuaState, i));
                break;
            case LUA_TTABLE:
                printf("table: %p", lua_topointer(pLuaState, i));
                break;
            case LUA_TFUNCTION:
                printf("function: %p", lua_topointer(pLuaState, i));
                break;
            case LUA_TUSERDATA:
                printf("userdata: %p", lua_topointer(pLuaState, i));
                break;
            case LUA_TLIGHTUSERDATA:
                printf("lightuserdata: %p", lua_touserdata(pLuaState, i));
                break;
            case LUA_TTHREAD:
                printf("thread: %p", lua_topointer(pLuaState, i));
                break;
            default:
                printf("unknown");
                break;
            }
            printf("\n");
        }
    }
    printf("==================\n\n");
}
