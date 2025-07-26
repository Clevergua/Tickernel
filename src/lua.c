#include "lua.h"

LuaContext *createLuaContextPtr()
{
    LuaContext *pLuaContext = tknMalloc(sizeof(LuaContext));
    pLuaContext->pLuaState = luaL_newstate();
    luaL_openlibs(pLuaContext->pLuaState);
    return pLuaContext;
}
void destroyLuaContextPtr(LuaContext *pLuaContext)
{
    lua_close(pLuaContext->pLuaState);
    tknFree(pLuaContext);
}

void updateLuaGameplay(LuaContext *pLuaContext)
{
}
void updateLuaGfx(LuaContext *pLuaContext, GfxContext *pGfxContext)
{

}