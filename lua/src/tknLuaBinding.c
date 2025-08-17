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

static int luaCreateDynamicAttachmentPtr(lua_State *pLuaState)
{
    GfxContext *pGfxContext = (GfxContext *)lua_touserdata(pLuaState, -5);
    VkFormat vkFormat = (VkFormat)lua_tointeger(pLuaState, -4);
    VkImageUsageFlags vkImageUsageFlags = (VkImageUsageFlags)lua_tointeger(pLuaState, -3);
    VkImageAspectFlags vkImageAspectFlags = (VkImageAspectFlags)lua_tointeger(pLuaState, -2);
    float scaler = (float)lua_tonumber(pLuaState, -1);
    Attachment *pAttachment = createDynamicAttachmentPtr(pGfxContext, vkFormat, vkImageUsageFlags, vkImageAspectFlags, scaler);
    lua_pushlightuserdata(pLuaState, pAttachment);
    return 1;
}

static int luaCreateFixedAttachmentPtr(lua_State *pLuaState)
{
    GfxContext *pGfxContext = (GfxContext *)lua_touserdata(pLuaState, -6);
    VkFormat vkFormat = (VkFormat)lua_tointeger(pLuaState, -5);
    VkImageUsageFlags vkImageUsageFlags = (VkImageUsageFlags)lua_tointeger(pLuaState, -4);
    VkImageAspectFlags vkImageAspectFlags = (VkImageAspectFlags)lua_tointeger(pLuaState, -3);
    uint32_t width = (uint32_t)lua_tointeger(pLuaState, -2);
    uint32_t height = (uint32_t)lua_tointeger(pLuaState, -1);
    Attachment *pAttachment = createFixedAttachmentPtr(pGfxContext, vkFormat, vkImageUsageFlags, vkImageAspectFlags, width, height);
    lua_pushlightuserdata(pLuaState, pAttachment);
    return 1;
}

static int luaGetSwapchainAttachmentPtr(lua_State *pLuaState)
{
    GfxContext *pGfxContext = (GfxContext *)lua_touserdata(pLuaState, -1);
    Attachment *pAttachment = getSwapchainAttachmentPtr(pGfxContext);
    lua_pushlightuserdata(pLuaState, pAttachment);
    return 1;
}

static int luaDestroyDynamicAttachmentPtr(lua_State *pLuaState)
{
    GfxContext *pGfxContext = (GfxContext *)lua_touserdata(pLuaState, -2);
    Attachment *pAttachment = (Attachment *)lua_touserdata(pLuaState, -1);
    destroyDynamicAttachmentPtr(pGfxContext, pAttachment);
    return 0;
}

static int luaDestroyFixedAttachmentPtr(lua_State *pLuaState)
{
    GfxContext *pGfxContext = (GfxContext *)lua_touserdata(pLuaState, -2);
    Attachment *pAttachment = (Attachment *)lua_touserdata(pLuaState, -1);
    destroyFixedAttachmentPtr(pGfxContext, pAttachment);
    return 0;
}

