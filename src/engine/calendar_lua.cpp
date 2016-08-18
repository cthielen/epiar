/**\file			calendar_lua.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Sunday, August 14, 2016
 * \date			Modified: Sunday, August 14, 2016
 * \brief
 * \details
 */

#include "includes.h"
#include "engine/calendar.h"
#include "engine/calendar_lua.h"
#include "menu.h"
#include "utilities/log.h"
#include "utilities/lua.h"

/**\class Calendar_Lua
 * \brief Lua Interface for dealing with the Calendar
 *
 *\see Calendar
 */

/**\brief Load all Planet related Lua functions
 */
void Calendar_Lua::RegisterCalendar(lua_State *L){
	static const luaL_Reg CalendarFunctions[] = {
		{"AdvanceAfter", &Calendar_Lua::AdvanceAfter},
		{"Advance", &Calendar_Lua::Advance},
		{NULL, NULL}
	};

	luaL_newmetatable(L, EPIAR_CALENDAR);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_openlib(L, EPIAR_CALENDAR, CalendarFunctions, 0);

	lua_pop(L, 2);
}

int Calendar_Lua::AdvanceAfter(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments

	if (n != 1 ) {
		return luaL_error(L, "Got %d arguments expected 1", n);
	}

	int ticks = TO_INT(luaL_checknumber (L, 1));

	Menu::GetCurrentScenario()->GetCalendar()->AdvanceAfter(ticks);

	return 0;
}

int Calendar_Lua::Advance(lua_State *L) {
	Menu::GetCurrentScenario()->GetCalendar()->Advance();

	return 0;
}
