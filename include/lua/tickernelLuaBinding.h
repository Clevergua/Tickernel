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
