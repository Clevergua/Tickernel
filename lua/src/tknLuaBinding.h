#ifndef tkn_lua_binding_h
#define tkn_lua_binding_h

#include "tknLua.h"
#include "tkn.h"
#include "lualib.h"

typedef enum
{
    TYPE_UINT8,
    TYPE_UINT16,
    TYPE_UINT32,
    TYPE_UINT64,
    TYPE_INT8,
    TYPE_INT16,
    TYPE_INT32,
    TYPE_INT64,
    TYPE_FLOAT,
    TYPE_DOUBLE,
} NumberType;

void bindFunctions(lua_State *pLuaState);
void printLuaStack(lua_State *pLuaState);

#endif