#include "graphic.h"
#include "lauxlib.h"
#include "lualib.h"
#include "tickernelVoxel.h"

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