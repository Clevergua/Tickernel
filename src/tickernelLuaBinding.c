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
    luaL_checktype(pLuaState, 1, LUA_TTABLE);
    lua_Unsigned vkFormatCount = lua_rawlen(pLuaState, 1);
    VkFormat vkFormats[vkFormatCount];
    for (uint32_t i = 0; i < vkFormatCount; i++)
    {
        int formatType = lua_geti(pLuaState, 1, i + 1);
        assertLuaType(formatType, LUA_TNUMBER);
        vkFormats[i] = lua_tointeger(pLuaState, -1);
        lua_pop(pLuaState, 1);
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
    createDynamicAttachment(getGraphicContextPointer(pLuaState), vkFormat, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags, scaler, &pAttachment);
    lua_pushlightuserdata(pLuaState, pAttachment);
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

    Attachment *pAttachment = lua_touserdata(pLuaState, 1);

    destroyDynamicAttachment(getGraphicContextPointer(pLuaState), pAttachment);

    return 0;
}

// -- void createFixedAttachment(GraphicContext *pGraphicContext, VkFormat vkFormat, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImageAspectFlags vkImageAspectFlags, uint32_t width, uint32_t height, Attachment *pAttachment);
// function engine.createFixedAttachment(vkFormat, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags, width, height)
//     local pAttachment
//     return pAttachment
// end
int luaCreateFixedAttachment(lua_State *pLuaState)
{
    VkFormat vkFormat = luaL_checkinteger(pLuaState, 1);
    VkImageUsageFlags vkImageUsageFlags = luaL_checkinteger(pLuaState, 2);
    VkMemoryPropertyFlags vkMemoryPropertyFlags = luaL_checkinteger(pLuaState, 3);
    VkImageAspectFlags vkImageAspectFlags = luaL_checkinteger(pLuaState, 4);
    uint32_t width = luaL_checknumber(pLuaState, 5);
    uint32_t height = luaL_checknumber(pLuaState, 6);

    Attachment *pAttachment;
    createFixedAttachment(getGraphicContextPointer(pLuaState), vkFormat, vkImageUsageFlags, vkMemoryPropertyFlags, vkImageAspectFlags, width, height, &pAttachment);
    lua_pushlightuserdata(pLuaState, pAttachment);
    return 1;
}

// function engine.destroyFixedAttachment(pAttachment)
// end
int luaDestroyFixedAttachment(lua_State *pLuaState)
{
    Attachment *pAttachment = lua_touserdata(pLuaState, 1);
    destroyFixedAttachment(getGraphicContextPointer(pLuaState), pAttachment);
    lua_pushlightuserdata(pLuaState, pAttachment);
    return 1;
}

// function engine.getSwapchainAttachment()
//     local pAttachment
//     return pAttachment
// end
int luaGetSwapchainAttachment(lua_State *pLuaState)
{
    lua_pushlightuserdata(pLuaState, &getGraphicContextPointer(pLuaState)->swapchainAttachment);
    return 1;
}

