/**\file			scenario_lua.h
 * \author			Christopher Thielen
 * \date			Created: Monday, August 18, 2014
 * \date			Modified: Monday, August 18, 2014
 * \brief			Scenario Managment from Lua
 * \details
 */


#ifndef __H_SCENARIO_LUA__
#define __H_SCENARIO_LUA__

#include "utilities/lua.h"
#include "sprites/sprite.h"
#include "engine/scenario.h"

class Scenario_Lua {
	public:
		static void RegisterScenario(lua_State *L);

		static Scenario* GetScenario(lua_State *L);
		static int GetScenarioInfo(lua_State *L);

		// Options Interfaces
		static int Getoption(lua_State *L);
		static int Setoption(lua_State *L);

		static int GetSectors(lua_State *L);
		static int GetPlanets(lua_State *L);

		// Game Components
		static int GetCommodityNames(lua_State *L);
		static int GetAllianceNames(lua_State *L);
		static int GetEngineNames(lua_State *L);
		static int GetModelNames(lua_State *L);
		static int GetWeaponNames(lua_State *L);
		static int GetOutfitNames(lua_State *L);
		static int GetTechnologyNames(lua_State *L);
		static int GetPlanetNames(lua_State *L);

		// Information about Components
		static int GetMSRP(lua_State *L);
		static int GetSimulationInfo(lua_State *L);
		static int GetCommodityInfo(lua_State *L);
		static int GetAllianceInfo(lua_State *L);
		static int GetModelInfo(lua_State *L);
		static int GetPlanetInfo(lua_State *L);
		static int GetWeaponInfo(lua_State *L);
		static int GetEngineInfo(lua_State *L);
		static int GetOutfitInfo(lua_State *L);
		static int GetTechnologyInfo(lua_State *L);
};

#endif // __H_SCENARIO_LUA__
