/**\file			timer_lua.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Sunday, January 31, 2021
 * \date			Modified: Sunday, January 31, 2021
 * \brief			Lua bridge for Timer
 * \details
 */

#include "includes.h"
#include "timer_lua.h"
#include "timer.h"

/**\class Timer_Lua
 * \brief Lua bridge for Timer. */

/**\brief Register's C++ functions to be callable in Lua.
 */
void Timer_Lua::RegisterTimer(lua_State *L){
	// Call them like:
	// ticks = Timer.GetTicks()
	static const luaL_Reg timerFunctions[] = {
		// Creation
		{"GetTicks", &Timer_Lua::GetTicks},
		{NULL, NULL}
	};

	luaL_newmetatable(L, EPIAR_TIMER);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_openlib(L, EPIAR_TIMER, timerFunctions, 0);

	lua_pop(L, 2);
}

int Timer_Lua::GetTicks(lua_State *L) {
	int n = lua_gettop(L); // Number of arguments

	if (n != 0 ) {
		return luaL_error(L, "Got %d arguments expected 0", n);
	}

	lua_pushinteger(L, Timer::GetTicks());

	return 1;
}
