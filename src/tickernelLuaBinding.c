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

static VkAttachmentReference handleVkAttachmentReference(lua_State *pLuaState)
{
    luaL_checktype(pLuaState, -1, LUA_TTABLE);

    lua_getfield(pLuaState, -1, "attachmentIndex");
    uint32_t attachmentIndex = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);

    lua_getfield(pLuaState, -1, "layout");
    VkImageLayout layout = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);

    VkAttachmentReference vkAttachmentReference = {
        .attachment = attachmentIndex,
        .layout = layout,
    };

    return vkAttachmentReference;
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
    VkAttachmentDescription *vkAttachmentDescriptions = tickernelMalloc(sizeof(VkAttachmentDescription) * attachmentCount);
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
        pAttachments[i] = lua_touserdata(pLuaState, -1);
        lua_pop(pLuaState, 1);
    }

    luaL_checktype(pLuaState, 3, LUA_TTABLE);
    uint32_t subpassCount = lua_rawlen(pLuaState, 3);
    VkSubpassDescription *vkSubpassDescriptions = tickernelMalloc(sizeof(VkSubpassDescription) * subpassCount);
    for (uint32_t i = 0; i < subpassCount; ++i)
    {
        lua_rawgeti(pLuaState, 3, i + 1);

        lua_getfield(pLuaState, -1, "pipelineBindPoint");
        VkPipelineBindPoint pipelineBindPoint = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        lua_getfield(pLuaState, -1, "pInputAttachments");
        luaL_checktype(pLuaState, -1, LUA_TTABLE);
        uint32_t inputAttachmentCount = lua_rawlen(pLuaState, -1);
        VkAttachmentReference *pInputAttachments = tickernelMalloc(sizeof(VkAttachmentReference) * inputAttachmentCount);
        for (uint32_t j = 0; j < inputAttachmentCount; ++j)
        {
            lua_rawgeti(pLuaState, -1, j + 1);
            pInputAttachments[j] = handleVkAttachmentReference(pLuaState);
            lua_pop(pLuaState, 1);
        }
        lua_pop(pLuaState, 1);

        lua_getfield(pLuaState, -1, "pColorAttachments");
        luaL_checktype(pLuaState, -1, LUA_TTABLE);
        uint32_t colorAttachmentCount = lua_rawlen(pLuaState, -1);
        VkAttachmentReference *pColorAttachments = tickernelMalloc(sizeof(VkAttachmentReference) * colorAttachmentCount);
        for (uint32_t j = 0; j < colorAttachmentCount; ++j)
        {
            lua_rawgeti(pLuaState, -1, j + 1);
            pColorAttachments[j] = handleVkAttachmentReference(pLuaState);
            lua_pop(pLuaState, 1);
        }
        lua_pop(pLuaState, 1);

        lua_getfield(pLuaState, -1, "pResolveAttachments");
        luaL_checktype(pLuaState, -1, LUA_TTABLE);

        uint32_t pResolveAttachmentCount = lua_rawlen(pLuaState, -1);
        VkAttachmentReference *pResolveAttachments = tickernelMalloc(sizeof(VkAttachmentReference) * pResolveAttachmentCount);
        for (uint32_t j = 0; j < pResolveAttachmentCount; ++j)
        {
            lua_rawgeti(pLuaState, -1, j + 1);
            pResolveAttachments[j] = handleVkAttachmentReference(pLuaState);
            lua_pop(pLuaState, 1);
        }
        lua_pop(pLuaState, 1);

        lua_getfield(pLuaState, -1, "pDepthStencilAttachment");
        VkAttachmentReference *pDepthStencilAttachment;
        if (lua_isnil(pLuaState, -1))
        {
            pDepthStencilAttachment = NULL;
        }
        else
        {
            luaL_checktype(pLuaState, -1, LUA_TTABLE);
            pDepthStencilAttachment = tickernelMalloc(sizeof(VkAttachmentReference));
            *pDepthStencilAttachment = handleVkAttachmentReference(pLuaState);
        }
        lua_pop(pLuaState, 1);

        lua_getfield(pLuaState, -1, "pPreserveAttachments");
        luaL_checktype(pLuaState, -1, LUA_TTABLE);
        uint32_t preserveAttachmentCount = lua_rawlen(pLuaState, -1);
        uint32_t *pPreserveAttachments = tickernelMalloc(sizeof(uint32_t) * preserveAttachmentCount);
        for (uint32_t j = 0; j < preserveAttachmentCount; j++)
        {
            lua_rawgeti(pLuaState, -1, j + 1);
            lua_Integer attachment = luaL_checkinteger(pLuaState, -1);
            lua_pop(pLuaState, 1);
            pPreserveAttachments[j] = attachment;
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

        vkSubpassDescriptions[i] = vkSubpassDescription;
    }

    luaL_checktype(pLuaState, 4, LUA_TTABLE);
    uint32_t dependencyCount = lua_rawlen(pLuaState, 4);
    VkSubpassDependency *vkSubpassDependencies = tickernelMalloc(sizeof(VkSubpassDependency) * dependencyCount);
    for (uint32_t i = 0; i < dependencyCount; ++i)
    {
        lua_rawgeti(pLuaState, 4, i + 1);

        lua_getfield(pLuaState, -1, "srcSubpass");
        uint32_t srcSubpass = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "dstSubpass");
        uint32_t dstSubpass = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "srcStageMask");
        VkPipelineStageFlags srcStageMask = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "dstStageMask");
        VkPipelineStageFlags dstStageMask = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "srcAccessMask");
        VkAccessFlags srcAccessMask = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "dstAccessMask");
        VkAccessFlags dstAccessMask = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "dependencyFlags");
        VkDependencyFlags dependencyFlags = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        VkSubpassDependency vkSubpassDependency = {
            .srcSubpass = srcSubpass,
            .dstSubpass = dstSubpass,
            .srcStageMask = srcStageMask,
            .dstStageMask = dstStageMask,
            .srcAccessMask = srcAccessMask,
            .dstAccessMask = dstAccessMask,
            .dependencyFlags = dependencyFlags,
        };

        lua_pop(pLuaState, 1);
        vkSubpassDependencies[i] = vkSubpassDependency;
    }

    uint32_t renderPassIndex = luaL_checkinteger(pLuaState, 5);

    RenderPass *pRenderPass;
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
        &pRenderPass);

    tickernelFree(vkAttachmentDescriptions);
    tickernelFree(pAttachments);
    for (uint32_t i = 0; i < subpassCount; ++i)
    {
        tickernelFree((void *)vkSubpassDescriptions[i].pInputAttachments);
        tickernelFree((void *)vkSubpassDescriptions[i].pColorAttachments);
        tickernelFree((void *)vkSubpassDescriptions[i].pResolveAttachments);
        if (vkSubpassDescriptions[i].pDepthStencilAttachment != NULL)
        {
            tickernelFree((void *)vkSubpassDescriptions[i].pDepthStencilAttachment);
        }
        tickernelFree((void *)vkSubpassDescriptions[i].pPreserveAttachments);
    }
    tickernelFree(vkSubpassDescriptions);
    tickernelFree(vkSubpassDependencies);
    lua_pushlightuserdata(pLuaState, pRenderPass);

    return 1;
}

// function engine.destroyRenderPass(pRenderPass)
// end
int luaDestroyRenderPass(lua_State *pLuaState)
{
    RenderPass *pRenderPass = lua_touserdata(pLuaState, 1);
    destroyRenderPass(getGraphicContextPointer(pLuaState), pRenderPass);
    return 0;
}

int luaGetSwapchainExtent(lua_State *pLuaState)
{
    
    return 1;
}

int luaDestroyAttachment(lua_State *pLuaState)
{

    return 0;
}

