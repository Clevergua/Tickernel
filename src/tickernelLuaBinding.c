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
    Attachment *pAttachments[attachmentPtrCount];
    for (uint32_t i = 0; i < attachmentPtrCount; ++i)
    {
        lua_rawgeti(pLuaState, 2, i + 1);
        pAttachments[i] = lua_touserdata(pLuaState, -1);
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
    VkSubpassDependency vkSubpassDependencies[dependencyCount];
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

// function engine.createPipeline(stages, vertexInputState, inputAssemblyState, viewportState,
//                                rasterizationState,
//                                multisampleState, depthStencilState, colorBlendState,
//                                dynamicState, vkDescriptorSetLayoutCreateInfo, renderPass, subpassIndex,
//                                vkDescriptorPoolSizes, pipelineIndex)
//     local pPipeline
//     return pPipeline
// end
int luaCreatePipeline(lua_State *pLuaState)
{
    // handle stages 1
    luaL_checktype(pLuaState, 1, LUA_TTABLE);
    uint32_t stageCount = lua_rawlen(pLuaState, 1);
    const char *shaderPaths[stageCount];
    VkPipelineShaderStageCreateInfo stages[stageCount];
    for (uint32_t i = 0; i < stageCount; i++)
    {
        lua_rawgeti(pLuaState, 1, i + 1);
        lua_getfield(pLuaState, -1, "stage");
        VkShaderStageFlagBits stage = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "shaderPath");
        shaderPaths[i] = luaL_checkstring(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "pName");
        const char *pName = luaL_checkstring(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_pop(pLuaState, 1);

        VkPipelineShaderStageCreateInfo vkPipelineShaderStageCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = stage,
            .module = VK_NULL_HANDLE,
            .pName = pName,
            .pSpecializationInfo = NULL,
        };
        stages[i] = vkPipelineShaderStageCreateInfo;
    }

    // handle vertexInputState 2
    luaL_checktype(pLuaState, 2, LUA_TTABLE);
    lua_getfield(pLuaState, 2, "pVertexBindingDescriptions");
    luaL_checktype(pLuaState, -1, LUA_TTABLE);
    uint32_t vertexBindingDescriptionCount = lua_rawlen(pLuaState, -1);
    VkVertexInputBindingDescription pVertexBindingDescriptions[vertexBindingDescriptionCount];
    for (uint32_t i = 0; i < vertexBindingDescriptionCount; ++i)
    {
        lua_rawgeti(pLuaState, -1, i + 1);
        lua_getfield(pLuaState, -1, "binding");
        uint32_t binding = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "stride");
        uint32_t stride = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "inputRate");
        VkVertexInputRate inputRate = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_pop(pLuaState, 1);

        VkVertexInputBindingDescription vkVertexInputBindingDescription = {
            .binding = binding,
            .stride = stride,
            .inputRate = inputRate,
        };
        pVertexBindingDescriptions[i] = vkVertexInputBindingDescription;
    }
    lua_pop(pLuaState, 1);

    // handle vertexInputState 2
    lua_getfield(pLuaState, 2, "pVertexAttributeDescriptions");
    luaL_checktype(pLuaState, -1, LUA_TTABLE);
    uint32_t vertexAttributeDescriptionCount = lua_rawlen(pLuaState, -1);
    VkVertexInputAttributeDescription pVertexAttributeDescriptions[vertexAttributeDescriptionCount];
    for (uint32_t i = 0; i < vertexAttributeDescriptionCount; ++i)
    {
        lua_rawgeti(pLuaState, -1, i + 1);
        lua_getfield(pLuaState, -1, "location");
        uint32_t location = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "binding");
        uint32_t binding = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "format");
        VkFormat format = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "offset");
        uint32_t offset = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_pop(pLuaState, 1);

        VkVertexInputAttributeDescription vkVertexInputAttributeDescription = {
            .location = location,
            .binding = binding,
            .format = format,
            .offset = offset,
        };
        pVertexAttributeDescriptions[i] = vkVertexInputAttributeDescription;
    }
    VkPipelineVertexInputStateCreateInfo vertexInputState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = vertexBindingDescriptionCount,
        .pVertexBindingDescriptions = pVertexBindingDescriptions,
        .vertexAttributeDescriptionCount = vertexAttributeDescriptionCount,
        .pVertexAttributeDescriptions = pVertexAttributeDescriptions,
    };
    lua_pop(pLuaState, 1);

    // handle inputAssemblyState 3
    luaL_checktype(pLuaState, -1, LUA_TTABLE);
    lua_getfield(pLuaState, 3, "topology");
    VkPrimitiveTopology topology = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 3, "primitiveRestartEnable");
    VkBool32 primitiveRestartEnable = lua_toboolean(pLuaState, -1);
    lua_pop(pLuaState, 1);
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = topology,
        .primitiveRestartEnable = primitiveRestartEnable,
    };

    // handle viewportState 4
    luaL_checktype(pLuaState, 4, LUA_TTABLE);
    lua_getfield(pLuaState, 4, "pViewports");
    luaL_checktype(pLuaState, -1, LUA_TTABLE);
    uint32_t viewportCount = lua_rawlen(pLuaState, -1);
    VkViewport pViewports[viewportCount];
    for (uint32_t i = 0; i < viewportCount; ++i)
    {
        lua_rawgeti(pLuaState, -1, i + 1);
        lua_getfield(pLuaState, -1, "x");
        float x = luaL_checknumber(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "y");
        float y = luaL_checknumber(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "width");
        float width = luaL_checknumber(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "height");
        float height = luaL_checknumber(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "minDepth");
        float minDepth = luaL_checknumber(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "maxDepth");
        float maxDepth = luaL_checknumber(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_pop(pLuaState, 1);

        VkViewport vkViewport = {
            .x = x,
            .y = y,
            .width = width,
            .height = height,
            .minDepth = minDepth,
            .maxDepth = maxDepth,
        };
        pViewports[i] = vkViewport;
    }
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 4, "pScissors");
    luaL_checktype(pLuaState, -1, LUA_TTABLE);
    uint32_t scissorCount = lua_rawlen(pLuaState, -1);
    VkRect2D pScissors[scissorCount];
    for (uint32_t i = 0; i < scissorCount; ++i)
    {
        lua_rawgeti(pLuaState, -1, i + 1);
        lua_getfield(pLuaState, -1, "offset");
        luaL_checktype(pLuaState, -1, LUA_TTABLE);
        lua_getfield(pLuaState, -1, "x");
        int32_t x = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "y");
        int32_t y = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_pop(pLuaState, 1);

        lua_getfield(pLuaState, -1, "extent");
        luaL_checktype(pLuaState, -1, LUA_TTABLE);
        lua_getfield(pLuaState, -1, "width");
        uint32_t width = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "height");
        uint32_t height = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_pop(pLuaState, 1);

        VkRect2D vkRect2D = {
            .offset = {.x = x,
                       .y = y},
            .extent = {.width = width,
                       .height = height},
        };
        pScissors[i] = vkRect2D;
    }
    VkPipelineViewportStateCreateInfo viewportState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 0,
        .pViewports = NULL,
        .scissorCount = 0,
        .pScissors = NULL,
    };
    lua_pop(pLuaState, 1);

    // handle rasterizationState 5
    luaL_checktype(pLuaState, 5, LUA_TTABLE);
    lua_getfield(pLuaState, 5, "depthClampEnable");
    VkBool32 depthClampEnable = lua_toboolean(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 5, "rasterizerDiscardEnable");
    VkBool32 rasterizerDiscardEnable = lua_toboolean(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 5, "polygonMode");
    VkPolygonMode polygonMode = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 5, "cullMode");
    VkCullModeFlags cullMode = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 5, "frontFace");
    VkFrontFace frontFace = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 5, "depthBiasEnable");
    VkBool32 depthBiasEnable = lua_toboolean(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 5, "depthBiasConstantFactor");
    float depthBiasConstantFactor = luaL_checknumber(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 5, "depthBiasClamp");
    float depthBiasClamp = luaL_checknumber(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 5, "depthBiasSlopeFactor");
    float depthBiasSlopeFactor = luaL_checknumber(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 5, "lineWidth");
    float lineWidth = luaL_checknumber(pLuaState, -1);
    lua_pop(pLuaState, 1);
    VkPipelineRasterizationStateCreateInfo rasterizationState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = depthClampEnable,
        .rasterizerDiscardEnable = rasterizerDiscardEnable,
        .polygonMode = polygonMode,
        .cullMode = cullMode,
        .frontFace = frontFace,
        .depthBiasEnable = depthBiasEnable,
        .depthBiasConstantFactor = depthBiasConstantFactor,
        .depthBiasClamp = depthBiasClamp,
        .depthBiasSlopeFactor = depthBiasSlopeFactor,
        .lineWidth = lineWidth,
    };

    // handle rasterizationState 6
    luaL_checktype(pLuaState, 6, LUA_TTABLE);

    lua_getfield(pLuaState, 6, "rasterizationSamples");
    VkSampleCountFlagBits rasterizationSamples = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 6, "sampleShadingEnable");
    VkBool32 sampleShadingEnable = lua_toboolean(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 6, "minSampleShading");
    float minSampleShading = luaL_checknumber(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 6, "sampleMask");
    uint32_t sampleMask = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 6, "alphaToCoverageEnable");
    VkBool32 alphaToCoverageEnable = lua_toboolean(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 6, "alphaToOneEnable");
    VkBool32 alphaToOneEnable = lua_toboolean(pLuaState, -1);
    lua_pop(pLuaState, 1);
    VkPipelineMultisampleStateCreateInfo multisampleState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = rasterizationSamples,
        .sampleShadingEnable = sampleShadingEnable,
        .minSampleShading = minSampleShading,
        .pSampleMask = &sampleMask,
        .alphaToCoverageEnable = alphaToCoverageEnable,
        .alphaToOneEnable = alphaToOneEnable,
    };

    // handle depthStencilState 7
    luaL_checktype(pLuaState, 7, LUA_TTABLE);
    lua_getfield(pLuaState, 7, "depthTestEnable");
    VkBool32 depthTestEnable = lua_toboolean(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 7, "depthWriteEnable");
    VkBool32 depthWriteEnable = lua_toboolean(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 7, "depthCompareOp");
    VkCompareOp depthCompareOp = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 7, "depthBoundsTestEnable");
    VkBool32 depthBoundsTestEnable = lua_toboolean(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 7, "stencilTestEnable");
    VkBool32 stencilTestEnable = lua_toboolean(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 7, "front");
    luaL_checktype(pLuaState, -1, LUA_TTABLE);
    lua_getfield(pLuaState, -1, "failOp");
    VkStencilOp failOp = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, -1, "passOp");
    VkStencilOp passOp = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, -1, "depthFailOp");
    VkStencilOp depthFailOp = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, -1, "compareOp");
    VkCompareOp compareOp = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, -1, "compareMask");
    uint32_t compareMask = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, -1, "writeMask");
    uint32_t writeMask = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, -1, "reference");
    uint32_t reference = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    VkStencilOpState front = {
        .failOp = failOp,
        .passOp = passOp,
        .depthFailOp = depthFailOp,
        .compareOp = compareOp,
        .compareMask = compareMask,
        .writeMask = writeMask,
        .reference = reference,
    };
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 7, "back");
    luaL_checktype(pLuaState, -1, LUA_TTABLE);
    lua_getfield(pLuaState, -1, "failOp");
    failOp = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, -1, "passOp");
    passOp = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, -1, "depthFailOp");
    depthFailOp = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, -1, "compareOp");
    compareOp = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, -1);
    lua_getfield(pLuaState, -1, "compareMask");
    compareMask = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, -1, "writeMask");
    writeMask = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, -1, "reference");
    reference = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    VkStencilOpState back = {
        .failOp = failOp,
        .passOp = passOp,
        .depthFailOp = depthFailOp,
        .compareOp = compareOp,
        .compareMask = compareMask,
        .writeMask = writeMask,
        .reference = reference,
    };
    lua_pop(pLuaState, 1);
    VkPipelineDepthStencilStateCreateInfo depthStencilState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = depthTestEnable,
        .depthWriteEnable = depthWriteEnable,
        .depthCompareOp = depthCompareOp,
        .depthBoundsTestEnable = depthBoundsTestEnable,
        .stencilTestEnable = stencilTestEnable,
        .front = front,
        .back = back,
    };

    // handle colorBlendState 8
    luaL_checktype(pLuaState, 8, LUA_TTABLE);
    lua_getfield(pLuaState, 8, "logicOpEnable");
    VkBool32 logicOpEnable = lua_toboolean(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 8, "logicOp");
    VkLogicOp logicOp = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 8, "pAttachments");
    luaL_checktype(pLuaState, -1, LUA_TTABLE);
    uint32_t attachmentCount = lua_rawlen(pLuaState, -1);
    VkPipelineColorBlendAttachmentState pAttachments[attachmentCount];
    for (uint32_t i = 0; i < attachmentCount; ++i)
    {
        lua_rawgeti(pLuaState, -1, i + 1);
        lua_getfield(pLuaState, -1, "blendEnable");
        VkBool32 blendEnable = lua_toboolean(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "srcColorBlendFactor");
        VkBlendFactor srcColorBlendFactor = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "dstColorBlendFactor");
        VkBlendFactor dstColorBlendFactor = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "colorBlendOp");
        VkBlendOp colorBlendOp = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "srcAlphaBlendFactor");
        VkBlendFactor srcAlphaBlendFactor = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "dstAlphaBlendFactor");
        VkBlendFactor dstAlphaBlendFactor = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "alphaBlendOp");
        VkBlendOp alphaBlendOp = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "colorWriteMask");
        VkColorComponentFlags colorWriteMask = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        VkPipelineColorBlendAttachmentState vkPipelineColorBlendAttachmentState = {
            .blendEnable = blendEnable,
            .srcColorBlendFactor = srcColorBlendFactor,
            .dstColorBlendFactor = dstColorBlendFactor,
            .colorBlendOp = colorBlendOp,
            .srcAlphaBlendFactor = srcAlphaBlendFactor,
            .dstAlphaBlendFactor = dstAlphaBlendFactor,
            .alphaBlendOp = alphaBlendOp,
            .colorWriteMask = colorWriteMask,
        };
        pAttachments[i] = vkPipelineColorBlendAttachmentState;
        lua_pop(pLuaState, 1);
    }
    lua_pop(pLuaState, 1);
    lua_getfield(pLuaState, 8, "blendConstants");
    luaL_checktype(pLuaState, -1, LUA_TTABLE);
    float blendConstants[4];
    for (uint32_t i = 0; i < 4; i++)
    {
        lua_geti(pLuaState, -1, i + 1);
        blendConstants[i] = luaL_checknumber(pLuaState, -1);
        lua_pop(pLuaState, 1);
    }
    lua_pop(pLuaState, 1);
    VkPipelineColorBlendStateCreateInfo colorBlendState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = logicOpEnable,
        .logicOp = logicOp,
        .attachmentCount = attachmentCount,
        .pAttachments = pAttachments,
        .blendConstants = {
            blendConstants[0],
            blendConstants[1],
            blendConstants[2],
            blendConstants[3],
        },
    };

    // handle dynamicState 9
    luaL_checktype(pLuaState, 9, LUA_TTABLE);
    lua_getfield(pLuaState, 9, "pDynamicStates");
    luaL_checktype(pLuaState, -1, LUA_TTABLE);
    uint32_t dynamicStateCount = lua_rawlen(pLuaState, -1);
    VkDynamicState pDynamicStates[dynamicStateCount];
    for (uint32_t i = 0; i < dynamicStateCount; ++i)
    {
        lua_rawgeti(pLuaState, -1, i + 1);
        VkDynamicState dynamicState = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        pDynamicStates[i] = dynamicState;
    }
    VkPipelineDynamicStateCreateInfo dynamicState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = dynamicStateCount,
        .pDynamicStates = pDynamicStates,
    };
    lua_pop(pLuaState, 1);


    // handle vkDescriptorSetLayoutCreateInfo 10
    luaL_checktype(pLuaState, 10, LUA_TTABLE);
    lua_getfield(pLuaState, 10, "bindings");
    luaL_checktype(pLuaState, -1, LUA_TTABLE);
    uint32_t bindingCount = lua_rawlen(pLuaState, -1);
    VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBindings[bindingCount];
    for (uint32_t i = 0; i < bindingCount; ++i)
    {
        lua_rawgeti(pLuaState, -1, i + 1);
        lua_getfield(pLuaState, -1, "binding");
        uint32_t binding = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "descriptorType");
        VkDescriptorType descriptorType = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "descriptorCount");
        uint32_t descriptorCount = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "stageFlags");
        VkShaderStageFlags stageFlags = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = {
            .binding = binding,
            .descriptorType = descriptorType,
            .descriptorCount = descriptorCount,
            .stageFlags = stageFlags,
            .pImmutableSamplers = NULL,
        };
        vkDescriptorSetLayoutBindings[i] = vkDescriptorSetLayoutBinding;
    }
    lua_pop(pLuaState, 1);
    VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = bindingCount,
        .pBindings = vkDescriptorSetLayoutBindings,
    };

    // handle pRenderPass 11
    luaL_checktype(pLuaState, 11, LUA_TLIGHTUSERDATA);
    RenderPass *pRenderPass = lua_touserdata(pLuaState, 11);

    // handle subpassIndex 12
    luaL_checktype(pLuaState, 12, LUA_TNUMBER);
    uint32_t subpassIndex = luaL_checkinteger(pLuaState, 12);

    luaL_checktype(pLuaState, 13, LUA_TTABLE);
    uint32_t vkDescriptorPoolSizeCount = lua_rawlen(pLuaState, 13);
    VkDescriptorPoolSize vkDescriptorPoolSizes[vkDescriptorPoolSizeCount];
    for (uint32_t i = 0; i < vkDescriptorPoolSizeCount; ++i)
    {
        lua_rawgeti(pLuaState, 13, i + 1);
        lua_getfield(pLuaState, -1, "type");
        VkDescriptorType type = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        lua_getfield(pLuaState, -1, "descriptorCount");
        uint32_t descriptorCount = luaL_checkinteger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        VkDescriptorPoolSize vkDescriptorPoolSize = {
            .type = type,
            .descriptorCount = descriptorCount,
        };
        vkDescriptorPoolSizes[i] = vkDescriptorPoolSize;
        lua_pop(pLuaState, 1);
    }
    lua_pop(pLuaState, 1);

    // handle pipelineIndex 14
    luaL_checktype(pLuaState, 14, LUA_TNUMBER);
    uint32_t pipelineIndex = luaL_checkinteger(pLuaState, 14);

    Pipeline *pPipeline;
    createPipeline(getGraphicContextPointer(pLuaState), stageCount, shaderPaths, stages, vertexInputState, inputAssemblyState, viewportState,
                   rasterizationState, multisampleState, depthStencilState, colorBlendState, dynamicState,
                   vkDescriptorSetLayoutCreateInfo, pRenderPass, subpassIndex, vkDescriptorPoolSizeCount,
                   vkDescriptorPoolSizes, pipelineIndex, &pPipeline);

    lua_pushlightuserdata(pLuaState, pPipeline);
    return 1;
}

// function engine.destroyPipeline(pRenderPass, subpassIndex, pPipeline)
// end
int luaDestroyPipeline(lua_State *pLuaState)
{
    RenderPass *pRenderPass = lua_touserdata(pLuaState, 1);
    uint32_t subpassIndex = luaL_checkinteger(pLuaState, 2);
    Pipeline *pPipeline = lua_touserdata(pLuaState, 3);
    destroyPipeline(getGraphicContextPointer(pLuaState), pRenderPass, subpassIndex, pPipeline);
    return 0;
}



