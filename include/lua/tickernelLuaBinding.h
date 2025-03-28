#include "graphic.h"
#include "lauxlib.h"
#include "lualib.h"
#include "tickernelVoxel.h"

int luaAddModelToOpaqueGeometrySubpass(lua_State *pLuaState);
int luaRemoveModelFromOpaqueGeometrySubpass(lua_State *pLuaState);
int luaUpdateInstancesInOpaqueGeometrySubpass(lua_State *pLuaState);

int luaAddModelToWaterGeometrySubpass(lua_State *pLuaState);
int luaRemoveModelFromWaterGeometrySubpass(lua_State *pLuaState);
int luaUpdateInstancesInWaterGeometrySubpass(lua_State *pLuaState);

int luaUpdateGlobalUniformBuffer(lua_State *pLuaState);
int luaUpdateLightsUniformBuffer(lua_State *pLuaState);

int luaLoadModel(lua_State *pLuaState);