#include "luaBinding.h"
#include "tickernelVoxel.h"
#define LUA_PEEK_TOP(L)      \
    {                        \
        top = lua_gettop(L); \
    }
static void tryThrowLuaError(lua_State *pLuaState, int luaResult)
{
    if (LUA_OK == luaResult)
    {
        // continue.
    }
    else
    {
        const char *msg = lua_tostring(pLuaState, -1);
        lua_pop(pLuaState, 1);
        tickernelError("Lua error code: %d!\n   Error msg: %s\n", luaResult, msg);
    }
}

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

static int luaAddModelToOpaqueGeometrySubpass(lua_State *pLuaState)
{
    lua_len(pLuaState, -3);
    lua_Integer vertexCount = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    if (vertexCount > 0)
    {
        OpaqueGeometrySubpassVertex *opaqueGeometrySubpassVertices = tickernelMalloc(sizeof(OpaqueGeometrySubpassVertex) * vertexCount);
        for (uint32_t i = 0; i < vertexCount; i++)
        {
            int vertexType = lua_geti(pLuaState, -3, i + 1);
            assertLuaType(vertexType, LUA_TTABLE);
            uint32_t vertexLength = 3;
            for (uint32_t j = 0; j < vertexLength; j++)
            {
                int vertexValueType = lua_geti(pLuaState, -1, j + 1);
                assertLuaType(vertexValueType, LUA_TNUMBER);
                opaqueGeometrySubpassVertices[i].position[j] = luaL_checknumber(pLuaState, -1);
                lua_pop(pLuaState, 1);
            }
            lua_pop(pLuaState, 1);

            int colorType = lua_geti(pLuaState, -2, i + 1);
            assertLuaType(colorType, LUA_TTABLE);
            uint32_t colorLength = 4;
            for (uint32_t j = 0; j < colorLength; j++)
            {
                int colorValueType = lua_geti(pLuaState, -1, j + 1);
                assertLuaType(colorValueType, LUA_TNUMBER);
                opaqueGeometrySubpassVertices[i].color[j] = luaL_checknumber(pLuaState, -1) / 255.0f;
                lua_pop(pLuaState, 1);
            }
            lua_pop(pLuaState, 1);

            int normalType = lua_geti(pLuaState, -1, i + 1);
            assertLuaType(normalType, LUA_TTABLE);
            uint32_t normalLength = 3;
            for (uint32_t j = 0; j < normalLength; j++)
            {
                int normalValueType = lua_geti(pLuaState, -1, j + 1);
                assertLuaType(normalValueType, LUA_TNUMBER);
                opaqueGeometrySubpassVertices[i].normal[j] = luaL_checknumber(pLuaState, -1);
                lua_pop(pLuaState, 1);
            }
            lua_pop(pLuaState, 1);
        }
        lua_pop(pLuaState, 2);

        uint32_t outputIndex;

        int engineTpye = lua_getglobal(pLuaState, "engine");
        assertLuaType(engineTpye, LUA_TTABLE);
        int pGraphicContextTpye = lua_getfield(pLuaState, -1, "pGraphicContext");
        assertLuaType(pGraphicContextTpye, LUA_TLIGHTUSERDATA);
        GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
        lua_pop(pLuaState, 2);

        addModelToOpaqueGeometrySubpass(&pGraphicContext->deferredRenderPass.opaqueGeometrySubpass, pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, pGraphicContext->graphicVkCommandPool, pGraphicContext->vkGraphicQueue, pGraphicContext->globalUniformBuffer, (uint32_t)vertexCount, opaqueGeometrySubpassVertices, &outputIndex);

        tickernelFree(opaqueGeometrySubpassVertices);

        lua_pushinteger(pLuaState, outputIndex);

        return 1;
    }
    else
    {
        //  vertices colors
        lua_pop(pLuaState, 2);
        //
        return 0;
    }
}
static int luaRemoveModelFromOpaqueGeometrySubpass(lua_State *pLuaState)
{
    lua_Integer index = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);

    int engineTpye = lua_getglobal(pLuaState, "engine");
    assertLuaType(engineTpye, LUA_TTABLE);
    int pGraphicContextTpye = lua_getfield(pLuaState, -1, "pGraphicContext");
    assertLuaType(pGraphicContextTpye, LUA_TLIGHTUSERDATA);
    GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 2);

    removeModelFromOpaqueGeometrySubpass(&pGraphicContext->deferredRenderPass.opaqueGeometrySubpass, pGraphicContext->vkDevice, (uint32_t)index);
    return 0;
}
static int luaUpdateInstancesInOpaqueGeometrySubpass(lua_State *pLuaState)
{
    //  index modelMatrix

    lua_len(pLuaState, -1);
    lua_Integer instanceCount = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    OpaqueGeometrySubpassInstance instances[instanceCount];
    for (uint32_t i = 0; i < instanceCount; i++)
    {
        int instanceType = lua_geti(pLuaState, -1, i + 1);
        assertLuaType(instanceType, LUA_TTABLE);
        for (uint32_t row = 0; row < 4; row++)
        {
            int rowValueType = lua_geti(pLuaState, -1, row + 1);
            assertLuaType(rowValueType, LUA_TTABLE);
            for (uint32_t column = 0; column < 4; column++)
            {
                int columnValueType = lua_geti(pLuaState, -1, column + 1);
                assertLuaType(columnValueType, LUA_TNUMBER);
                instances[i].model[column][row] = luaL_checknumber(pLuaState, -1);
                lua_pop(pLuaState, 1);
            }
            lua_pop(pLuaState, 1);
        }
        lua_pop(pLuaState, 1);
    }
    lua_pop(pLuaState, 1);

    // index
    lua_Integer modelIndex = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);

    int engineTpye = lua_getglobal(pLuaState, "engine");
    assertLuaType(engineTpye, LUA_TTABLE);
    int pGraphicContextTpye = lua_getfield(pLuaState, -1, "pGraphicContext");
    assertLuaType(pGraphicContextTpye, LUA_TLIGHTUSERDATA);
    GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 2);
    updateInstancesInOpaqueGeometrySubpass(&pGraphicContext->deferredRenderPass.opaqueGeometrySubpass, (uint32_t)modelIndex, pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, pGraphicContext->graphicVkCommandPool, pGraphicContext->vkGraphicQueue, pGraphicContext->globalUniformBuffer, instances, (uint32_t)instanceCount);
    return 0;
}

