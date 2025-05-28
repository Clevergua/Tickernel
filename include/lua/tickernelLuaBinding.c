#include "tickernelLuaBinding.h"

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

int luaAddModelToGeometrySubpass(lua_State *pLuaState)
{
    lua_len(pLuaState, -3);
    lua_Integer vertexCount = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    if (vertexCount > 0)
    {
        GeometrySubpassVertex *geometrySubpassVertices = tickernelMalloc(sizeof(GeometrySubpassVertex) * vertexCount);
        for (uint32_t i = 0; i < vertexCount; i++)
        {
            int vertexType = lua_geti(pLuaState, -3, i + 1);
            assertLuaType(vertexType, LUA_TTABLE);
            uint32_t vertexLength = 3;
            for (uint32_t j = 0; j < vertexLength; j++)
            {
                int vertexValueType = lua_geti(pLuaState, -1, j + 1);
                assertLuaType(vertexValueType, LUA_TNUMBER);
                geometrySubpassVertices[i].position[j] = luaL_checknumber(pLuaState, -1);
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
                geometrySubpassVertices[i].color[j] = luaL_checknumber(pLuaState, -1) / 255.0f;
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
                geometrySubpassVertices[i].normal[j] = luaL_checknumber(pLuaState, -1);
                lua_pop(pLuaState, 1);
            }
            lua_pop(pLuaState, 1);
        }
        lua_pop(pLuaState, 2);
        
        int pGraphicContextType = lua_getfield(pLuaState, LUA_REGISTRYINDEX, "pGraphicContext");
        assertLuaType(pGraphicContextType, LUA_TLIGHTUSERDATA);
        GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
        lua_pop(pLuaState, 1);
        
        SubpassModel *pSubpassModel = addModelToGeometrySubpass(&pGraphicContext->deferredRenderPass.geometrySubpass, pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, pGraphicContext->graphicVkCommandPool, pGraphicContext->vkGraphicQueue, pGraphicContext->globalUniformBuffer, (uint32_t)vertexCount, geometrySubpassVertices);
        
        tickernelFree(geometrySubpassVertices);
        
        lua_pushlightuserdata(pLuaState, pSubpassModel);
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
int luaRemoveModelFromGeometrySubpass(lua_State *pLuaState)
{
    SubpassModel* pSubpassModel = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 1);
    
    int pGraphicContextType = lua_getfield(pLuaState, LUA_REGISTRYINDEX, "pGraphicContext");
    assertLuaType(pGraphicContextType, LUA_TLIGHTUSERDATA);
    GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 1);
    
    removeModelFromGeometrySubpass(&pGraphicContext->deferredRenderPass.geometrySubpass, pGraphicContext->vkDevice, pSubpassModel);
    return 0;
}
int luaUpdateInstancesInGeometrySubpass(lua_State *pLuaState)
{
    //  index modelMatrix
    
    lua_len(pLuaState, -1);
    lua_Integer instanceCount = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    GeometrySubpassInstance instances[instanceCount];
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
    SubpassModel* pSubpassModel = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 1);
    
    int pGraphicContextType = lua_getfield(pLuaState, LUA_REGISTRYINDEX, "pGraphicContext");
    assertLuaType(pGraphicContextType, LUA_TLIGHTUSERDATA);
    GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 1);
    
    updateInstancesInGeometrySubpass(&pGraphicContext->deferredRenderPass.geometrySubpass, pSubpassModel, pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, pGraphicContext->graphicVkCommandPool, pGraphicContext->vkGraphicQueue, pGraphicContext->globalUniformBuffer, instances, (uint32_t)instanceCount);
    return 0;
}

int luaAddModelToWaterGeometrySubpass(lua_State *pLuaState)
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
        
        int pGraphicContextType = lua_getfield(pLuaState, LUA_REGISTRYINDEX, "pGraphicContext");
        assertLuaType(pGraphicContextType, LUA_TLIGHTUSERDATA);
        GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
        lua_pop(pLuaState, 1);
        
        SubpassModel *pSubpassModel = addModelToWaterGeometrySubpass(&pGraphicContext->deferredRenderPass.waterGeometrySubpass, pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, pGraphicContext->graphicVkCommandPool, pGraphicContext->vkGraphicQueue, pGraphicContext->globalUniformBuffer, (uint32_t)vertexCount, waterGeometrySubpassVertices);
        
        tickernelFree(waterGeometrySubpassVertices);
        
        lua_pushlightuserdata(pLuaState, pSubpassModel);
        
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
int luaRemoveModelFromWaterGeometrySubpass(lua_State *pLuaState)
{
    SubpassModel* pSubpassModel = lua_touserdata (pLuaState, -1);
    lua_pop(pLuaState, 1);
    
    int pGraphicContextType = lua_getfield(pLuaState, LUA_REGISTRYINDEX, "pGraphicContext");
    assertLuaType(pGraphicContextType, LUA_TLIGHTUSERDATA);
    GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 1);
    
    removeModelFromWaterGeometrySubpass(&pGraphicContext->deferredRenderPass.waterGeometrySubpass, pGraphicContext->vkDevice, pSubpassModel);
    return 0;
}
int luaUpdateInstancesInWaterGeometrySubpass(lua_State *pLuaState)
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
    SubpassModel* pSubpassModel = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 1);
    
    int pGraphicContextType = lua_getfield(pLuaState, LUA_REGISTRYINDEX, "pGraphicContext");
    assertLuaType(pGraphicContextType, LUA_TLIGHTUSERDATA);
    GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 1);
    
    updateInstancesInWaterGeometrySubpass(&pGraphicContext->deferredRenderPass.waterGeometrySubpass, pSubpassModel, pGraphicContext->vkDevice, pGraphicContext->vkPhysicalDevice, pGraphicContext->graphicVkCommandPool, pGraphicContext->vkGraphicQueue, pGraphicContext->globalUniformBuffer, instances, (uint32_t)instanceCount);
    return 0;
}

