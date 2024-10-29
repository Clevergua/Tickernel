#include <luaEngine.h>
#include <tickernelVoxel.h>
static int top;
#define LUA_PEEK_TOP(L)      \
    {                        \
        top = lua_gettop(L); \
    }
static void TryThrowLuaError(lua_State *pLuaState, int luaResult)
{
    if (LUA_OK == luaResult)
    {
        // continue.
    }
    else
    {
        const char *msg = lua_tostring(pLuaState, -1);
        lua_pop(pLuaState, 1);
        TickernelError("Lua error code: %d!\n   Error msg: %s\n", luaResult, msg);
    }
}

static void AssertLuaType(int type, int targetType)
{
    if (type == targetType)
    {
        // continue.
    }
    else
    {
        TickernelError("Lua type error: type:%d ,target:%d!\n", type, targetType);
    }
}

static int AddModel(lua_State *pLuaState)
{
    lua_len(pLuaState, -2);
    //  vertices colors length

    uint32_t vertexCount = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);
    //  vertices colors

    if (vertexCount > 0)
    {
        GeometrySubpassVertex *geometrySubpassVertices = TickernelMalloc(sizeof(GeometrySubpassVertex) * vertexCount);
        for (uint32_t i = 0; i < vertexCount; i++)
        {
            int vertexType = lua_geti(pLuaState, -3, i + 1);
            AssertLuaType(vertexType, LUA_TTABLE);
            uint32_t vertexLength = 3;
            for (uint32_t j = 0; j < vertexLength; j++)
            {
                int vertexValueType = lua_geti(pLuaState, -1, j + 1);
                AssertLuaType(vertexValueType, LUA_TNUMBER);
                geometrySubpassVertices[i].position[j] = luaL_checknumber(pLuaState, -1);
                lua_pop(pLuaState, 1);
            }
            lua_pop(pLuaState, 1);

            int colorType = lua_geti(pLuaState, -2, i + 1);
            AssertLuaType(colorType, LUA_TTABLE);
            uint32_t colorLength = 4;
            for (uint32_t j = 0; j < colorLength; j++)
            {
                int colorValueType = lua_geti(pLuaState, -1, j + 1);
                AssertLuaType(colorValueType, LUA_TNUMBER);
                geometrySubpassVertices[i].color[j] = luaL_checknumber(pLuaState, -1) / 255.0f;
                lua_pop(pLuaState, 1);
            }
            lua_pop(pLuaState, 1);

            int normalType = lua_geti(pLuaState, -1, i + 1);
            AssertLuaType(normalType, LUA_TTABLE);
            uint32_t normalLength = 3;
            for (uint32_t j = 0; j < normalLength; j++)
            {
                int normalValueType = lua_geti(pLuaState, -1, j + 1);
                AssertLuaType(normalValueType, LUA_TNUMBER);
                geometrySubpassVertices[i].normal[j] = luaL_checknumber(pLuaState, -1);
                lua_pop(pLuaState, 1);
            }
            lua_pop(pLuaState, 1);
        }
        lua_pop(pLuaState, 2);

        uint32_t outputIndex;

        int gameStateTpye = lua_getglobal(pLuaState, "gameState");
        AssertLuaType(gameStateTpye, LUA_TTABLE);
        int pGraphicEngineTpye = lua_getfield(pLuaState, -1, "pGraphicEngine");
        AssertLuaType(pGraphicEngineTpye, LUA_TLIGHTUSERDATA);
        GraphicEngine *pGraphicEngine = lua_touserdata(pLuaState, -1);
        lua_pop(pLuaState, 2);

        AddModelToGeometrySubpass(&pGraphicEngine->deferredRenderPass.subpasses[0], pGraphicEngine->vkDevice, pGraphicEngine->vkPhysicalDevice, pGraphicEngine->graphicVkCommandPool, pGraphicEngine->vkGraphicQueue, pGraphicEngine->globalUniformBuffer, vertexCount, geometrySubpassVertices, &outputIndex);

        TickernelFree(geometrySubpassVertices);

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

static int RemoveModel(lua_State *pLuaState)
{
    uint32_t index = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);

    int gameStateTpye = lua_getglobal(pLuaState, "gameState");
    AssertLuaType(gameStateTpye, LUA_TTABLE);
    int pGraphicEngineTpye = lua_getfield(pLuaState, -1, "pGraphicEngine");
    AssertLuaType(pGraphicEngineTpye, LUA_TLIGHTUSERDATA);
    GraphicEngine *pGraphicEngine = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 2);

    RemoveModelFromGeometrySubpass(&pGraphicEngine->deferredRenderPass.subpasses[0], pGraphicEngine->vkDevice, index);
    return 0;
}

