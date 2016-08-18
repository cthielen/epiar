/**\file			calendar_lua.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Sunday, August 14, 2016
 * \date			Modified: Sunday, August 14, 2016
 * \brief
 * \details
 */

#ifndef __h_lua_calendar__
#define __h_lua_calendar__

#ifdef __cplusplus
extern "C" {
#endif
#	include <lua.h>
#	include <lauxlib.h>
#	include <lualib.h>
#ifdef __cplusplus
}
#endif

#include "engine/calendar.h"
#include "includes.h"

#define EPIAR_CALENDAR "Calendar"

class Calendar_Lua {
	public:
		static void RegisterCalendar(lua_State *L);
		static int AdvanceAfter(lua_State *L);
		static int Advance(lua_State *L);
};

#endif // __h_lua_calendar__