static int luaAddModelToWaterGeometrySubpass(lua_State *pLuaState)
{
    lua_len(pLuaState, -2);
    //  vertices colors length

    lua_Integer vertexCount = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    //  vertices colors

    if (vertexCount > 0)
    {
        WaterGeometrySubpassVertex *waterGeometrySubpassVertices = tickernelMalloc(sizeof(WaterGeometrySubpassVertex) * vertexCount);
        for (uint32_t i = 0; i < vertexCount; i++)
        {
            int vertexType = lua_geti(pLuaState, -3, i + 1);
            assertLuaType(vertexType, LUA_TTABLE);
            uint32_t vertexLength = 3;
            for (uint32_t j = 0; j < vertexLength; j++)
            {
                int vertexValueType = lua_geti(pLuaState, -1, j + 1);
                assertLuaType(vertexValueType, LUA_TNUMBER);
                waterGeometrySubpassVertices[i].position[j] = luaL_checknumber(pLuaState, -1);
                lua_pop(pLuaState, 1);
            }
            lua_pop(pLuaState, 1);

            int colorType = lua_geti(pLuaState, -2, i + 1);
            assertLuaType(colorType, LUA_TTABLE);
            uint32_t colorLength = 4;
            for (uint32_t j = 0; j < colorLength; j++)
            {
                int colorValueType = lua_geti(pLuaState, -1, j + 1);
                assertLuaType(colorValueType, LUA_TNUMBER);
                waterGeometrySubpassVertices[i].color[j] = luaL_checknumber(pLuaState, -1) / 255.0f;
                lua_pop(pLuaState, 1);
            }
            lua_pop(pLuaState, 1);

            int normalType = lua_geti(pLuaState, -1, i + 1);
            assertLuaType(normalType, LUA_TTABLE);
            uint32_t normalLength = 3;
            for (uint32_t j = 0; j < normalLength; j++)
            {
                int normalValueType = lua_geti(pLuaState, -1, j + 1);
                assertLuaType(normalValueType, LUA_TNUMBER);
                waterGeometrySubpassVertices[i].normal[j] = luaL_checknumber(pLuaState, -1);
                lua_pop(pLuaState, 1);
            }
            lua_pop(pLuaState, 1);
        }
        lua_pop(pLuaState, 2);

        uint32_t outputIndex;

        int engineTpye = lua_getglobal(pLuaState, "engine");
        assertLuaType(engineTpye, LUA_TTABLE);
        int pGraphicContextTpye = lua_getfield(pLuaState, -1, "pGraphicContext");
        assertLuaType(pGraphicContextTpye, LUA_TLIGHTUSERDATA);
        GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
        lua_pop(pLuaState, 2);

        addModelToWaterGeometrySubpass(&pGraphicContext->deferredRenderPass.waterGeometrySubpass, pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, pGraphicContext->graphicVkCommandPool, pGraphicContext->vkGraphicQueue, pGraphicContext->globalUniformBuffer, (uint32_t)vertexCount, waterGeometrySubpassVertices, &outputIndex);

        tickernelFree(waterGeometrySubpassVertices);

        lua_pushinteger(pLuaState, outputIndex);

        return 1;
    }
    else
    {
        //  vertices colors
        lua_pop(pLuaState, 2);
        //
        return 0;
    }
}
static int luaRemoveModelFromWaterGeometrySubpass(lua_State *pLuaState)
{
    lua_Integer index = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);

    int engineTpye = lua_getglobal(pLuaState, "engine");
    assertLuaType(engineTpye, LUA_TTABLE);
    int pGraphicContextTpye = lua_getfield(pLuaState, -1, "pGraphicContext");
    assertLuaType(pGraphicContextTpye, LUA_TLIGHTUSERDATA);
    GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 2);

    removeModelFromWaterGeometrySubpass(&pGraphicContext->deferredRenderPass.waterGeometrySubpass, pGraphicContext->vkDevice, (uint32_t)index);
    return 0;
}
static int luaUpdateInstancesInWaterGeometrySubpass(lua_State *pLuaState)
{
    lua_len(pLuaState, -1);
    lua_Integer instanceCount = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    WaterGeometrySubpassInstance instances[instanceCount];
    for (uint32_t i = 0; i < instanceCount; i++)
    {
        int instanceType = lua_geti(pLuaState, -1, i + 1);
        assertLuaType(instanceType, LUA_TTABLE);
        for (uint32_t row = 0; row < 4; row++)
        {
            int rowValueType = lua_geti(pLuaState, -1, row + 1);
            assertLuaType(rowValueType, LUA_TTABLE);
            for (uint32_t column = 0; column < 4; column++)
            {
                int columnValueType = lua_geti(pLuaState, -1, column + 1);
                assertLuaType(columnValueType, LUA_TNUMBER);
                instances[i].model[column][row] = luaL_checknumber(pLuaState, -1);
                lua_pop(pLuaState, 1);
            }
            lua_pop(pLuaState, 1);
        }
        lua_pop(pLuaState, 1);
    }
    lua_pop(pLuaState, 1);

    // index
    lua_Integer modelIndex = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);

    int engineTpye = lua_getglobal(pLuaState, "engine");
    assertLuaType(engineTpye, LUA_TTABLE);
    int pGraphicContextTpye = lua_getfield(pLuaState, -1, "pGraphicContext");
    assertLuaType(pGraphicContextTpye, LUA_TLIGHTUSERDATA);
    GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 2);
    updateInstancesInWaterGeometrySubpass(&pGraphicContext->deferredRenderPass.waterGeometrySubpass, (uint32_t)modelIndex, pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, pGraphicContext->graphicVkCommandPool, pGraphicContext->vkGraphicQueue, pGraphicContext->globalUniformBuffer, instances, (uint32_t)instanceCount);
    return 0;
}