static int UpdateModelUniformBuffer(lua_State *pLuaState)
{
    //  index modelMatrix
    GeometrySubpassModelUniformBuffer buffer;
    for (uint32_t row = 0; row < 4; row++)
    {
        int rowValueType = lua_geti(pLuaState, -1, row + 1);
        //  index modelMatrix column
        AssertLuaType(rowValueType, LUA_TTABLE);
        for (uint32_t column = 0; column < 4; column++)
        {
            //  index modelMatrix column value
            int columnValueType = lua_geti(pLuaState, -1, column + 1);
            AssertLuaType(columnValueType, LUA_TNUMBER);
            buffer.model[column][row] = luaL_checknumber(pLuaState, -1);
            lua_pop(pLuaState, 1);
            //  index modelMatrix column
        }
        lua_pop(pLuaState, 1);
        //  index modelMatrix
    }

    lua_pop(pLuaState, 1);
    // index
    uint32_t index = luaL_checkinteger(pLuaState, -1);
    lua_pop(pLuaState, 1);

    int gameStateTpye = lua_getglobal(pLuaState, "gameState");
    AssertLuaType(gameStateTpye, LUA_TTABLE);
    int pGraphicEngineTpye = lua_getfield(pLuaState, -1, "pGraphicEngine");
    AssertLuaType(pGraphicEngineTpye, LUA_TLIGHTUSERDATA);
    GraphicEngine *pGraphicEngine = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 2);
    UpdateModelUniformToGeometrySubpass(&pGraphicEngine->deferredRenderPass.subpasses[0], index, buffer);
    return 0;
}

static int SetCamera(lua_State *pLuaState)
{
    int gameStateTpye = lua_getglobal(pLuaState, "gameState");
    AssertLuaType(gameStateTpye, LUA_TTABLE);
    int pGraphicEngineTpye = lua_getfield(pLuaState, -1, "pGraphicEngine");
    AssertLuaType(pGraphicEngineTpye, LUA_TLIGHTUSERDATA);
    GraphicEngine *pGraphicEngine = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 2);

    for (uint32_t i = 0; i < 3; i++)
    {
        int targetPositionValueType = lua_geti(pLuaState, -1, i + 1);
        AssertLuaType(targetPositionValueType, LUA_TNUMBER);
        pGraphicEngine->targetPosition[i] = luaL_checknumber(pLuaState, -1);
        lua_pop(pLuaState, 1);

        int cameraPositionValueType = lua_geti(pLuaState, -2, i + 1);
        AssertLuaType(cameraPositionValueType, LUA_TNUMBER);
        pGraphicEngine->cameraPosition[i] = luaL_checknumber(pLuaState, -1);
        lua_pop(pLuaState, 1);
    }
    lua_pop(pLuaState, 2);

    return 0;
}

static int LoadModel(lua_State *pLuaState)
{
    const char *path = luaL_checkstring(pLuaState, -1);
    lua_pop(pLuaState, 1);
    TickernelVoxel *pTickernelVoxel = TickernelMalloc(sizeof(TickernelVoxel));
    DeserializeTickernelVoxel(path, pTickernelVoxel, TickernelMalloc);

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
                TickernelError("Unknown property type %d", pTickernelVoxel->types[i]);
                break;
            }

            lua_settable(pLuaState, -3);
        }
        lua_settable(pLuaState, -3);
    }
    lua_settable(pLuaState, -3);
    ReleaseTickernelVoxel(pTickernelVoxel, TickernelFree);
    TickernelFree(pTickernelVoxel);
    return 1;
}