// function engine.createRenderPass(vkAttachmentDescriptions, pAttachments, vkSubpassDescriptions, vkSubpassDependencies,
//                                  renderPassIndex)
//     local pRenderPass = 0
//     return pRenderPass
// end
int luaCreateRenderPass(lua_State *pLuaState)
{
    luaL_checktype(pLuaState, 1, LUA_TTABLE);
    uint32_t attachmentCount = lua_rawlen(pLuaState, 1);
    VkAttachmentDescription vkAttachmentDescriptions[attachmentCount];
    for (uint32_t i = 0; i < attachmentCount; ++i)
    {
        lua_rawgeti(pLuaState, 1, i + 1);

        lua_getfield(pLuaState, -1, "format");
        VkFormat format = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "samples");
        VkSampleCountFlagBits samples = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "loadOp");
        VkAttachmentLoadOp loadOp = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "storeOp");
        VkAttachmentStoreOp storeOp = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "stencilLoadOp");
        VkAttachmentLoadOp stencilLoadOp = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "stencilStoreOp");
        VkAttachmentStoreOp stencilStoreOp = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "initialLayout");
        VkImageLayout initialLayout = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "finalLayout");
        VkImageLayout finalLayout = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        VkAttachmentDescription vkAttachmentDescription = {
            .flags = 0,
            .format = format,
            .samples = samples,
            .loadOp = loadOp,
            .storeOp = storeOp,
            .stencilLoadOp = stencilLoadOp,
            .stencilStoreOp = stencilStoreOp,
            .initialLayout = initialLayout,
            .finalLayout = finalLayout,
        };
        vkAttachmentDescriptions[i] = vkAttachmentDescription;

        lua_pop(pLuaState, 1);
    }

    luaL_checktype(pLuaState, 2, LUA_TTABLE);
    uint32_t attachmentPtrCount = lua_rawlen(pLuaState, 2);
    Attachment **pAttachments = tickernelMalloc(sizeof(Attachment *) * attachmentPtrCount);
    for (uint32_t i = 0; i < attachmentPtrCount; ++i)
    {
        lua_rawgeti(pLuaState, 2, i + 1);
        pAttachments[i] = (Attachment *)lua_touserdata(pLuaState, -1);
        lua_pop(pLuaState, 1);
    }


    luaL_checktype(pLuaState, 3, LUA_TTABLE);
    uint32_t subpassCount = lua_rawlen(pLuaState, 3);
    VkSubpassDescription vkSubpassDescriptions[subpassCount];
    for (uint32_t i = 0; i < subpassCount; ++i)
    {
        lua_rawgeti(pLuaState, 3, i + 1);

        lua_getfield(pLuaState, -1, "pipelineBindPoint");
        VkPipelineBindPoint pipelineBindPoint = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        lua_getfield(pLuaState, -1, "pInputAttachments");
        uint32_t inputAttachmentCount;
        VkAttachmentReference *pInputAttachments;
        if (lua_isnil(pLuaState, -1))
        {
            inputAttachmentCount = 0;
            pInputAttachments = NULL;
        }
        else
        {
            luaL_checktype(pLuaState, -1, LUA_TTABLE);
            inputAttachmentCount = lua_rawlen(pLuaState, -1);
            VkAttachmentReference pInputAttachmentsStack[inputAttachmentCount];
            for (uint32_t j = 0; j < inputAttachmentCount; ++j)
            {
                lua_rawgeti(pLuaState, -1, j + 1);

                lua_getfield(pLuaState, -1, "attachmentIndex");
                uint32_t attachment = luaL_checkinteger(pLuaState, -1);
                lua_pop(pLuaState, 1);

                lua_getfield(pLuaState, -1, "layout");
                VkImageLayout layout = luaL_checkinteger(pLuaState, -1);
                lua_pop(pLuaState, 1);

                lua_pop(pLuaState, 1);

                VkAttachmentReference vkInputAttachment = {
                    .attachment = attachment,
                    .layout = layout,
                };
                pInputAttachmentsStack[j] = vkInputAttachment;
            }
            pInputAttachments = pInputAttachmentsStack;
        }
        lua_pop(pLuaState, 1);

        lua_getfield(pLuaState, -1, "pColorAttachments");
        uint32_t colorAttachmentCount;
        VkAttachmentReference *pColorAttachments;
        if (lua_isnil(pLuaState, -1))
        {
            colorAttachmentCount = 0;
            pColorAttachments = NULL;
        }
        else
        {
            uint32_t colorAttachmentCount = lua_rawlen(pLuaState, -1);
            VkAttachmentReference pColorAttachmentsStack[colorAttachmentCount];
            for (uint32_t j = 0; j < colorAttachmentCount; ++j)
            {
                lua_rawgeti(pLuaState, -1, j + 1);

                lua_getfield(pLuaState, -1, "attachmentIndex");
                uint32_t attachment = luaL_checkinteger(pLuaState, -1);
                lua_pop(pLuaState, 1);

                lua_getfield(pLuaState, -1, "layout");
                VkImageLayout layout = luaL_checkinteger(pLuaState, -1);
                lua_pop(pLuaState, 1);

                lua_pop(pLuaState, 1);

                VkAttachmentReference vkColorAttachment = {
                    .attachment = attachment,
                    .layout = layout,
                };
                pColorAttachmentsStack[j] = vkColorAttachment;
            }
            pColorAttachments = pColorAttachmentsStack;
        }

        lua_pop(pLuaState, 1);

        lua_getfield(pLuaState, -1, "pResolveAttachments");
        uint32_t pResolveAttachmentCount;
        VkAttachmentReference *pResolveAttachments;
        if (lua_isnil(pLuaState, -1))
        {
            pResolveAttachmentCount = 0;
            pResolveAttachments = NULL;
        }
        else
        {
            luaL_checktype(pLuaState, -1, LUA_TTABLE);
            pResolveAttachmentCount = lua_rawlen(pLuaState, -1);
            VkAttachmentReference pResolveAttachmentsStack[pResolveAttachmentCount];
            for (uint32_t j = 0; j < pResolveAttachmentCount; ++j)
            {
                lua_rawgeti(pLuaState, -1, j + 1);

                lua_getfield(pLuaState, -1, "attachmentIndex");
                uint32_t attachment = luaL_checkinteger(pLuaState, -1);
                lua_pop(pLuaState, 1);

                lua_getfield(pLuaState, -1, "layout");
                VkImageLayout layout = luaL_checkinteger(pLuaState, -1);
                lua_pop(pLuaState, 1);

                lua_pop(pLuaState, 1);

                VkAttachmentReference vkResolveAttachment = {
                    .attachment = attachment,
                    .layout = layout,
                };
                pResolveAttachmentsStack[j] = vkResolveAttachment;
            }
            pResolveAttachments = pResolveAttachmentsStack;
        }
        lua_pop(pLuaState, 1);

        VkSubpassDescription vkSubpassDescription = {
            .flags = 0,
            .pipelineBindPoint = pipelineBindPoint,
            .inputAttachmentCount = inputAttachmentCount,
            .pInputAttachments = pInputAttachments,
            .colorAttachmentCount = colorAttachmentCount,
            .pColorAttachments = pColorAttachments,
            .pResolveAttachments = pResolveAttachments,
            .pDepthStencilAttachment = pDepthStencilAttachment,
            .preserveAttachmentCount = preserveAttachmentCount,
            .pPreserveAttachments = pPreserveAttachments,
        };
        lua_pop(pLuaState, 1);
    }

    // 参数4: vkSubpassDependencies (table)
    luaL_checktype(pLuaState, 4, LUA_TTABLE);
    uint32_t dependencyCount = lua_rawlen(pLuaState, 4);
    VkSubpassDependency *vkSubpassDependencies = tickernelMalloc(sizeof(VkSubpassDependency) * dependencyCount);
    memset(vkSubpassDependencies, 0, sizeof(VkSubpassDependency) * dependencyCount);
    for (uint32_t i = 0; i < dependencyCount; ++i)
    {
        lua_rawgeti(pLuaState, 4, i + 1);
        VkSubpassDependency *dep = &vkSubpassDependencies[i];
        lua_getfield(pLuaState, -1, "srcSubpass");
        dep->srcSubpass = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "dstSubpass");
        dep->dstSubpass = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "srcStageMask");
        dep->srcStageMask = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "dstStageMask");
        dep->dstStageMask = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "srcAccessMask");
        dep->srcAccessMask = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "dstAccessMask");
        dep->dstAccessMask = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "dependencyFlags");
        dep->dependencyFlags = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_pop(pLuaState, 1);
    }

    // 参数5: renderPassIndex (integer)
    uint32_t renderPassIndex = luaL_checkinteger(pLuaState, 5);

    // 创建 RenderPass
    RenderPass *pRenderPass = tickernelMalloc(sizeof(RenderPass));
    createRenderPass(
        getGraphicContextPointer(pLuaState),
        attachmentCount,
        vkAttachmentDescriptions,
        pAttachments,
        subpassCount,
        vkSubpassDescriptions,
        dependencyCount,
        vkSubpassDependencies,
        renderPassIndex,
        pRenderPass);

    lua_pushlightuserdata(pLuaState, pRenderPass);

    return 1;
}

// function engine.destroyRenderPass(pRenderPass)
// end
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