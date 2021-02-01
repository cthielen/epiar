/**\file			timer_lua.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Sunday, January 31, 2021
 * \date			Modified: Sunday, January 31, 2021
 * \brief			Lua bridge for Timer
 * \details
 */


#ifndef __H_TIMER_LUA
#define __H_TIMER_LUA

#ifdef __cplusplus
extern "C" {
#endif
#	include <lua.h>
#	include <lauxlib.h>
#	include <lualib.h>
#ifdef __cplusplus
}
#endif

#define EPIAR_TIMER_TABLE "Epiar.Timer"
#define EPIAR_TIMER "Timer"

class Timer_Lua {
	public:
		// Functions to communicate with Lua
		static void RegisterTimer(lua_State *L);

		static int GetTicks(lua_State *L);
};

#endif /*__H_TIMER_LUA */