static int updateGlobalUniformBuffer(lua_State *pLuaState)
{
    int engineTpye = lua_getglobal(pLuaState, "engine");
    assertLuaType(engineTpye, LUA_TTABLE);
    int pGraphicContextTpye = lua_getfield(pLuaState, -1, "pGraphicContext");
    assertLuaType(pGraphicContextTpye, LUA_TLIGHTUSERDATA);
    GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 2);

    lua_Number time = luaL_checknumber(pLuaState, -1);
    lua_pop(pLuaState, 1);

    vec3 targetPosition;
    vec3 cameraPosition;
    for (uint32_t i = 0; i < 3; i++)
    {
        int targetPositionValueType = lua_geti(pLuaState, -1, i + 1);
        assertLuaType(targetPositionValueType, LUA_TNUMBER);
        targetPosition[i] = luaL_checknumber(pLuaState, -1);
        lua_pop(pLuaState, 1);

        int cameraPositionValueType = lua_geti(pLuaState, -2, i + 1);
        assertLuaType(cameraPositionValueType, LUA_TNUMBER);
        cameraPosition[i] = luaL_checknumber(pLuaState, -1);
        lua_pop(pLuaState, 1);
    }

    GlobalUniformBuffer ubo;
    glm_lookat(cameraPosition, targetPosition, (vec3){0.0f, 0.0f, 1.0f}, ubo.view);
    float deg = 30.0f;
    ubo.pointSizeFactor = 0.618 * pGraphicContext->swapchainHeight / tanf(glm_rad(deg / 2)) / 16.0;
    glm_perspective(glm_rad(deg), pGraphicContext->swapchainWidth / (float)pGraphicContext->swapchainHeight, 8.0f, 32.0f, ubo.proj);
    ubo.proj[1][1] *= -1;
    mat4 view_proj;
    glm_mat4_mul(ubo.proj, ubo.view, view_proj);
    glm_mat4_inv(view_proj, ubo.inv_view_proj);
    ubo.time = time;
    lua_pop(pLuaState, 2);
    pGraphicContext->inputGlobalUniformBuffer = ubo;
    pGraphicContext->canUpdateGlobalUniformBuffer = true;
    return 0;
}

