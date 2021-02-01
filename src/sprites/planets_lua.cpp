/**\file			planets_lua.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Saturday, June 4 2011
 * \date			Modified: Monday, February 15, 2016
 * \brief
 * \details
 */

#include "includes.h"
#include "sprites/planets.h"
#include "sprites/planets_lua.h"
#include "utilities/log.h"
#include "utilities/components.h"
#include "utilities/lua.h"
#include "utilities/timer.h"
#include "engine/models.h"
#include "engine/engines.h"
#include "engine/weapons.h"
#include "engine/alliances.h"
#include "engine/scenario_lua.h"
#include "sprites/spritemanager.h"

/**\class Planets_Lua
 * \brief Lua Interface for dealing with Planets
 *
 *\see Planet
 *\see Planets
 */

/**\brief Load all Planet related Lua functions
 */
void Planets_Lua::RegisterPlanets(lua_State *L) {
	static const luaL_Reg PlanetFunctions[] = {
		// Normally we would put a "new" function here.
		// Lua may not ever need to create planets though.
		{"Get", &Planets_Lua::Get},
		{NULL, NULL}
	};

	static const luaL_Reg PlanetMethods[] = {
		{"GetName", &Planets_Lua::GetName},
		{"GetID", &Planets_Lua::GetID},
		{"GetType", &Planets_Lua::GetType},
		{"GetPosition", &Planets_Lua::GetPosition},
		{"GetSize", &Planets_Lua::GetSize},
		{"GetAlliance", &Planets_Lua::GetAlliance},
		{"Landable", &Planets_Lua::GetLandable},
		{"GetSummary", &Planets_Lua::GetSummary},
		{"GetSurfaceImage", &Planets_Lua::GetSurfaceImage},
		{"GetModels", &Planets_Lua::GetModels},
		{"GetEngines", &Planets_Lua::GetEngines},
		{"GetWeapons", &Planets_Lua::GetWeapons},
		{"GetOutfits", &Planets_Lua::GetOutfits},
		{"GetForbidden", &Planets_Lua::GetForbidden},
		{"SetForbidden", &Planets_Lua::SetForbidden},

		// Editor Only
		{"SetPosition", &Planets_Lua::SetPosition},
		{"SetRadarColor", &Planets_Lua::SetRadarColor},
		{NULL, NULL}
	};
	luaL_newmetatable(L, EPIAR_PLANET);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_openlib(L, NULL, PlanetMethods, 0);
	luaL_openlib(L, EPIAR_PLANET, PlanetFunctions, 0);

	lua_pop(L, 2);
}

/**\brief Get a planet by name or id
 */
int Planets_Lua::Get(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if( n != 1 ) {
		return luaL_error(L, "Got %d arguments expected 1 (id or name)", n);
	}

	Planet* p = NULL;

	if(lua_isstring(L, 1)) {
		string name = (string)luaL_checkstring(L,1);
		p = (Planet*)Scenario_Lua::GetScenario(L)->GetPlanets()->GetPlanet(name);

		if (p == NULL) {
			return luaL_error(L, "There is no planet by the name of '%s'", name.c_str());
		}
	} else if(lua_isnumber(L, 1)) {
		int id = (int)luaL_checkinteger(L, 1);

		p = (Planet*)Scenario_Lua::GetScenario(L)->GetSpriteManager()->GetSpriteByID(id);

		if (p == NULL || p->GetDrawOrder() != DRAW_ORDER_PLANET) {
			return luaL_error(L, "There is no planet with ID %d", id);
		}
	} else {
		return luaL_error(L, "Cannot get planet with these arguments. Expected id or name.");
	}

	Scenario_Lua::PushSprite(L, p);

	return 1;
}

/**\brief Check that the a Lua value really is a Planet
 */
Planet *Planets_Lua::checkPlanet(lua_State *L, int index) {
	int *idptr = NULL;

	idptr = (int*)luaL_checkudata(L, index, EPIAR_PLANET);
	luaL_argcheck(L, idptr != NULL, index, "`EPIAR_PLANET' expected");

	Sprite* s = NULL;

	Planets *planets = Scenario_Lua::GetScenario(L)->GetPlanets();

	s = (Sprite *)planets->GetPlanetByID(*idptr);

	if((s) == NULL) luaL_typerror(L, index, EPIAR_PLANET);

	if(0 == ((s)->GetDrawOrder() & DRAW_ORDER_PLANET)) {
		luaL_typerror(L, index, EPIAR_PLANET);
	}

	return (Planet*)s;
}

/**\brief Get the Name of this planet
 */
int Planets_Lua::GetName(lua_State* L) {
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		Planet* planet = checkPlanet(L, 1);
		lua_pushstring(L, planet->GetName().c_str());
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}

	return 1;
}

/**\brief Get the ID of this planet
 */
