
#ifndef _LUA_PROTOBUF_
#define _LUA_PROTOBUF_

#if __cplusplus
extern "C" {
#endif

#define LUA_LIB
#include "lua.h"
#include "lauxlib.h"
#if LUA_VERSION_NUM > 501
	//
	// Lua 5.2
	//
#define lua_strlen lua_rawlen
// luaL_typerror always used with arg at ndx == NULL
#define luaL_typerror(L,ndx,str) luaL_error(L,"bad argument %d (%s expected, got nil)",ndx,str)
// luaL_register used once, so below expansion is OK for this case
#define luaL_register(L,name,reg) lua_newtable(L);luaL_setfuncs(L,reg,0)
// luaL_openlib always used with name == NULL
#define luaL_openlib(L,name,reg,nup) luaL_setfuncs(L,reg,nup)
#endif
	LUALIB_API int luaopen_pb(lua_State* L);

	int luaopen_lprotobuf(lua_State* L) {
		luaopen_pb(L);
		return 1;
	}

#if __cplusplus
}
#endif

#endif // _LUA_PROTOBUF_