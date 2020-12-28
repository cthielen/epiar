/**\file			npc_lua.h
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Thursday, October 29, 2009
 * \date			Modified: Monday, November 16 2009
 * \brief			Lua Bridge for AI objects
 * \details
 */

#ifndef __H_NPC_LUA_
#define __H_NPC_LUA_

#include "sprites/npc.h"
#include "utilities/lua.h"

#define EPIAR_SHIP_TABLE "Epiar.Ship" ///< The Lua tag used to identify the metatable for a Ship
#define EPIAR_SHIP "Ship" ///< The Lua tag used to identify the Library for a Ship.
#define EPIAR_OUTFIT "Outfit" ///< The Lua tag used to identify the Library for an Outfit

class NPC_Lua {
	public:
		// Functions to communicate with Lua
		static void RegisterAI(lua_State *L);
		//static void pushShip(lua_State *L,int id);
		static NPC *checkShip(lua_State *L, int index);
		static Outfit *checkOutfit(lua_State *L, int index);
		static int newShip(lua_State *L);

		// Actions
		static int ShipAccelerate(lua_State* L);
		static int ShipRotate(lua_State* L);
		static int ShipRadarColor(lua_State* L);
		static int ShipFirePrimary(lua_State* L);
		static int ShipFireSecondary(lua_State* L);
		static int ShipDamage(lua_State* L);
		static int ShipRepair(lua_State* L);
		static int ShipExplode(lua_State* L);
		static int ShipRemove(lua_State* L);
		static int ShipAddToWeaponList (lua_State* L);
		static int ShipAddWeapon (lua_State* L);
		static int ShipRemoveFromWeaponList (lua_State* L);
		static int ShipRemoveWeapon (lua_State* L);
		static int ShipChangeWeapon(lua_State* L);
		static int ShipAddAmmo(lua_State* L);
		static int ShipSetModel(lua_State* L);
		static int ShipSetEngine(lua_State* L);
		static int ShipAddOutfit(lua_State* L);
		static int ShipRemoveOutfit(lua_State* L);
		static int ShipSetCredits(lua_State* L);
		static int ShipStoreCommodities(lua_State* L);
		static int ShipDiscardCommodities(lua_State* L);
		static int ShipAcceptMission(lua_State* L);
		static int ShipRejectMission(lua_State* L);
		static int ShipSetLuaControlFunc(lua_State* L);
		static int ShipRemoveLuaControlFunc(lua_State* L);
		static int ShipLand(lua_State* L);

		//Power Distribution
		static int ShipGetShieldBooster(lua_State* L);
		static int ShipGetEngineBooster(lua_State* L);
		static int ShipGetDamageBooster(lua_State* L);		
		static int ShipSetShieldBooster(lua_State* L);
		static int ShipSetEngineBooster(lua_State* L);
		static int ShipSetDamageBooster(lua_State* L);

		// Current Ship State
		static int ShipGetType(lua_State* L);
		static int ShipGetID(lua_State* L);
		static int ShipGetMass(lua_State* L);
		static int ShipGetName(lua_State* L);
		static int ShipSetName(lua_State* L);
		static int ShipGetAlliance(lua_State* L);
		static int ShipGetAngle(lua_State* L);
		static int ShipGetPosition(lua_State* L);
		//static int ShipSetPosition(lua_State* L);
		static int ShipGetMomentumAngle(lua_State* L);
		static int ShipGetMomentumSpeed(lua_State* L);
		static int ShipGetDirectionTowards(lua_State* L); // Accepts either Angles or Coordinates
		//static int ShipGetCurrentWeapon(lua_State* L);
		//static int ShipGetCurrentAmmo(lua_State* L);
		static int ShipGetWeapons(lua_State* L);
		static int ShipGetState(lua_State* L);
		static int ShipSetStateMachine(lua_State* L);
		static int ShipGetCredits(lua_State* L);
		static int ShipGetCargo(lua_State* L);
		static int ShipGetOutfits(lua_State* L);
		static int ShipGetTotalCost(lua_State* L);
		static int ShipIsDisabled(lua_State* L);
		static int ShipGetShieldDamage(lua_State* L);
		static int ShipSetShieldDamage(lua_State* L);
		static int ShipGetHullDamage(lua_State* L);
		static int ShipSetHullDamage(lua_State* L);
		static int SetTarget(lua_State* L);
		static int PlayerAddHiredEscort(lua_State *L);
		static int ShipUpdateFavor(lua_State* L);
		static int ShipGetFavor(lua_State* L);
	
		// Weapon slot related functions
		static int ShipGetWeaponsAmmo(lua_State* L);
		static int ShipGetWeaponSlotCount(lua_State* L);
		static int ShipGetWeaponSlotName(lua_State* L);
		static int ShipGetWeaponSlotStatus(lua_State* L);
		static int ShipSetWeaponSlotStatus(lua_State* L);
		static int ShipGetWeaponSlotFG(lua_State* L);
		static int ShipSetWeaponSlotFG(lua_State* L);
		static int ShipGetWeaponSlotContents(lua_State* L);

		// Ship Properties
		static int ShipGetModelName(lua_State* L);
		static int ShipGetEngine(lua_State* L);
		static int ShipGetHull(lua_State* L);
		static int ShipGetShield(lua_State* L);
		static int ShipGetMissions(lua_State* L);

		// Situational Awareness
		static int ShipGetMerciful(lua_State* L);
		static int ShipSetMerciful(lua_State* L);
	private:
};

#endif /* __H_NPC_LUA_ */
