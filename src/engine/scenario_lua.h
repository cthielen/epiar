/**\file			scenario_lua.h
 * \author			Matt Zweig
 * \date			Created: Friday, September 3, 2010
 * \date			Modified: Thursday, October 8, 2015
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
		static void RegisterEditor(lua_State *L);

		static void StoreScenario(lua_State *L, Scenario *sim);
		static Scenario* GetScenario(lua_State *L);

		static int Console_echo(lua_State *L);
		static int Pause(lua_State *L);

		// Options Interfaces
		static int GetOption(lua_State *L);
		static int SetOption(lua_State *L);

		// Input Interfaces
		static int RegisterKey(lua_State *L);
		static int UnRegisterKey(lua_State *L);

		// Scenario Interfaces
		static int Unpause(lua_State *L);
		static int IsPaused(lua_State *L);
		static int GetCamera(lua_State *L);
		static int MoveCamera(lua_State *L);
		static int FocusCamera(lua_State *L);
		static int ShakeCamera(lua_State *L);
		static int GetPlayerNames(lua_State *L);
		static int SetLastPlanet(lua_State *L);
		static int LoadPlayer(lua_State *L);
		static int SavePlayer(lua_State *L);
		static int NewPlayer(lua_State *L);

		// Sprite Fetchers
		static int GetPlayer(lua_State *L);
		static int GetSpriteByID(lua_State *L);
		static int GetSprites(lua_State *L, int includeKind, int excludeKind);
		static int GetNearestSprite(lua_State *L, int type = DRAW_ORDER_ALL);
		static int GetNearestShip(lua_State *L);
		static int GetNearestPlanet(lua_State *L);
		static int GetNearbyNPCs(lua_State *L);
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
		static int GetScenarioInfo(lua_State *L);
		static int GetCommodityInfo(lua_State *L);
		static int GetAllianceInfo(lua_State *L);
		static int GetModelInfo(lua_State *L);
		static int GetPlanetInfo(lua_State *L);
		static int GetWeaponInfo(lua_State *L);
		static int GetEngineInfo(lua_State *L);
		static int GetOutfitInfo(lua_State *L);
		static int GetTechnologyInfo(lua_State *L);

		// Editor Commands
		static int SetInfo(lua_State *L);
		static int SetDefaultPlayer(lua_State *L);
		static int GetDefaultPlayer(lua_State *L);
		static int SaveComponents(lua_State *L);
		static int ListImages(lua_State *L);
		static int ListAnimations(lua_State *L);
		static int ListSounds(lua_State *L);
		static int SetDescription(lua_State *L);

		static void PushSprite(lua_State *L,Sprite* sprite);
		static void PushComponents(lua_State *L, list<Component*> *components);
	private:
};

#endif // __H_SCENARIO_LUA__