static int luaCreateRenderPassPtr(lua_State *pLuaState)
{
    // Get parameters from Lua stack
    GfxContext *pGfxContext = (GfxContext *)lua_touserdata(pLuaState, -7);

    // Get VkAttachmentDescription array
    lua_len(pLuaState, -6);
    uint32_t attachmentCount = (uint32_t)lua_tointeger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    VkAttachmentDescription *vkAttachmentDescriptions = tknMalloc(sizeof(VkAttachmentDescription) * attachmentCount);
    for (uint32_t i = 0; i < attachmentCount; i++)
    {
        lua_rawgeti(pLuaState, -6, i + 1);
        VkAttachmentDescription attachmentDescription = {0};

        lua_getfield(pLuaState, -1, "flags");
        attachmentDescription.flags = lua_isnil(pLuaState, -1) ? 0 : (VkAttachmentDescriptionFlags)lua_tointeger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        lua_getfield(pLuaState, -1, "format");
        attachmentDescription.format = lua_isnil(pLuaState, -1) ? VK_FORMAT_UNDEFINED : (VkFormat)lua_tointeger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        lua_getfield(pLuaState, -1, "samples");
        attachmentDescription.samples = (VkSampleCountFlagBits)lua_tointeger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        lua_getfield(pLuaState, -1, "loadOp");
        attachmentDescription.loadOp = (VkAttachmentLoadOp)lua_tointeger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        lua_getfield(pLuaState, -1, "storeOp");
        attachmentDescription.storeOp = (VkAttachmentStoreOp)lua_tointeger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        lua_getfield(pLuaState, -1, "stencilLoadOp");
        attachmentDescription.stencilLoadOp = (VkAttachmentLoadOp)lua_tointeger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        lua_getfield(pLuaState, -1, "stencilStoreOp");
        attachmentDescription.stencilStoreOp = (VkAttachmentStoreOp)lua_tointeger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        lua_getfield(pLuaState, -1, "initialLayout");
        attachmentDescription.initialLayout = (VkImageLayout)lua_tointeger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        lua_getfield(pLuaState, -1, "finalLayout");
        attachmentDescription.finalLayout = (VkImageLayout)lua_tointeger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        vkAttachmentDescriptions[i] = attachmentDescription;
        lua_pop(pLuaState, 1);
    }

    // Get inputAttachmentPtrs array
    lua_len(pLuaState, -5);
    uint32_t inputAttachmentCount = (uint32_t)lua_tointeger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    Attachment **inputAttachmentPtrs = tknMalloc(sizeof(Attachment *) * inputAttachmentCount);
    for (uint32_t i = 0; i < inputAttachmentCount; i++)
    {
        lua_rawgeti(pLuaState, -5, i + 1);
        inputAttachmentPtrs[i] = (Attachment *)lua_touserdata(pLuaState, -1);
        lua_pop(pLuaState, 1);
    }

    // Get VkClearValue array
    lua_len(pLuaState, -4);
    uint32_t clearValueCount = (uint32_t)lua_tointeger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    VkClearValue *vkClearValues = tknMalloc(sizeof(VkClearValue) * clearValueCount);
    for (uint32_t i = 0; i < clearValueCount; i++)
    {
        lua_rawgeti(pLuaState, -4, i + 1);
        VkClearValue clearValue = {0};
        
        // Check if it's a depth/stencil clear value (has depth field)
        lua_getfield(pLuaState, -1, "depth");
        if (!lua_isnil(pLuaState, -1))
        {
            // This is a depth/stencil clear value
            clearValue.depthStencil.depth = (float)lua_tonumber(pLuaState, -1);
            lua_pop(pLuaState, 1);
            
            lua_getfield(pLuaState, -1, "stencil");
            clearValue.depthStencil.stencil = (uint32_t)lua_tointeger(pLuaState, -1);
            lua_pop(pLuaState, 1);
        }
        else
        {
            // This is a color clear value (array of 4 floats)
            lua_pop(pLuaState, 1);
            for (uint32_t j = 0; j < 4; j++)
            {
                lua_rawgeti(pLuaState, -1, j + 1);
                clearValue.color.float32[j] = (float)lua_tonumber(pLuaState, -1);
                lua_pop(pLuaState, 1);
            }
        }
        
        vkClearValues[i] = clearValue;
        lua_pop(pLuaState, 1);
    }

    // Get VkSubpassDescription array
    lua_len(pLuaState, -3);
    uint32_t subpassCount = (uint32_t)lua_tointeger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    VkSubpassDescription *vkSubpassDescriptions = tknMalloc(sizeof(VkSubpassDescription) * subpassCount);
    for (uint32_t i = 0; i < subpassCount; i++)
    {
        lua_rawgeti(pLuaState, -3, i + 1);
        VkSubpassDescription subpassDescription = {0};

        lua_getfield(pLuaState, -1, "pipelineBindPoint");
        subpassDescription.pipelineBindPoint = (VkPipelineBindPoint)lua_tointeger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        // Handle pInputAttachments
        lua_getfield(pLuaState, -1, "pInputAttachments");
        lua_len(pLuaState, -1);
        subpassDescription.inputAttachmentCount = (uint32_t)lua_tointeger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        if (subpassDescription.inputAttachmentCount > 0)
        {
            VkAttachmentReference *pInputAttachments = tknMalloc(sizeof(VkAttachmentReference) * subpassDescription.inputAttachmentCount);
            for (uint32_t j = 0; j < subpassDescription.inputAttachmentCount; j++)
            {
                lua_rawgeti(pLuaState, -1, j + 1);
                lua_getfield(pLuaState, -1, "attachment");
                pInputAttachments[j].attachment = (uint32_t)lua_tointeger(pLuaState, -1);
                lua_pop(pLuaState, 1);
                lua_getfield(pLuaState, -1, "layout");
                pInputAttachments[j].layout = (VkImageLayout)lua_tointeger(pLuaState, -1);
                lua_pop(pLuaState, 2);
            }
            subpassDescription.pInputAttachments = pInputAttachments;
        }
        lua_pop(pLuaState, 1);

        // Handle pColorAttachments
        lua_getfield(pLuaState, -1, "pColorAttachments");
        lua_len(pLuaState, -1);
        subpassDescription.colorAttachmentCount = (uint32_t)lua_tointeger(pLuaState, -1);
        lua_pop(pLuaState, 1);
        if (subpassDescription.colorAttachmentCount > 0)
        {
            VkAttachmentReference *pColorAttachments = tknMalloc(sizeof(VkAttachmentReference) * subpassDescription.colorAttachmentCount);
            for (uint32_t j = 0; j < subpassDescription.colorAttachmentCount; j++)
            {
                lua_rawgeti(pLuaState, -1, j + 1);
                lua_getfield(pLuaState, -1, "attachment");
                pColorAttachments[j].attachment = (uint32_t)lua_tointeger(pLuaState, -1);
                lua_pop(pLuaState, 1);
                lua_getfield(pLuaState, -1, "layout");
                pColorAttachments[j].layout = (VkImageLayout)lua_tointeger(pLuaState, -1);
                lua_pop(pLuaState, 2);
            }
            subpassDescription.pColorAttachments = pColorAttachments;
        }
        lua_pop(pLuaState, 1);

        // Handle pDepthStencilAttachment
        lua_getfield(pLuaState, -1, "pDepthStencilAttachment");
        if (!lua_isnil(pLuaState, -1))
        {
            VkAttachmentReference *pDepthStencilAttachment = tknMalloc(sizeof(VkAttachmentReference));
            lua_getfield(pLuaState, -1, "attachment");
            pDepthStencilAttachment->attachment = (uint32_t)lua_tointeger(pLuaState, -1);
            lua_pop(pLuaState, 1);
            lua_getfield(pLuaState, -1, "layout");
            pDepthStencilAttachment->layout = (VkImageLayout)lua_tointeger(pLuaState, -1);
            lua_pop(pLuaState, 1);
            subpassDescription.pDepthStencilAttachment = pDepthStencilAttachment;
        }
        lua_pop(pLuaState, 1);

        vkSubpassDescriptions[i] = subpassDescription;
        lua_pop(pLuaState, 1);
    }

    // Get spvPaths array (2D array)
    lua_len(pLuaState, -2);
    uint32_t spvPathsArrayCount = (uint32_t)lua_tointeger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    uint32_t *spvPathCounts = tknMalloc(sizeof(uint32_t) * spvPathsArrayCount);
    const char ***spvPathsArray = tknMalloc(sizeof(const char **) * spvPathsArrayCount);
    for (uint32_t i = 0; i < spvPathsArrayCount; i++)
    {
        lua_rawgeti(pLuaState, -2, i + 1);
        lua_len(pLuaState, -1);
        spvPathCounts[i] = (uint32_t)lua_tointeger(pLuaState, -1);
        lua_pop(pLuaState, 1);

        const char **spvPaths = tknMalloc(sizeof(const char *) * spvPathCounts[i]);
        for (uint32_t j = 0; j < spvPathCounts[i]; j++)
        {
            lua_rawgeti(pLuaState, -1, j + 1);
            spvPaths[j] = lua_tostring(pLuaState, -1);
            lua_pop(pLuaState, 1);
        }
        spvPathsArray[i] = spvPaths;
        lua_pop(pLuaState, 1);
    }

    // Get VkSubpassDependency array
    lua_len(pLuaState, -1);
    uint32_t vkSubpassDependencyCount = (uint32_t)lua_tointeger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    VkSubpassDependency *vkSubpassDependencies = NULL;
    if (vkSubpassDependencyCount > 0)
    {
        vkSubpassDependencies = tknMalloc(sizeof(VkSubpassDependency) * vkSubpassDependencyCount);
        for (uint32_t i = 0; i < vkSubpassDependencyCount; i++)
        {
            lua_rawgeti(pLuaState, -1, i + 1);
            VkSubpassDependency subpassDependency = {0};

            lua_getfield(pLuaState, -1, "srcSubpass");
            subpassDependency.srcSubpass = (uint32_t)lua_tointeger(pLuaState, -1);
            lua_pop(pLuaState, 1);

            lua_getfield(pLuaState, -1, "dstSubpass");
            subpassDependency.dstSubpass = (uint32_t)lua_tointeger(pLuaState, -1);
            lua_pop(pLuaState, 1);

            lua_getfield(pLuaState, -1, "srcStageMask");
            subpassDependency.srcStageMask = (VkPipelineStageFlags)lua_tointeger(pLuaState, -1);
            lua_pop(pLuaState, 1);

            lua_getfield(pLuaState, -1, "dstStageMask");
            subpassDependency.dstStageMask = (VkPipelineStageFlags)lua_tointeger(pLuaState, -1);
            lua_pop(pLuaState, 1);

            lua_getfield(pLuaState, -1, "srcAccessMask");
            subpassDependency.srcAccessMask = (VkAccessFlags)lua_tointeger(pLuaState, -1);
            lua_pop(pLuaState, 1);

            lua_getfield(pLuaState, -1, "dstAccessMask");
            subpassDependency.dstAccessMask = (VkAccessFlags)lua_tointeger(pLuaState, -1);
            lua_pop(pLuaState, 1);

            lua_getfield(pLuaState, -1, "dependencyFlags");
            subpassDependency.dependencyFlags = (VkDependencyFlags)lua_tointeger(pLuaState, -1);
            lua_pop(pLuaState, 1);

            vkSubpassDependencies[i] = subpassDependency;
            lua_pop(pLuaState, 1);
        }
    }

    // Get renderPassIndex
    uint32_t renderPassIndex = (uint32_t)lua_tointeger(pLuaState, -1);

    // Call the C function
    RenderPass *pRenderPass = createRenderPassPtr(pGfxContext, attachmentCount, vkAttachmentDescriptions,
                                                  inputAttachmentPtrs, vkClearValues, subpassCount, vkSubpassDescriptions,
                                                  spvPathCounts, spvPathsArray, vkSubpassDependencyCount,
                                                  vkSubpassDependencies);

    // Clean up memory
    tknFree(vkAttachmentDescriptions);
    tknFree(inputAttachmentPtrs);
    tknFree(vkClearValues);

    for (uint32_t i = 0; i < subpassCount; i++)
    {
        if (vkSubpassDescriptions[i].pInputAttachments)
            tknFree((void *)vkSubpassDescriptions[i].pInputAttachments);
        if (vkSubpassDescriptions[i].pColorAttachments)
            tknFree((void *)vkSubpassDescriptions[i].pColorAttachments);
        if (vkSubpassDescriptions[i].pDepthStencilAttachment)
            tknFree((void *)vkSubpassDescriptions[i].pDepthStencilAttachment);
    }
    tknFree(vkSubpassDescriptions);

    for (uint32_t i = 0; i < spvPathsArrayCount; i++)
    {
        tknFree(spvPathsArray[i]);
    }
    tknFree(spvPathCounts);
    tknFree(spvPathsArray);

    if (vkSubpassDependencies)
        tknFree(vkSubpassDependencies);

    // Return RenderPass as userdata
    lua_pushlightuserdata(pLuaState, pRenderPass);
    return 1;
}

static int luaDestroyRenderPassPtr(lua_State *pLuaState)
{
    GfxContext *pGfxContext = (GfxContext *)lua_touserdata(pLuaState, -2);
    RenderPass *pRenderPass = (RenderPass *)lua_touserdata(pLuaState, -1);
    destroyRenderPassPtr(pGfxContext, pRenderPass);
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
        {"createRenderPassPtr", luaCreateRenderPassPtr},
        {"destroyRenderPassPtr", luaDestroyRenderPassPtr},
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