static int updateLightsUniformBuffer(lua_State *pLuaState)
{
    int engineTpye = lua_getglobal(pLuaState, "engine");
    assertLuaType(engineTpye, LUA_TTABLE);
    int pGraphicContextTpye = lua_getfield(pLuaState, -1, "pGraphicContext");
    assertLuaType(pGraphicContextTpye, LUA_TLIGHTUSERDATA);
    GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 2);

    LightsUniformBuffer lightsUniformBuffer;

    lua_len(pLuaState, -1);
    lua_Integer pointLightCount = luaL_checkinteger(pLuaState, -1);
    lightsUniformBuffer.pointLightCount = (int)pointLightCount;
    if (pointLightCount > MAX_POINT_LIGHT_COUNT)
    {
        tickernelError("Point light count: %d out of range!\n", (int)pointLightCount);
    }

    lua_pop(pLuaState, 1);
    for (uint32_t i = 0; i < pointLightCount; i++)
    {
        int pointLightsType = lua_geti(pLuaState, -1, i + 1);
        assertLuaType(pointLightsType, LUA_TTABLE);
        {
            int colorType = lua_getfield(pLuaState, -1, "color");
            assertLuaType(colorType, LUA_TTABLE);
            for (uint32_t j = 0; j < 4; j++)
            {
                int colorValueType = lua_geti(pLuaState, -1, j + 1);
                assertLuaType(colorValueType, LUA_TNUMBER);
                lightsUniformBuffer.pointLights[i].color[j] = luaL_checknumber(pLuaState, -1);
                lua_pop(pLuaState, 1);
            }
            lua_pop(pLuaState, 1);

            int positionType = lua_getfield(pLuaState, -1, "position");
            assertLuaType(positionType, LUA_TTABLE);
            for (uint32_t j = 0; j < 3; j++)
            {
                int positionValueType = lua_geti(pLuaState, -1, j + 1);
                assertLuaType(positionValueType, LUA_TNUMBER);
                lightsUniformBuffer.pointLights[i].position[j] = luaL_checknumber(pLuaState, -1);
                lua_pop(pLuaState, 1);
            }
            lua_pop(pLuaState, 1);

            int rangeType = lua_getfield(pLuaState, -1, "range");
            assertLuaType(rangeType, LUA_TNUMBER);
            lightsUniformBuffer.pointLights[i].range = luaL_checknumber(pLuaState, -1);
            lua_pop(pLuaState, 1);
        }
        lua_pop(pLuaState, 1);
    }
    lua_pop(pLuaState, 1);

    {
        int colorType = lua_getfield(pLuaState, -1, "color");
        assertLuaType(colorType, LUA_TTABLE);
        for (uint32_t i = 0; i < 4; i++)
        {
            int colorValueType = lua_geti(pLuaState, -1, i + 1);
            assertLuaType(colorValueType, LUA_TNUMBER);
            lightsUniformBuffer.directionalLight.color[i] = luaL_checknumber(pLuaState, -1);
            lua_pop(pLuaState, 1);
        }
        lua_pop(pLuaState, 1);

        int directionType = lua_getfield(pLuaState, -1, "direction");
        assertLuaType(directionType, LUA_TTABLE);
        for (uint32_t i = 0; i < 3; i++)
        {
            int directionValueType = lua_geti(pLuaState, -1, i + 1);
            assertLuaType(directionValueType, LUA_TNUMBER);
            lightsUniformBuffer.directionalLight.direction[i] = luaL_checknumber(pLuaState, -1);
            lua_pop(pLuaState, 1);
        }
        lua_pop(pLuaState, 1);
    }
    lua_pop(pLuaState, 1);
    pGraphicContext->inputLightsUniformBuffer = lightsUniformBuffer;
    pGraphicContext->canUpdateLightsUniformBuffer = true;
    // assert(lua_gettop(pLuaState) == 0);
    return 0;
}