void StartLua(LuaEngine *pLuaEngine)
{
    // New lua state
    pLuaEngine->pLuaState = luaL_newstate();
    lua_State *pLuaState = pLuaEngine->pLuaState;
    luaL_openlibs(pLuaState);

    // Set package path
    char packagePath[FILENAME_MAX];
    strcpy(packagePath, pLuaEngine->assetsPath);
    TickernelCombinePaths(packagePath, FILENAME_MAX, "lua");
    TickernelCombinePaths(packagePath, FILENAME_MAX, "?.lua;");
    lua_getglobal(pLuaState, "package");
    lua_pushstring(pLuaState, packagePath);
    lua_setfield(pLuaState, -2, "path");
    lua_pop(pLuaState, 1);

    // Do file main.lua
    char luaMainFilePath[FILENAME_MAX];
    strcpy(luaMainFilePath, pLuaEngine->assetsPath);
    TickernelCombinePaths(luaMainFilePath, FILENAME_MAX, "lua");
    TickernelCombinePaths(luaMainFilePath, FILENAME_MAX, "main.lua");
    // Put engine state on the stack
    int luaResult = luaL_dofile(pLuaState, luaMainFilePath);
    TryThrowLuaError(pLuaState, luaResult);

    lua_pushlightuserdata(pLuaState, pLuaEngine->pGraphicEngine);
    lua_setfield(pLuaState, -2, "pGraphicEngine");

    lua_pushstring(pLuaState, pLuaEngine->assetsPath);
    lua_setfield(pLuaState, -2, "assetsPath");

    lua_pushstring(pLuaState, TickernelGetPathSeparator());
    lua_setfield(pLuaState, -2, "pathSeparator");

    lua_pushcfunction(pLuaState, AddModel);
    lua_setfield(pLuaState, -2, "AddModel");

    lua_pushcfunction(pLuaState, RemoveModel);
    lua_setfield(pLuaState, -2, "RemoveModel");

    lua_pushcfunction(pLuaState, UpdateModelUniformBuffer);
    lua_setfield(pLuaState, -2, "UpdateModelUniformBuffer");

    lua_pushcfunction(pLuaState, SetCamera);
    lua_setfield(pLuaState, -2, "SetCamera");

    lua_pushcfunction(pLuaState, LoadModel);
    lua_setfield(pLuaState, -2, "LoadModel");

    // Call start
    int startFunctionType = lua_getfield(pLuaState, -1, "Start");
    AssertLuaType(startFunctionType, LUA_TFUNCTION);
    luaResult = lua_pcall(pLuaState, 0, 0, 0);
    TryThrowLuaError(pLuaState, luaResult);
}

void UpdateLua(LuaEngine *pLuaEngine)
{
    lua_State *pLuaState = pLuaEngine->pLuaState;

    int startFunctionType = lua_getfield(pLuaState, -1, "Update");
    AssertLuaType(startFunctionType, LUA_TFUNCTION);
    int luaResult = lua_pcall(pLuaState, 0, 0, 0);
    TryThrowLuaError(pLuaState, luaResult);
}

void EndLua(LuaEngine *pLuaEngine)
{
    // Call End
    lua_State *pLuaState = pLuaEngine->pLuaState;
    int startFunctionType = lua_getfield(pLuaState, -1, "End");
    AssertLuaType(startFunctionType, LUA_TFUNCTION);
    int luaResult = lua_pcall(pLuaState, 0, 0, 0);
    TryThrowLuaError(pLuaState, luaResult);

    //  Pop engine state off the stack
    lua_pop(pLuaState, 1);

    lua_close(pLuaEngine->pLuaState);
}
