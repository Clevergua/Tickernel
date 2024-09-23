#include <luaEngine.h>
static int top;
#define LUA_PEEK_TOP(L)      \
    {                        \
        top = lua_gettop(L); \
    }
static void TryThrowLuaError(int luaResult)
{
    if (LUA_OK == luaResult)
    {
        // continue.
    }
    else
    {
        printf("Lua error code: %d!\n", luaResult);
        abort();
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
        printf("Lua type error: type:%d ,target:%d!\n", type, targetType);
        abort();
    }
}

static int AddModel(lua_State *pLuaState)
{
    int gameStateTpye = lua_getglobal(pLuaState, "gameState");
    AssertLuaType(gameStateTpye, LUA_TTABLE);
    int pGraphicEngineTpye = lua_getfield(pLuaState, -1, "pGraphicEngine");
    AssertLuaType(pGraphicEngineTpye, LUA_TLIGHTUSERDATA);
    GraphicEngine *pGraphicEngine = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 2);

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
            int colorType = lua_geti(pLuaState, -1, i + 1);
            //  vertices colors color

            AssertLuaType(colorType, LUA_TTABLE);
            uint32_t colorLength = 4;
            for (uint32_t j = 0; j < colorLength; j++)
            {
                int colorValueType = lua_geti(pLuaState, -1, j + 1);
                //  vertices colors color value

                AssertLuaType(colorValueType, LUA_TNUMBER);
                geometrySubpassVertices[i].color[j] = luaL_checknumber(pLuaState, -1);
                lua_pop(pLuaState, 1);
                //  vertices colors color
            }
            lua_pop(pLuaState, 1);
            //  vertices colors

            int vertexType = lua_geti(pLuaState, -2, i + 1);
            //  vertices colors vertex
            AssertLuaType(vertexType, LUA_TTABLE);
            uint32_t vertexLength = 3;
            for (uint32_t j = 0; j < vertexLength; j++)
            {
                int vertexValueType = lua_geti(pLuaState, -1, j + 1);
                //  vertices colors vertex value

                AssertLuaType(vertexValueType, LUA_TNUMBER);
                geometrySubpassVertices[i].position[j] = luaL_checknumber(pLuaState, -1);
                lua_pop(pLuaState, 1);
                //  vertices colors vertex
            }
            lua_pop(pLuaState, 1);
        }
        lua_pop(pLuaState, 2);

        uint32_t outputIndex;
        AddModelToGeometrySubpass(pGraphicEngine, vertexCount, geometrySubpassVertices, &outputIndex);

        TickernelFree(geometrySubpassVertices);

        lua_pushinteger(pLuaState, outputIndex);

        return 1;
    }
    else
    {
        //  gameState vertices colors
        lua_pop(pLuaState, 2);
        //  gameState
        return 0;
    }
}
static int UpdateModel(lua_State *pLuaState)
{
    int gameStateTpye = lua_getglobal(pLuaState, "gameState");
    AssertLuaType(gameStateTpye, LUA_TTABLE);
    int pGraphicEngineTpye = lua_getfield(pLuaState, -1, "pGraphicEngine");
    AssertLuaType(pGraphicEngineTpye, LUA_TLIGHTUSERDATA);
    GraphicEngine *pGraphicEngine = lua_touserdata(pLuaState, -1);
    lua_pop(pLuaState, 2);

    //  index modelMatrix
    GeometrySubpassModelUniformBuffer buffer;
    for (uint32_t column = 0; column < 4; column++)
    {
        int columnValueType = lua_geti(pLuaState, -1, column + 1);
        //  index modelMatrix column
        AssertLuaType(columnValueType, LUA_TTABLE);
        for (uint32_t row = 0; row < 4; row++)
        {
            //  index modelMatrix column value
            int rowValueType = lua_geti(pLuaState, -1, row + 1);
            AssertLuaType(rowValueType, LUA_TNUMBER);
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
    UpdateModelUniformToGeometrySubpass(pGraphicEngine, index, buffer);
    return 0;
}

void StartLua(LuaEngine *pLuaEngine)
{
    // New lua state
    pLuaEngine->pLuaState = luaL_newstate();
    lua_State *pLuaState = pLuaEngine->pLuaState;
    luaL_openlibs(pLuaState);

    // Set package path
    char packagePath[FILENAME_MAX];
    strcpy(packagePath, pLuaEngine->luaAssetsPath);
    TickernelCombinePaths(packagePath, FILENAME_MAX, "?.lua;");
    lua_getglobal(pLuaState, "package");
    lua_pushstring(pLuaState, packagePath);
    lua_setfield(pLuaState, -2, "path");
    lua_pop(pLuaState, 1);

    // Do file main.lua
    char luaMainFilePath[FILENAME_MAX];
    strcpy(luaMainFilePath, pLuaEngine->luaAssetsPath);
    TickernelCombinePaths(luaMainFilePath, FILENAME_MAX, "main.lua");
    // Put engine state on the stack
    int luaResult = luaL_dofile(pLuaState, luaMainFilePath);
    TryThrowLuaError(luaResult);

    lua_pushlightuserdata(pLuaState, pLuaEngine->pGraphicEngine);
    lua_setfield(pLuaState, -2, "pGraphicEngine");

    lua_pushcfunction(pLuaState, AddModel);
    lua_setfield(pLuaState, -2, "AddModel");

    // lua_pushcfunction(pLuaState, RemoveModel);
    // lua_setfield(pLuaState, -2, "RemoveModel");

    lua_pushcfunction(pLuaState, UpdateModel);
    lua_setfield(pLuaState, -2, "UpdateModel");

    // Call start
    int startFunctionType = lua_getfield(pLuaState, -1, "Start");
    AssertLuaType(startFunctionType, LUA_TFUNCTION);
    luaResult = lua_pcall(pLuaState, 0, 0, 0);
    TryThrowLuaError(luaResult);
}

void UpdateLua(LuaEngine *pLuaEngine)
{
    lua_State *pLuaState = pLuaEngine->pLuaState;

    int startFunctionType = lua_getfield(pLuaState, -1, "Update");
    AssertLuaType(startFunctionType, LUA_TFUNCTION);
    int luaResult = lua_pcall(pLuaState, 0, 0, 0);
    TryThrowLuaError(luaResult);
}

void EndLua(LuaEngine *pLuaEngine)
{
    // Call End
    lua_State *pLuaState = pLuaEngine->pLuaState;
    int startFunctionType = lua_getfield(pLuaState, -1, "End");
    AssertLuaType(startFunctionType, LUA_TFUNCTION);
    int luaResult = lua_pcall(pLuaState, 0, 0, 0);
    TryThrowLuaError(luaResult);

    //  Pop engine state off the stack
    lua_pop(pLuaState, 1);

    lua_close(pLuaEngine->pLuaState);
    TickernelFree(pLuaEngine->luaAssetsPath);
}