static int loadModel(lua_State *pLuaState)
{
    const char *path = luaL_checkstring(pLuaState, -1);
    lua_pop(pLuaState, 1);
    TickernelVoxel *pTickernelVoxel = tickernelMalloc(sizeof(TickernelVoxel));
    deserializeTickernelVoxel(path, pTickernelVoxel, tickernelMalloc);

    lua_newtable(pLuaState);
    lua_pushstring(pLuaState, "vertexCount");
    lua_pushinteger(pLuaState, pTickernelVoxel->vertexCount);
    lua_settable(pLuaState, -3);

    lua_pushstring(pLuaState, "propertyCount");
    lua_pushinteger(pLuaState, pTickernelVoxel->propertyCount);
    lua_settable(pLuaState, -3);

    lua_pushstring(pLuaState, "names");
    lua_newtable(pLuaState);
    for (uint32_t i = 0; i < pTickernelVoxel->propertyCount; i++)
    {
        lua_pushinteger(pLuaState, i + 1);
        lua_pushstring(pLuaState, pTickernelVoxel->names[i]);
        lua_settable(pLuaState, -3);
    }
    lua_settable(pLuaState, -3);

    lua_pushstring(pLuaState, "types");
    lua_newtable(pLuaState);
    for (uint32_t i = 0; i < pTickernelVoxel->propertyCount; i++)
    {
        lua_pushinteger(pLuaState, i + 1);
        lua_pushinteger(pLuaState, pTickernelVoxel->types[i]);
        lua_settable(pLuaState, -3);
    }
    lua_settable(pLuaState, -3);

    lua_pushstring(pLuaState, "indexToProperties");
    lua_newtable(pLuaState);
    for (uint32_t i = 0; i < pTickernelVoxel->propertyCount; i++)
    {
        lua_pushinteger(pLuaState, i + 1);
        lua_newtable(pLuaState);
        for (uint32_t j = 0; j < pTickernelVoxel->vertexCount; j++)
        {
            lua_pushinteger(pLuaState, j + 1);

            switch (pTickernelVoxel->types[i])
            {
            case TICKERNEL_VOXEL_INT8:
                lua_pushinteger(pLuaState, ((int8_t *)pTickernelVoxel->indexToProperties[i])[j]);
                break;
            case TICKERNEL_VOXEL_UINT8:
                lua_pushinteger(pLuaState, ((uint8_t *)pTickernelVoxel->indexToProperties[i])[j]);
                break;
            case TICKERNEL_VOXEL_INT16:
                lua_pushinteger(pLuaState, ((int16_t *)pTickernelVoxel->indexToProperties[i])[j]);
                break;
            case TICKERNEL_VOXEL_UINT16:
                lua_pushinteger(pLuaState, ((uint16_t *)pTickernelVoxel->indexToProperties[i])[j]);
                break;
            case TICKERNEL_VOXEL_INT32:
                lua_pushinteger(pLuaState, ((int32_t *)pTickernelVoxel->indexToProperties[i])[j]);
                break;
            case TICKERNEL_VOXEL_UINT32:
                lua_pushinteger(pLuaState, ((uint32_t *)pTickernelVoxel->indexToProperties[i])[j]);
                break;
            case TICKERNEL_VOXEL_FLOAT32:
                lua_pushnumber(pLuaState, ((float *)pTickernelVoxel->indexToProperties[i])[j]);
                break;
            default:
                tickernelError("Unknown property type %d", pTickernelVoxel->types[i]);
                break;
            }

            lua_settable(pLuaState, -3);
        }
        lua_settable(pLuaState, -3);
    }
    lua_settable(pLuaState, -3);
    releaseTickernelVoxel(pTickernelVoxel, tickernelFree);
    tickernelFree(pTickernelVoxel);
    return 1;
}