int luaUpdateGlobalUniformBuffer(lua_State *pLuaState)
{
    int pGraphicContextType = lua_getfield(pLuaState, LUA_REGISTRYINDEX, "pGraphicContext");
    assertLuaType(pGraphicContextType, LUA_TLIGHTUSERDATA);
    GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 1);
    
    lua_getfield(pLuaState, -1, "time");
    lua_Number time = luaL_checknumber(pLuaState, -1);
    lua_pop(pLuaState, 1);
    
    lua_getfield(pLuaState, -1, "frameCount");
    uint32_t frameCount = luaL_checknumber(pLuaState, -1);
    lua_pop(pLuaState, 1);
    
    lua_getfield(pLuaState, -1, "cameraPosition");
    vec3 cameraPosition;
    for (uint32_t i = 0; i < 3; i++)
    {
        lua_geti(pLuaState, -1, i + 1);
        cameraPosition[i] = luaL_checknumber(pLuaState, -1);
        lua_pop(pLuaState, 1);
    }
    lua_pop(pLuaState, 1);
    
    lua_getfield(pLuaState, -1, "cameraRotation");
    vec3 cameraRotation;
    for (uint32_t i = 0; i < 3; i++)
    {
        lua_geti(pLuaState, -1, i + 1);
        cameraRotation[i] = luaL_checknumber(pLuaState, -1);
        lua_pop(pLuaState, 1);
    }
    lua_pop(pLuaState, 1);
    
    lua_getfield(pLuaState, -1, "fov");
    float fov = luaL_checknumber(pLuaState, -1);
    lua_pop(pLuaState, 1);
    
    lua_getfield(pLuaState, -1, "near");
    float near = luaL_checknumber(pLuaState, -1);
    lua_pop(pLuaState, 1);
    
    lua_getfield(pLuaState, -1, "far");
    float far = luaL_checknumber(pLuaState, -1);
    lua_pop(pLuaState, 1);
    
    lua_getfield(pLuaState, -1, "pointSizeScale");
    float pointSizeScale = luaL_checknumber(pLuaState, -1);
    lua_pop(pLuaState, 1);
    
    pGraphicContext->inputGlobalUniformBuffer.time = time;
    pGraphicContext->inputGlobalUniformBuffer.frameCount = frameCount;
    pGraphicContext->inputGlobalUniformBuffer.near = near;
    pGraphicContext->inputGlobalUniformBuffer.far = far;
    pGraphicContext->inputGlobalUniformBuffer.fov = fov;
    pGraphicContext->inputGlobalUniformBuffer.width = pGraphicContext->swapchainWidth;
    pGraphicContext->inputGlobalUniformBuffer.height = pGraphicContext->swapchainHeight;
    
    mat4 rotationMatrix;
    glm_euler_xyz(cameraRotation, rotationMatrix);
    
    mat4 translationMatrix;
    glm_translate_make(translationMatrix, cameraPosition);
    
    mat4 viewMatrix;
    glm_mat4_mul(translationMatrix, rotationMatrix, viewMatrix);
    glm_mat4_inv(viewMatrix, pGraphicContext->inputGlobalUniformBuffer.view);
    
    pGraphicContext->inputGlobalUniformBuffer.pointSizeFactor = pointSizeScale * pGraphicContext->swapchainHeight / tanf(glm_rad(fov / 2)) / 16.0;
    glm_perspective(glm_rad(fov), pGraphicContext->swapchainWidth / (float)pGraphicContext->swapchainHeight, near, far, pGraphicContext->inputGlobalUniformBuffer.proj);
    pGraphicContext->inputGlobalUniformBuffer.proj[1][1] *= -1;
    mat4 view_proj;
    glm_mat4_mul(pGraphicContext->inputGlobalUniformBuffer.proj, pGraphicContext->inputGlobalUniformBuffer.view, view_proj);
    glm_mat4_inv(view_proj, pGraphicContext->inputGlobalUniformBuffer.inv_view_proj);
    
    lua_pop(pLuaState, 2);
    pGraphicContext->canUpdateGlobalUniformBuffer = true;
    return 0;
}

int luaUpdateLightsUniformBuffer(lua_State *pLuaState)
{
    int pGraphicContextType = lua_getfield(pLuaState, LUA_REGISTRYINDEX, "pGraphicContext");
    assertLuaType(pGraphicContextType, LUA_TLIGHTUSERDATA);
    GraphicContext *pGraphicContext = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 1);
    
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

int luaLoadModel(lua_State *pLuaState)
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