int Planets_Lua::GetID(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet = checkPlanet(L,1);
		lua_pushinteger(L, planet->GetID());
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the Type of this planet
 */
int Planets_Lua::GetSummary(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet = checkPlanet(L,1);
		lua_pushstring(L, planet->GetSummary().c_str());
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the Type of this planet
 */
int Planets_Lua::GetSurfaceImage(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet = checkPlanet(L,1);
		Image* surface = planet->GetSurfaceImage();
		assert(surface);
		if( surface )
			lua_pushstring(L, surface->GetPath().c_str() );
		else
			lua_pushstring(L, "" );
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the Type of this planet
 */
int Planets_Lua::GetType(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet = checkPlanet(L,1);
		lua_pushinteger(L, planet->GetDrawOrder());
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the Position of this planet
 */
int Planets_Lua::GetPosition(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet = checkPlanet(L,1);
		lua_pushnumber(L, planet->GetWorldPosition().GetX() );
		lua_pushnumber(L, planet->GetWorldPosition().GetY() );
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 2;
}

/**\brief Get the Size of this planet
 */
int Planets_Lua::GetSize(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet = checkPlanet(L,1);
		lua_pushnumber(L, planet->GetRadarSize() );
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the Alliance of this planet
 */
int Planets_Lua::GetAlliance(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet = checkPlanet(L,1);
		lua_pushstring(L, planet->GetAlliance()->GetName().c_str());
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get Landable boolean of this planet
 */
int Planets_Lua::GetLandable(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet = checkPlanet(L,1);
		lua_pushboolean(L, planet->GetLandable() );
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the Models available at this planet
 */
int Planets_Lua::GetModels(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet = checkPlanet(L,1);
		list<Model*> models = planet->GetModels();
		list<Model*>::iterator iter;
		lua_createtable(L, models.size(), 0);
		int newTable = lua_gettop(L);
		int index = 1;
		for(iter=models.begin();iter!=models.end();++iter,++index){
			lua_pushstring(L, (*iter)->GetName().c_str() );
			lua_rawseti(L, newTable, index);
		}
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the Engines available at this planet
 */
int Planets_Lua::GetEngines(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet = checkPlanet(L,1);

		list<Engine*> engines = planet->GetEngines();
		list<Engine*>::iterator iter;

		lua_createtable(L, engines.size(), 0);

		int newTable = lua_gettop(L);
		int index = 1;

		for(iter = engines.begin(); iter != engines.end(); ++iter, ++index) {
			lua_pushstring(L, (*iter)->GetName().c_str() );
			lua_rawseti(L, newTable, index);
		}
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the Weapons available at this planet
 */
int Planets_Lua::GetWeapons(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet = checkPlanet(L,1);
		list<Weapon*> weapons = planet->GetWeapons();
		list<Weapon*>::iterator iter;

		lua_createtable(L, weapons.size(), 0);

		int newTable = lua_gettop(L);
		int index = 1;

		for(iter = weapons.begin(); iter != weapons.end(); ++iter, ++index) {
			lua_pushstring(L, (*iter)->GetName().c_str() );
			lua_rawseti(L, newTable, index);
		}
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the Outfits available at this planet
 */
int Planets_Lua::GetOutfits(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet = checkPlanet(L,1);
		list<Outfit*> outfits = planet->GetOutfits();
		list<Outfit*>::iterator iter;
		lua_createtable(L, outfits.size(), 0);
		int newTable = lua_gettop(L);
		int index = 1;
		for(iter=outfits.begin();iter!=outfits.end();++iter,++index){
			lua_pushstring(L, (*iter)->GetName().c_str() );
			lua_rawseti(L, newTable, index);
		}
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get forbidden status
 */
int Planets_Lua::GetForbidden(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		Planet* p = checkPlanet(L,1);
		if(p==NULL){
			lua_pushnumber(L, 0 );
			return 1;
		}
		lua_pushinteger(L, (int) ((p)->GetForbidden() ? 1 : 0) );
	}
	else {  
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to set forbidden status
 */
int Planets_Lua::SetForbidden(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		Planet* p = checkPlanet(L,1);
		if(p==NULL) return 0;
		int f = luaL_checkint (L, 2);
		(p)->SetForbidden( (f == 1) );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (planet, forbidden)", n);
	}
	return 0;
}

/**\brief Lua callable function to set forbidden status
*/
int Planets_Lua::SetPosition(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 3) {
		Planet* p = checkPlanet(L,1);
		if(p==NULL) return 0;
		int x = luaL_checkint (L, 2);
		int y = luaL_checkint (L, 3);
		p->SetWorldPosition( Coordinate(x, y) );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (planet, forbidden)", n);
	}
	return 0;
}

/**\brief Lua callable function to set planet's radar color.
 * \sa Sprite::SetRadarColor
 */
int Planets_Lua::SetRadarColor(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 4) {
		Planet* p = checkPlanet(L,1);
		if(p==NULL) return 0;
		int red = (int) luaL_checknumber (L, 2);
		int green = (int) luaL_checknumber (L, 3);
		int blue = (int) luaL_checknumber (L, 4);
		p->SetRadarColor(Color(red,green,blue));
	} else {
		luaL_error(L, "Got %d arguments expected 4 (self, red, green, blue)", n);
	}
	return 0;
}