LuaContext *startLua(const char *assetPath, GraphicContext *pGraphicContext)
{
    LuaContext *pLuaContext = tickernelMalloc(sizeof(LuaContext));
    pLuaContext->assetPath = assetPath;
    pLuaContext->pGraphicContext = pGraphicContext;
    // New lua state
    pLuaContext->pLuaState = luaL_newstate();
    lua_State *pLuaState = pLuaContext->pLuaState;
    luaL_openlibs(pLuaState);

    // Set package path
    char packagePath[FILENAME_MAX];
    strcpy(packagePath, pLuaContext->assetPath);
    tickernelCombinePaths(packagePath, FILENAME_MAX, "?.lua;");
    lua_getglobal(pLuaState, "package");
    lua_pushstring(pLuaState, packagePath);
    lua_setfield(pLuaState, -2, "path");
    lua_pop(pLuaState, 1);

    // Do file main.lua
    char luaMainFilePath[FILENAME_MAX];
    strcpy(luaMainFilePath, pLuaContext->assetPath);
    tickernelCombinePaths(luaMainFilePath, FILENAME_MAX, "main.lua");
    // Put engine state on the stack
    int luaResult = luaL_dofile(pLuaState, luaMainFilePath);
    tryThrowLuaError(pLuaState, luaResult);

    lua_pushlightuserdata(pLuaState, pLuaContext->pGraphicContext);
    lua_setfield(pLuaState, -2, "pGraphicContext");

    lua_pushstring(pLuaState, pLuaContext->assetPath);
    lua_setfield(pLuaState, -2, "assetsPath");

    lua_pushstring(pLuaState, tickernelGetPathSeparator());
    lua_setfield(pLuaState, -2, "pathSeparator");

    lua_pushcfunction(pLuaState, luaAddModelToOpaqueGeometrySubpass);
    lua_setfield(pLuaState, -2, "AddModelToOpaqueGeometrySubpass");

    lua_pushcfunction(pLuaState, luaRemoveModelFromOpaqueGeometrySubpass);
    lua_setfield(pLuaState, -2, "RemoveModelFromOpaqueGeometrySubpass");

    lua_pushcfunction(pLuaState, luaUpdateInstancesInOpaqueGeometrySubpass);
    lua_setfield(pLuaState, -2, "UpdateInstancesInOpaqueGeometrySubpass");

    lua_pushcfunction(pLuaState, luaAddModelToWaterGeometrySubpass);
    lua_setfield(pLuaState, -2, "AddModelToWaterGeometrySubpass");

    lua_pushcfunction(pLuaState, luaRemoveModelFromWaterGeometrySubpass);
    lua_setfield(pLuaState, -2, "RemoveModelFromWaterGeometrySubpass");

    lua_pushcfunction(pLuaState, luaUpdateInstancesInWaterGeometrySubpass);
    lua_setfield(pLuaState, -2, "UpdateInstancesInWaterGeometrySubpass");

    lua_pushcfunction(pLuaState, updateGlobalUniformBuffer);
    lua_setfield(pLuaState, -2, "UpdateGlobalUniformBuffer");

    lua_pushcfunction(pLuaState, updateLightsUniformBuffer);
    lua_setfield(pLuaState, -2, "UpdateLightsUniformBuffer");

    lua_pushcfunction(pLuaState, loadModel);
    lua_setfield(pLuaState, -2, "LoadModel");

    // Call start
    int startFunctionType = lua_getfield(pLuaState, -1, "Start");
    assertLuaType(startFunctionType, LUA_TFUNCTION);
    luaResult = lua_pcall(pLuaState, 0, 0, 0);
    tryThrowLuaError(pLuaState, luaResult);
    return pLuaContext;
}

void updateLua(LuaContext *pLuaContext)
{
    lua_State *pLuaState = pLuaContext->pLuaState;

    int startFunctionType = lua_getfield(pLuaState, -1, "Update");
    assertLuaType(startFunctionType, LUA_TFUNCTION);
    int luaResult = lua_pcall(pLuaState, 0, 0, 0);
    tryThrowLuaError(pLuaState, luaResult);
}

void endLua(LuaContext *pLuaContext)
{
    // Call End
    lua_State *pLuaState = pLuaContext->pLuaState;
    int startFunctionType = lua_getfield(pLuaState, -1, "End");
    assertLuaType(startFunctionType, LUA_TFUNCTION);
    int luaResult = lua_pcall(pLuaState, 0, 0, 0);
    tryThrowLuaError(pLuaState, luaResult);

    //  Pop engine state off the stack
    lua_pop(pLuaState, 1);

    lua_close(pLuaContext->pLuaState);
    tickernelFree(pLuaContext);
}
