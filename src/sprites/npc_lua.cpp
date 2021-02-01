/**\file			npc_lua.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Thursday, October 29, 2009
 * \date			Modified: Thursday, December 3, 2015
 * \brief			Lua Bridge for AI objects
 * \details
 */

#include "includes.h"
#include "common.h"
#include "utilities/lua.h"
#include "sprites/effects.h"
#include "sprites/player.h"
#include "sprites/planets.h"
#include "sprites/planets_lua.h"
#include "sprites/npc_lua.h"
#include "audio/sound.h"
#include "engine/camera.h"
#include "utilities/trig.h"
#include "engine/commodities.h"
#include "engine/scenario_lua.h"

/**\class NPC_Lua
 * \brief Lua bridge for NPC.*/

/**\brief Registers functions callable by Lua scripts for the NPC.
 */
void NPC_Lua::RegisterAI(lua_State *L){

	// This is the Function for creating Ships
	// Call this like:
	// some_ship = Epiar.Ship.new()
	static const luaL_Reg shipFunctions[] = {
		{"new", &NPC_Lua::newShip},
		{NULL, NULL}
	};

	// These are the Ship we're supporting in Lua
	// Call these like:
	// some_ship:Accelerate()
	static const luaL_Reg shipMethods[] = {
		// Actions
		{"Accelerate", &NPC_Lua::ShipAccelerate},
		{"Decelerate", &NPC_Lua::ShipDecelerate},
		{"Rotate", &NPC_Lua::ShipRotate},
		{"SetRadarColor", &NPC_Lua::ShipRadarColor},
		{"FirePrimary", &NPC_Lua::ShipFirePrimary},
		{"FireSecondary", &NPC_Lua::ShipFireSecondary},
		{"Damage", &NPC_Lua::ShipDamage},
		{"Repair", &NPC_Lua::ShipRepair},
		{"Explode", &NPC_Lua::ShipExplode},
		{"Remove", &NPC_Lua::ShipRemove},
		{"Land", &NPC_Lua::ShipLand},
		{"Dock", &NPC_Lua::ShipDock},
		{"SetLuaControlFunc", &NPC_Lua::ShipSetLuaControlFunc},
		
		// Power Distribution
		{"GetShieldBooster", &NPC_Lua::ShipGetShieldBooster},
		{"GetEngineBooster", &NPC_Lua::ShipGetEngineBooster},
		{"GetDamageBooster", &NPC_Lua::ShipGetDamageBooster},
		{"SetShieldBooster", &NPC_Lua::ShipSetShieldBooster},
		{"SetEngineBooster", &NPC_Lua::ShipSetEngineBooster},
		{"SetDamageBooster", &NPC_Lua::ShipSetDamageBooster},
		
		// Outfit Changes
		{"AddWeapon", &NPC_Lua::ShipAddWeapon},
		{"AddToWeaponList", &NPC_Lua::ShipAddToWeaponList},
		{"RemoveWeapon", &NPC_Lua::ShipRemoveWeapon},
		{"RemoveFromWeaponList", &NPC_Lua::ShipRemoveFromWeaponList},
		{"AddAmmo", &NPC_Lua::ShipAddAmmo},
		{"SetModel", &NPC_Lua::ShipSetModel},
		{"SetEngine", &NPC_Lua::ShipSetEngine},
		{"AddOutfit", &NPC_Lua::ShipAddOutfit},
		{"RemoveOutfit", &NPC_Lua::ShipRemoveOutfit},
		{"SetCredits", &NPC_Lua::ShipSetCredits},
		{"StoreCommodities", &NPC_Lua::ShipStoreCommodities},
		{"DiscardCommodities", &NPC_Lua::ShipDiscardCommodities},
		{"AcceptMission", &NPC_Lua::ShipAcceptMission},
		{"RejectMission", &NPC_Lua::ShipRejectMission},

		// Current State
		{"GetID", &NPC_Lua::ShipGetID},
		{"GetMass", &NPC_Lua::ShipGetMass},
		{"GetName", &NPC_Lua::ShipGetName},
		{"SetName", &NPC_Lua::ShipSetName},
		{"GetAlliance", &NPC_Lua::ShipGetAlliance},
		{"GetType", &NPC_Lua::ShipGetType},
		{"GetAngle", &NPC_Lua::ShipGetAngle},
		{"GetPosition", &NPC_Lua::ShipGetPosition},
		{"GetMomentumAngle", &NPC_Lua::ShipGetMomentumAngle},
		{"GetMomentumSpeed", &NPC_Lua::ShipGetMomentumSpeed},
		{"directionTowards", &NPC_Lua::ShipGetDirectionTowards},
		{"SetMerciful", &NPC_Lua::ShipSetMerciful},
		{"GetMerciful", &NPC_Lua::ShipGetMerciful},

		// Favor State
		{"GetFavor", &NPC_Lua::ShipGetFavor},
		{"UpdateFavor", &NPC_Lua::ShipUpdateFavor},

		// General State
		{"GetModelName", &NPC_Lua::ShipGetModelName},
		{"GetEngine", &NPC_Lua::ShipGetEngine},
		{"GetHull", &NPC_Lua::ShipGetHull},
		{"GetShield", &NPC_Lua::ShipGetShield},
		{"GetWeapons", &NPC_Lua::ShipGetWeapons},
		{"GetOutfits", &NPC_Lua::ShipGetOutfits},
		{"GetState", &NPC_Lua::ShipGetState},
		{"SetStateMachine", &NPC_Lua::ShipSetStateMachine},
		{"GetCredits", &NPC_Lua::ShipGetCredits},
		{"GetCargo", &NPC_Lua::ShipGetCargo},
		{"GetTotalCost", &NPC_Lua::ShipGetTotalCost},
		{"IsDisabled", &NPC_Lua::ShipIsDisabled},
		{"GetMissions", &NPC_Lua::ShipGetMissions},
		{"GetHullDamage", &NPC_Lua::ShipGetHullDamage},
		{"SetHullDamage", &NPC_Lua::ShipSetHullDamage},
		{"GetShieldDamage", &NPC_Lua::ShipGetShieldDamage},
		{"SetShieldDamage", &NPC_Lua::ShipSetShieldDamage},
		{"GetWeaponsAmmo", &NPC_Lua::ShipGetWeaponsAmmo},
		{"GetWeaponSlotCount", &NPC_Lua::ShipGetWeaponSlotCount},
		{"GetWeaponSlotName", &NPC_Lua::ShipGetWeaponSlotName},
		{"GetWeaponSlotStatus", &NPC_Lua::ShipGetWeaponSlotStatus},
		{"SetWeaponSlotStatus", &NPC_Lua::ShipSetWeaponSlotStatus},
		{"GetWeaponSlotContents", &NPC_Lua::ShipGetWeaponSlotContents}, // builds a Lua table; no setter for this one
		{"GetWeaponSlotFG", &NPC_Lua::ShipGetWeaponSlotFG},
		{"SetWeaponSlotFG", &NPC_Lua::ShipSetWeaponSlotFG},
		{"SetTarget", &NPC_Lua::SetTarget},
		{"AddHiredEscort", &NPC_Lua::PlayerAddHiredEscort},
		{NULL, NULL}
	};

	luaL_newmetatable(L, EPIAR_SHIP);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_openlib(L, NULL, shipMethods, 0);

	luaL_openlib(L, EPIAR_SHIP, shipFunctions, 0);

	lua_pop(L, 2);
}

/**\brief Validates Ship in Lua.
 */
NPC* NPC_Lua::checkShip(lua_State *L, int index){
	int* idptr = (int*)luaL_checkudata(L, index, EPIAR_SHIP);
	luaL_argcheck(L, idptr != NULL, index, "`EPIAR_SHIP' expected");

	Sprite* s = Scenario_Lua::GetScenario(L)->GetSpriteManager()->GetSpriteByID(*idptr);
	/*
	if ((s) == NULL) luaL_typerror(L, index, EPIAR_SHIP);
	if (0==((s)->GetDrawOrder() & DRAW_ORDER_SHIP|DRAW_ORDER_PLAYER)){
		luaL_typerror(L, index, EPIAR_SHIP);
	}
	*/

	return (NPC*)s;
}

/**\brief Validates Outfit in Lua.
 */
Outfit* NPC_Lua::checkOutfit(lua_State *L, int index){
	int* idptr = (int*)luaL_checkudata(L, index, EPIAR_OUTFIT);
	luaL_argcheck(L, idptr != NULL, index, "`EPIAR_OUTFIT' expected");
	Sprite* s;
	s = Scenario_Lua::GetScenario(L)->GetSpriteManager()->GetSpriteByID(*idptr);
	return (Outfit*)s;
}

/**\brief Spawns a new AI ship for Lua.
 */
int NPC_Lua::newShip(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 7)
		return luaL_error(L, "Got %d arguments expected 5 (name, x, y, model, engine, script, alliance)", n);

	string name = luaL_checkstring (L, 1);
	double x = luaL_checknumber (L, 2);
	double y = luaL_checknumber (L, 3);
	string modelname = luaL_checkstring (L, 4);
	string enginename = luaL_checkstring (L, 5);
	string statemachine = luaL_checkstring (L, 6);
	string alliancename = luaL_checkstring (L, 7);

	//LogMsg(INFO,"Creating new Ship (%s) (%f,%f) (%s) (%s) (%s) (%s)",name.c_str(), x,y,modelname.c_str(),enginename.c_str(),statemachine.c_str(), alliancename.c_str());

	// Allocate memory for a pointer to object
	NPC *s = new NPC(name,statemachine);
	s->SetWorldPosition( Coordinate(x, y) );
	s->SetModel( Scenario_Lua::GetScenario(L)->GetModels()->GetModel(modelname) );
	s->SetEngine( Scenario_Lua::GetScenario(L)->GetEngines()->GetEngine(enginename) );
	s->SetAlliance( Scenario_Lua::GetScenario(L)->GetAlliances()->GetAlliance(alliancename) );
	Scenario_Lua::PushSprite(L,s);

	// Add this ship to the SpriteManager
	Scenario_Lua::GetScenario(L)->GetSpriteManager()->Add((Sprite*)(s));

	return 1;
}

// Ship Functions

// Ship Actions

/**\brief Lua callable function to accelerate the ship.
 * \sa Ship::Accelerate()
 */
int NPC_Lua::ShipAccelerate(lua_State* L) {
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		NPC* ai = checkShip(L, 1);
		if(ai == NULL) return 0;
		luaL_argcheck(L, ai != NULL, 1, "`array' expected");
		(ai)->Accelerate( false );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (self, direction)", n);
	}

	return 0;
}

/**\brief Lua callable function to decelerate the ship.
 * \sa Ship::Accelerate()
 */
int NPC_Lua::ShipDecelerate(lua_State* L) {
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		NPC* ai = checkShip(L, 1);
		if(ai == NULL) return 0;
		luaL_argcheck(L, ai != NULL, 1, "`array' expected");
		(ai)->Decelerate();
	} else {
		luaL_error(L, "Got %d arguments expected 2 (self, direction)", n);
	}

	return 0;
}

/**\brief Lua callable function to rotate the ship.
 * \sa Ship::Rotate(float)
 */
int NPC_Lua::ShipRotate(lua_State* L) {
	int n = lua_gettop(L); // Number of arguments

	if (n == 2) {
		NPC* ai = checkShip(L, 1);
		if(ai == NULL) { return 0; }
		float dir = static_cast<float>( luaL_checknumber(L, 2) );
		(ai)->Rotate(dir, false);
	} else {
		luaL_error(L, "Got %d arguments expected 2 (self, direction)", n);
	}

	return 0;
}

/**\brief Lua callable function to set ship's radar color.
 * \sa Sprite::SetRadarColor
 */
int NPC_Lua::ShipRadarColor(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 4) {
		NPC* ai = checkShip(L, 1);
		if(ai == NULL) { return 0; }
		int red = (int) luaL_checknumber (L, 2);
		int green = (int) luaL_checknumber (L, 3);
		int blue = (int) luaL_checknumber (L, 4);
		(ai)->SetRadarColor(Color(red, green, blue));
	} else {
		luaL_error(L, "Got %d arguments expected 4 (self, red, green, blue)", n);
	}

	return 0;
}

/**\brief Lua callable function to fire ship's weapons.
 * \sa Ship::FirePrimary()
 * \returns FireStatus result of the firing attempt
 */
int NPC_Lua::ShipFirePrimary(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	int target = -1;
	if( (n == 1) || (n == 2) ) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		if(n == 2)
		{
			target = luaL_checkinteger(L,2);
		}
		FireStatus result = (ai)->FirePrimary(target);
		lua_pushinteger(L, (int)(result) );
		return 1;
	}
	return luaL_error(L, "Got %d arguments expected 1 or 2 (ship, [target])", n);
}

/**\brief Lua callable function to fire ship's weapons.
 * \sa Ship::FireSecondary()
 * \returns FireStatus result of the firing attempt
 */
int NPC_Lua::ShipFireSecondary(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	int target = -1;
	if( (n == 1) || (n == 2) ) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		if(n == 2)
		{
			target = luaL_checkinteger(L,2);
		}
		FireStatus result = (ai)->FireSecondary(target);
		lua_pushinteger(L, (int)(result) );
		return 1;
	}
	return luaL_error(L, "Got %d arguments expected 1 or 2 (ship, [target])", n);
}

/**\brief Lua callable function to add damage to ship.
 * \sa Ship::Damage(short int)
 */
int NPC_Lua::ShipDamage(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		short damage = (short) luaL_checknumber (L, 2);
		(ai)->Damage( damage );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, damage)", n);
	}
	return 0;
}

/**\brief Lua callable function to repair the ship.
 */
int NPC_Lua::ShipRepair(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		short damage = (short) luaL_checkint (L, 2);
		(ai)->Repair( damage );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, damage)", n);
	}
	return 0;
}

/**\brief Lua callable function to explode the ship.
 * \sa Effect
 */
int NPC_Lua::ShipExplode(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		LogMsg(INFO,"A %s Exploded!",(ai)->GetModelName().c_str());
		// Play explode sound
		Sound *explodesnd = Sound::Get("data/audio/effects/18384__inferno__largex.wav.ogg");
		if(OPTION(int, "options/sound/explosions"))
			explodesnd->Play(
				(ai)->GetWorldPosition() - Scenario_Lua::GetScenario(L)->GetCamera()->GetFocusCoordinate());
		Scenario_Lua::GetScenario(L)->GetSpriteManager()->Add(
			new Effect((ai)->GetWorldPosition(), "data/animations/explosion1.ani", 0) );
		Scenario_Lua::GetScenario(L)->GetSpriteManager()->Delete((Sprite*)(ai));
	} else {
		luaL_error(L, "Got %d arguments expected 1 (ship)", n);
	}
	return 0;
}

/**\brief Lua callable function to remove this ship
 *
 *  This does not create an explosion, the ship is simply removed instantly.
 */
int NPC_Lua::ShipRemove(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		Scenario_Lua::GetScenario(L)->GetSpriteManager()->Delete((Sprite*)(ai));
	} else {
		luaL_error(L, "Got %d arguments expected 1 (ship)", n);
	}
	return 0;
}

/**\brief Lua callable function to add weapon to ship (but see function below)
 */
int NPC_Lua::ShipAddToWeaponList(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		string weaponName = luaL_checkstring (L, 2);
		(ai)->AddToShipWeaponList(weaponName);
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, weaponName)", n);
	}
	return 0;
}	
/**\brief Lua callable function to add weapon to ship and update the weapon slots accordingly (PREFERRED)
 */
int NPC_Lua::ShipAddWeapon(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		string weaponName = luaL_checkstring (L, 2);
		int status = (ai)->AddShipWeapon(weaponName);
		lua_pushinteger(L, status);
		if(!status)
			return luaL_error(L, "Not able to add ship weapon! All slots are full.", n);
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, weaponName)", n);
	}
	return 0;
}	

/**\brief Lua callable function to remove weapon from ship.
 */
int NPC_Lua::ShipRemoveFromWeaponList(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		if( lua_isnumber(L,2)){
			int weaponNum = luaL_checkinteger(L, 2);
			(ai)->RemoveFromShipWeaponList(weaponNum);
		} else {
			string weaponName = luaL_checkstring (L, 2);
			(ai)->RemoveFromShipWeaponList(weaponName);
		}
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, weaponName)", n);
	}
	return 0;
}

/**\brief Lua callable function to remove weapon from ship and update the weapon slots accordingly (PREFERRED)
 */
int NPC_Lua::ShipRemoveWeapon(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		string weaponName = luaL_checkstring (L, 2);
		(ai)->RemoveShipWeapon(weaponName);
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, weaponName)", n);
	}
	return 0;
}	

/**\brief Lua callable function to retrieve shield boost data
 *\sa Ship::GetShieldBoost
 */
int NPC_Lua::ShipGetShieldBooster(lua_State* L){
	int n = lua_gettop(L); //Number of arguments
	if (n==1){
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		double s=(double) (ai)->GetShieldBoost();
		lua_pushnumber (L, s);

	}
	else {
		luaL_error(L, "Got %d arguments expected 1 argument (ship)",n);
	}
	return 1;
}

/**\brief Lua callable function to retrieve damage boost data
 *\sa Ship::GetDamageBoost
 */
int NPC_Lua::ShipGetDamageBooster(lua_State* L){
	int n = lua_gettop(L); //Number of arguments
	if (n==1){
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		lua_pushnumber (L, (double) (ai)->GetDamageBoost());
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 argument (ship)",n);
	}
	return 1;
}

/**\brief Lua callable function to retrieve engine boost data
 *\sa Ship::GetEngineBoost
 */
int NPC_Lua::ShipGetEngineBooster(lua_State* L){
	int n = lua_gettop(L); //Number of arguments
	if (n==1){
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		lua_pushnumber (L, (double) (ai)->GetEngineBoost());
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 argument (ship)",n);
	}
	return 1;
}

/**\brief Lua callable function to set shield boost data
 *\sa Ship::SetShieldBoost
 */
int NPC_Lua::ShipSetShieldBooster(lua_State* L){
	int n = lua_gettop(L); //Number of arguments
	if (n==2){
		NPC* ai = checkShip( L, 1 );
		if(ai==NULL) return 0;
		float shield=(float) luaL_checknumber( L, 2 );
		(ai)->SetShieldBoost(shield);
	}
	else {
		luaL_error(L, "Got %d arguments expected 2 arguments (ship, shield boost)",n);
	}
	return 0;
}

/**\brief Lua callable function to set damage boost data
 *\sa Ship::SetDamageBoost
 */
int NPC_Lua::ShipSetDamageBooster(lua_State* L){
	int n = lua_gettop(L); //Number of arguments
	if (n==2){
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		float damage=(float) luaL_checknumber( L, 2 );
		(ai)->SetDamageBoost(damage);
	}
	else {
		luaL_error(L, "Got %d arguments expected 2 arguments (ship, damage boost)",n);
	}
	return 0;
}

/**\brief Lua callable function to retrieve engine boost data
 *\sa Ship::SetEngineBoost
 */
int NPC_Lua::ShipSetEngineBooster(lua_State* L){
	int n = lua_gettop(L); //Number of arguments
	if (n==2){
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		
		float engine=(float) luaL_checknumber( L, 2 );
		(ai)->SetEngineBoost(engine);

	}
	else {
		luaL_error(L, "Got %d arguments expected 2 arguments (ship ,engine boost)",n);
	}
	return 0;
}

/**\brief Lua callable function to add ammo to a ship's weapon.
 * \sa Ship::addAmmo
 * \todo This should be passed an Ammo Type, not a weapon name
 */
int NPC_Lua::ShipAddAmmo(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 3) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		string weaponName = luaL_checkstring (L, 2);
		int qty = (int) luaL_checknumber (L, 3);

		Weapon* weapon = Scenario_Lua::GetScenario(L)->GetWeapons()->GetWeapon(weaponName);
		if(weapon==NULL){
			return luaL_error(L, "There is no such weapon as a '%s'", weaponName.c_str());
		}
		(ai)->AddAmmo(weapon->GetAmmoType(),qty);
	} else {
		luaL_error(L, "Got %d arguments expected 3 (ship, weaponName, qty)", n);
	}
	return 0;
}	

/**\brief Lua callable function to set the ship model.
 * \sa Ship::SetModel
 */
int NPC_Lua::ShipSetModel(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		string modelName = luaL_checkstring (L, 2);
		Model* model = Scenario_Lua::GetScenario(L)->GetModels()->GetModel( modelName );
		luaL_argcheck(L, model != NULL, 2, string("There is no Model named `" + modelName + "'").c_str());
		(ai)->SetModel( model );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, modelName)", n);
	}
	return 0;

}

/**\brief Lua callable function to set the ship engine.
 * \sa Ship::SetEngine(Engine*)
 */
int NPC_Lua::ShipSetEngine(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		string engineName = luaL_checkstring (L, 2);
		Engine* engine = Scenario_Lua::GetScenario(L)->GetEngines()->GetEngine( engineName );
		luaL_argcheck(L, engine != NULL, 2, string("There is no Engine named `" + engineName + "'").c_str());
		(ai)->SetEngine( engine );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, engineName)", n);
	}
	return 0;
}

/**\brief Lua callable function to add an Outfit to a ship.
 * \sa Ship::addOutfit(Outfit*)
 */
int NPC_Lua::ShipAddOutfit(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		string outfitName = luaL_checkstring (L, 2);
		Outfit* outfit = Scenario_Lua::GetScenario(L)->GetOutfits()->GetOutfit( outfitName );
		luaL_argcheck(L, outfit != NULL, 2, string("There is no Outfit named `" + outfitName + "'").c_str());
		(ai)->AddOutfit( outfit );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, outfitName)", n);
	}
	return 0;
}

/**\brief Lua callable function to remove an Outfit from a ship.
 * \sa Ship::RemoveOutfit(Outfit*)
 */
int NPC_Lua::ShipRemoveOutfit(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		string outfitName = luaL_checkstring (L, 2);
		Outfit* outfit = Scenario_Lua::GetScenario(L)->GetOutfits()->GetOutfit( outfitName );
		luaL_argcheck(L, outfit != NULL, 2, string("There is no Outfit named `" + outfitName + "'").c_str());
		// TODO: luaL_argcheck that ai has this outfit already.
		(ai)->RemoveOutfit( outfit );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, outfitName)", n);
	}
	return 0;
}

/**\brief Lua callable function to set the credits for this ship
 * \sa Ship::SetCredits()
 */
int NPC_Lua::ShipSetCredits(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		int credits = luaL_checkint (L, 2);
		luaL_argcheck(L, credits >= 0, 2, "Cannot set Credits to a negative number." );
		(ai)->SetCredits( credits );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, engineName)", n);
	}
	return 0;
}

/**\brief Lua callable function to Store a number of Commodities on this ship
 * \sa Ship::StoreCommodities()
 */
int NPC_Lua::ShipStoreCommodities(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 3) {
		return luaL_error(L, "Got %d arguments expected 2 (ship, commodityName, count)", n);
	}

	// Get the Inputs
	NPC* ai = checkShip(L,1);
	string commodityName = luaL_checkstring (L, 2);
	int count = luaL_checkint (L, 3);

	LogMsg(INFO, "Storing %d tons of %s.", count, commodityName.c_str());

	// Check Inputs
	if(ai==NULL) { return 0; }
	Commodity *commodity = Scenario_Lua::GetScenario(L)->GetCommodities()->GetCommodity( commodityName );
	luaL_argcheck(L, commodity != NULL, 2, string("There is no Commodity named `" + commodityName + "'").c_str());

	// Store the Commodity
	int actuallyStored = (ai)->StoreCommodities( commodityName, count );
	lua_pushinteger(L, actuallyStored );
	return 1;
}

/**\brief Lua callable function to Discard a number of Commodities from this ship
 * \sa Ship::DiscardCommodities()
 */
int NPC_Lua::ShipDiscardCommodities(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 3) {
		return luaL_error(L, "Got %d arguments expected 2 (ship, commodityName, count)", n);
	}

	// Get the Inputs
	NPC* ai = checkShip(L,1);
	string commodityName = luaL_checkstring (L, 2);
	int count = luaL_checkint (L, 3);

	LogMsg(INFO, "Discarding %d tons of %s.", count, commodityName.c_str());

	// Check Inputs
	if(ai==NULL) { return 0; }
	Commodity *commodity = Scenario_Lua::GetScenario(L)->GetCommodities()->GetCommodity( commodityName );
	luaL_argcheck(L, commodity != NULL, 2, string("There is no Commodity named `" + commodityName + "'").c_str());

	// Discard the Commodity
	int actuallyDiscarded = (ai)->DiscardCommodities( commodityName, count );
	lua_pushinteger(L, actuallyDiscarded );
	return 1;
}

int NPC_Lua::ShipAcceptMission(lua_State *L){
	int n=lua_gettop(L);
	if(n!=3){
		return luaL_error(L, "%d arguments provided, but expected 2 (self, MissionType, MissionTable)");
	}

	// Check that only players accept missions
	Ship* ship = checkShip(L,1);
	if( ship->GetDrawOrder() != DRAW_ORDER_PLAYER ) {
		return luaL_error(L, "Only players may accept Missions");
	}
	Player *player = (Player*)ship;
	
	// Get and Validate the Mission Information
	string missionType = (string) luaL_checkstring(L,2);
	int missionTable = luaL_ref(L, LUA_REGISTRYINDEX); // Gets and pops the top of the stack, which should have the the missionTable.
	if( Mission::ValidateMission( L, missionType, missionTable, 0 ) ) {
		Mission *mission = new Mission( L, missionType, missionTable );
		player->AcceptMission( mission );
	} else {
		luaL_unref(L, LUA_REGISTRYINDEX, missionTable);
		return luaL_error(L, "The Mission Type '%s' or the Mission Table is invalid.", missionType.c_str() );
	}
	return 0;
}

int NPC_Lua::ShipRejectMission(lua_State *L){
	int n=lua_gettop(L);
	if(n!=2){
		return luaL_error(L, "%d arguments provided, but expected 2 (self, MissionName)");
	}

	// Check that only players accept missions
	Ship* ship = checkShip(L,1);
	if( ship->GetDrawOrder() != DRAW_ORDER_PLAYER ) {
		return luaL_error(L, "Only players may accept Missions");
	}
	Player *player = (Player*)ship;
	
	// Get and Validate the Mission Information
	string missionName = (string) luaL_checkstring(L,2);
	player->RejectMission( missionName );
	return 0;
}


/**\brief Lua callable function to get the Type of this ship
 *
 * This is usually used by Lua prior to casting a generic sprite as a Ship.
 *
 * \sa Sprite::GetDrawOrder()
 */

int NPC_Lua::ShipGetType(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		lua_pushinteger(L, (ai)->GetDrawOrder() );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get the ship's ID
 * \sa Sprite::GetID()
 */
int NPC_Lua::ShipGetID(lua_State* L){
	int n = lua_gettop(L); // Number of arguments

	if (n == 1) {
		NPC* ai = checkShip(L, 1);

		if(ai == NULL) {
			lua_pushnumber(L, 0);
			return 1;
		}

		lua_pushinteger(L, (ai)->GetID() );
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}

	return 1;
}

/**\brief Lua callable function to get the ship's mass
 * \sa Sprite::GetMass()
 */
int NPC_Lua::ShipGetMass(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L, 0 );
			return 1;
		}
		lua_pushnumber(L, (ai)->GetModel()->GetMass() );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get the ship's ID
 * \sa Ship::GetName() AI::GetName() Player::GetName()
 */
int NPC_Lua::ShipGetName(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Ship* ship = (Ship*)checkShip(L,1);
		if(ship==NULL){
			return 0;
		} else {
			lua_pushstring(L, ship->GetName().c_str() );
		}
	} else {
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to set the ship's name
 */
int NPC_Lua::ShipSetName(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		string newName = luaL_checkstring (L, 2);
		(ai)->SetName( newName );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, newName)", n);
	}
	return 0;
}

/**\brief Lua callable function to get the ship's ID
 * \sa AI::GetAlliance()
 */
int NPC_Lua::ShipGetAlliance(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		NPC* ai = checkShip(L,1);
		if (ai==NULL){
			return 0;
		} else if( ai->GetDrawOrder() & DRAW_ORDER_PLAYER ) {
			lua_pushstring(L, "Independent" ); ///< TODO The "Independent" Alliance should be created or this should be changed.
		} else {
			Alliance* alliance = ai->GetAlliance();
			if (alliance==NULL){
				lua_pushstring(L, "Independent" ); ///< TODO The "Independent" Alliance should be created or this should be changed.
			} else {
				lua_pushstring(L, alliance->GetName().c_str() );
			}
		}
	} else {
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get the ship's angle.
 * \sa Sprite::GetAngle()
 */
int NPC_Lua::ShipGetAngle(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L, 0 );
			return 1;
		}
		lua_pushnumber(L, (double) normalizeAngle( (ai)->GetAngle() ) );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get the world position
 * \sa Coordinate::GetWorldPosition()
 */
int NPC_Lua::ShipGetPosition(lua_State* L){
	int n = lua_gettop(L); // Number of arguments

	if (n == 1) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L,0);
			lua_pushnumber(L,0);
		} else {
			lua_pushnumber(L, (double) (ai)->GetWorldPosition().GetX() );
			lua_pushnumber(L, (double) (ai)->GetWorldPosition().GetY() );
		}
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 2;
}

/**\brief Lua callable function to get the ship's momentum angle.
 * \sa Sprite::GetMomentum()
 * \sa Sprite::GetAngle()
 */
int NPC_Lua::ShipGetMomentumAngle(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		NPC* ai = checkShip(L,1);
		if(ai == NULL){
			lua_pushnumber(L, 0 );
		} else {
			lua_pushnumber(L, (double) (ai)->GetMomentum().GetAngle() );
		}
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}

	return 1;
}

/**\brief Lua callable function to get the ship's momentum speed.
 * \sa Sprite::GetMomentum()
 * \sa Coordinate::GetMagnitude()
 */
int NPC_Lua::ShipGetMomentumSpeed(lua_State* L) {
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		NPC* ai = checkShip(L, 1);
		if(ai == NULL){
			lua_pushnumber(L, 0 );
		} else {
			lua_pushnumber(L, (double) (ai)->GetMomentum().GetMagnitude() );
		}
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}

	return 1;
}

/**\brief Lua callable function to get the ship's direction towards target.
 * \sa Ship::directionTowards
 */
int NPC_Lua::ShipGetDirectionTowards(lua_State* L) {
	int n = lua_gettop(L); // Number of arguments

	if (n == 2) { // Angle
		NPC* ai = checkShip(L, 1);
		if(ai == NULL){
			lua_pushnumber(L, 0);
		} else {
			float angle = static_cast<float>( luaL_checknumber(L, 2) );
			lua_pushnumber(L, (double) (ai)->GetDirectionTowards(angle) );
		}
	} else if(n == 3) { // Coordinate
		NPC* ai = checkShip(L, 1);
		if(ai == NULL) {
			lua_pushnumber(L, 0 );
		} else {
			double x = static_cast<float>( luaL_checknumber(L, 2) );
			double y = static_cast<float>( luaL_checknumber(L, 3) );
			lua_pushnumber(L, (double) (ai)->GetDirectionTowards(Coordinate(x, y)) );
		}
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}

	return 1;
}

/**\brief Lua callable function to get the ship's weapons.
 * \sa Ship::getWeapons()
 */
int NPC_Lua::ShipGetWeapons(lua_State* L){
	vector<Weapon*>::iterator iter;
	int newTable;
	int n = lua_gettop(L);  // Number of arguments
	unsigned int w;
	if (n != 1)
		luaL_error(L, "Got %d arguments expected 1 (self)", n);

	NPC* ai = checkShip(L,1);
	if(ai==NULL){
		return 0;
	}

	vector<Weapon*>* weapons = (ai)->GetWeapons();
	lua_createtable(L, weapons->size(), 0);
	newTable = lua_gettop(L);
	for( w = 0; w < weapons->size(); ++w )
	{
		lua_pushinteger(L, w ); // Key
		lua_pushfstring(L, ((*weapons)[w])->GetName().c_str() ); // Value
		lua_settable(L,newTable);
	}
	return 1;
}

///**\brief Lua callable function to get the current weapon.
// * \sa Ship::getCurrentWeapon()
// */
//int NPC_Lua::ShipGetCurrentWeapon(lua_State* L){
//	int n = lua_gettop(L);  // Number of arguments
//	if (n != 1)
//		luaL_error(L, "Got %d arguments expected 1 (self)", n);
//
//	NPC* ai = checkShip(L,1);
//	if(ai==NULL){
//		return 0;
//	}
//	Weapon* cur = (ai)->GetCurrentWeapon();
//	lua_pushfstring(L, cur?cur->GetName().c_str():"" );
//	return 1;
//}
//
///**\brief Lua callable function to get the current ammo.
// * \sa Ship::getCurrentAmmo()
// */
//int NPC_Lua::ShipGetCurrentAmmo(lua_State* L){
//	int n = lua_gettop(L);  // Number of arguments
//	if (n != 1)
//		luaL_error(L, "Got %d arguments expected 1 (self)", n);
//
//	NPC* ai = checkShip(L,1);
//	if(ai==NULL){
//		return 0;
//	}
//	lua_pushnumber(L, (ai)->GetCurrentAmmo() );
//	return 1;
//}


/**\brief Lua callable function to get the ship's model name.
 * \sa Ship::GetModelName()
 */
int NPC_Lua::ShipGetModelName(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		luaL_error(L, "Got %d arguments expected 1 (self)", n);

	NPC* ai = checkShip(L,1);
	if(ai==NULL){
		return 0;
	}
	lua_pushfstring(L, ((ai)->GetModelName()).c_str() );
	return 1;
}

/**\brief Lua callable function to get the ship's engine name.
 * \sa Ship::GetEngine()
 */
int NPC_Lua::ShipGetEngine(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		luaL_error(L, "Got %d arguments expected 1 (self)", n);

	NPC* ai = checkShip(L,1);
	if(ai==NULL){
		return 0;
	}
	lua_pushfstring(L, ((ai)->GetEngine())->GetName().c_str() );
	return 1;
}

/**\brief Lua callable function to get the hull status (in %).
 * \sa Ship::getHullIntegrityPct
 */
int NPC_Lua::ShipGetHull(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL){
			// The ship doesn't exist (anymore?) so it's probably dead
			lua_pushnumber(L, 0 );
			return 1;
		}
		lua_pushnumber(L, (double) (ai)->GetHullIntegrityPct() );
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get the shield status (in %).
 * \sa Ship::GetShieldIntegrityPct
 */
int NPC_Lua::ShipGetShield(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL){
			// The ship doesn't exist (anymore?) so it's probably dead
			lua_pushnumber(L, 0 );
			return 1;
		}
		lua_pushnumber(L, (double) (ai)->GetShieldIntegrityPct() );
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get the State Machine of an NPC.
 * \sa AI::GetStateMachine
 */
int NPC_Lua::ShipGetState(lua_State* L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL){
			return 0;
		} else if( ai->GetDrawOrder() & DRAW_ORDER_PLAYER ) {
			// We need to do this since the Player doesn't have a StateMachine.
			// Warning! these are not actually valid States or StateMachines
			lua_pushstring(L, "Player" ); // State Machine
			lua_pushstring(L, "Playing" ); // State
		} else {
			lua_pushstring(L, (ai)->GetStateMachine().c_str() );
			lua_pushstring(L, (ai)->GetState().c_str() );
		}
	} else {
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 2;
}

/**\brief Lua callable function to set the state machine of an NPC.
 */
int NPC_Lua::ShipSetStateMachine(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL){
			luaL_error(L, "Can't set the state machine of a null sprite.");
		}
		else if(ai->GetDrawOrder() & DRAW_ORDER_PLAYER){
			return luaL_error(L, "Can't set the state machine of a player.");
		}
		else {
			string sm = luaL_checkstring (L, 2);
			(ai)->SetStateMachine( sm );
		}
		
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, statemachine)", n);
	}
	return 0;
}


/**\brief Lua callable function to get the current credits.
 * \sa AI::GetStateMachine
 */
int NPC_Lua::ShipGetCredits(lua_State* L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L, 0 );
		} else {
			lua_pushnumber(L, (ai)->GetCredits() );
		}
	} else {
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get the ship's stored Commodities.
 * \sa Ship::getCargo()
 */
int NPC_Lua::ShipGetCargo(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n != 1)
		luaL_error(L, "Got %d arguments expected 1 (self)", n);

	NPC* ai = checkShip(L,1);
	if(ai==NULL){
		return 0;
	}

	map<Commodity*,unsigned int> cargo = (ai)->GetCargo();
	map<Commodity*,unsigned int>::iterator it = cargo.begin();

	// Create a Lua table and populate it using the form:
	// {"Foo": 10, "Bar": 20}
	lua_createtable(L, cargo.size(), 0);
	int newTable = lua_gettop(L);
	while( it!=cargo.end() ) {
		lua_pushfstring(L, ((*it).first)->GetName().c_str() ); // KEY
		lua_pushinteger(L, (*it).second ); // Value
		lua_settable(L,newTable);
		++it;
	}

	// Push Cargo statistics
	lua_pushinteger(L, (ai)->GetCargoSpaceUsed() ); // Total Tons Stored
	lua_pushinteger(L, Scenario_Lua::GetScenario(L)->GetModels()->GetModel((ai)->GetModelName())->GetCargoSpace() ); // Maximum Tons Storable
		
	return 3;
}

/**\brief Lua callable function to get the ship's Outfits.
 *
 * Outfits are returned to Lua as a table of outfit names.
 *
 * \sa Ship::getOutfit()
 */
int NPC_Lua::ShipGetOutfits(lua_State* L){
	list<Outfit*>::iterator iter;
	int newTable, tableIndex;
	int n = lua_gettop(L);  // Number of arguments

	if (n != 1)
		luaL_error(L, "Got %d arguments expected 1 (self)", n);

	NPC* ai = checkShip(L,1);
	if(ai==NULL){
		return 0;
	}

	list<Outfit*>* outfits = (ai)->GetOutfits();
	lua_createtable(L, outfits->size(), 0);
	newTable = lua_gettop(L);
	for(iter=outfits->begin(), tableIndex=1; iter!=outfits->end(); ++iter, ++tableIndex)
	{
		lua_pushinteger(L, tableIndex ); // Key
		lua_pushfstring(L, (*iter)->GetName().c_str() ); // Value
		lua_settable(L,newTable);
	}

	return 1;
}

/**\brief Lua callable function to get the total cost of the ship.
 * \sa Ship::GetTotalCost
 */
int NPC_Lua::ShipGetTotalCost(lua_State* L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L, 0 );
		} else {
			lua_pushnumber(L, (ai)->GetTotalCost() );
		}
	} else {
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to determine if ship is disabled.
 * \sa Ship::IsDisabled
 */
int NPC_Lua::ShipIsDisabled(lua_State* L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L, 0 );
		} else {
			lua_pushnumber(L, (ai)->IsDisabled() );
		}
	} else {
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

int NPC_Lua::ShipGetMissions(lua_State* L) {
	int n = lua_gettop(L);  // Number of arguments
	if(n!=1){
		return luaL_error(L, "%d arguments provided, but expected 2 (self)");
	}

	// Check that only players accept missions
	Ship* ship = checkShip(L,1);
	if(ship==NULL) return 0;
	if( ship->GetDrawOrder() != DRAW_ORDER_PLAYER ) {
		return luaL_error(L, "Only players may accept Missions");
	}
	Player *player = (Player*)ship;

	list<Mission*>* missions = player->GetMissions();

	// Create the Table
	lua_createtable(L, missions->size(), 0);
	const int missionTableIndex = lua_gettop(L);
	list<Mission*>::iterator iter;

	// Fill the above table with all the Players mission tables
	int m;
	for( m=1, iter = missions->begin(); iter != missions->end() ; ++iter, ++m )
	{
		// Push each mission table
		lua_pushinteger(L,m);
		(*iter)->PushMissionTable();
	
		lua_settable(L,missionTableIndex);
	}

	return 1;
}

/**\brief Lua callable function to get merciful status of a ship
 */
int NPC_Lua::ShipGetMerciful(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		if( ai->GetDrawOrder() == DRAW_ORDER_PLAYER ) {
			// Players are always merciful to themselves
			lua_pushnumber(L, 1 );
			return 1;
		}
		lua_pushnumber(L, (int) (ai)->GetMerciful() );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to set the merciful status of a ship
 */
int NPC_Lua::ShipSetMerciful(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		if( ai->GetDrawOrder() == DRAW_ORDER_PLAYER ) {
			return luaL_error(L, "Cannot force players to be merciful.");
		}
		int merciful = luaL_checkint (L, 2);
		(ai)->SetMerciful( merciful );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, merciful)", n);
	}
	return 0;
}

/**\brief Lua callable function to get Shield damage of a ship
 */
int NPC_Lua::ShipGetShieldDamage(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L, 0 );
			return 1;
		}
		lua_pushinteger(L, (int) (ai)->GetShieldDamage() );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to set the Shield damage of a ship
 */
int NPC_Lua::ShipSetShieldDamage(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		int damage = luaL_checkint (L, 2);
		(ai)->SetShieldDamage( damage );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, damage)", n);
	}
	return 0;
}

/**\brief Lua callable function to get Hull damage of a ship
 */
int NPC_Lua::ShipGetHullDamage(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L, 0 );
			return 1;
		}
		lua_pushinteger(L, (int) (ai)->GetHullDamage() );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to set the Hull damage of a ship
 */
int NPC_Lua::ShipSetHullDamage(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		int damage = luaL_checkint (L, 2);
		(ai)->SetHullDamage( damage );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, damage)", n);
	}
	return 0;
}


/**\brief Lua callable function to set the ai's current target
 */
int NPC_Lua::SetTarget(lua_State* L){
	int n=lua_gettop(L);//Number of arguments
	if(n==2){
		NPC* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		int target = luaL_checknumber(L,2);
		ai->SetTarget(target);
	} else {
		luaL_error(L, "Got %d arguments expected 2 arguments (self, target)",n);
	}
	return 0;
}

/**\brief Lua callable function to get the model of a ship
 */
/*int NPC_Lua::ShipGetModel(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		Ship* s = checkShip(L,1);
		if(s==NULL){
			lua_pushnumber(L, 0 );
			return 1;
		}
		lua_pushinteger(L, (int) (s)->GetModel() );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}*/

/**\brief Lua callable function to get the ship's weapons.
 * \sa Ship::getWeaponsAndAmmo()
 */
int NPC_Lua::ShipGetWeaponsAmmo(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		luaL_error(L, "Got %d arguments expected 1 (self)", n);

	NPC* ai = checkShip(L,1);
	if(ai==NULL){
		return 0;
	}

	map<Weapon*,int> weaponPack = (ai)->GetWeaponsAndAmmo();
	map<Weapon*,int>::iterator it = weaponPack.begin();

	lua_createtable(L, weaponPack.size(), 0);
	int newTable = lua_gettop(L);
	while( it!=weaponPack.end() ) {
		lua_pushfstring(L, ((*it).first)->GetName().c_str() ); // KEY
		lua_pushinteger(L, (*it).second ); // Value
		lua_settable(L,newTable);
		++it;
	}
	return 1;
}

/**\brief Lua callable function to get the number of weapon slots of any kind on an outfit (probably a ship model)
 */
int NPC_Lua::ShipGetWeaponSlotCount(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		Ship* s = checkShip(L,1);
		if(s==NULL){
			lua_pushnumber(L, 0 );
			return 1;
		}
		lua_pushinteger(L, (int) (s)->GetWeaponSlotCount() );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}


/**\brief Lua callable function to get name of a weapon slot
 */
int NPC_Lua::ShipGetWeaponSlotName(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 2) {
		Ship* s = checkShip(L,1);
		int slotNum = luaL_checkint (L, 2);
		if(s==NULL){
			lua_pushstring(L, "");
			return 1;
		}
		lua_pushstring(L, (s)->GetWeaponSlotName(slotNum).c_str() );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, slot)", n);
	}
	return 1;
}

/**\brief Lua callable function to get status of a weapon slot
 */
int NPC_Lua::ShipGetWeaponSlotStatus(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 2) {
		Ship* s = checkShip(L,1);
		int slotNum = luaL_checkint (L, 2);
		if(s==NULL){
			lua_pushstring(L, "");
			return 1;
		}
		lua_pushstring(L, (s)->GetWeaponSlotContent(slotNum).c_str() );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, slot)", n);
	}
	return 1;
}
/**\brief Lua callable function to set status of a weapon slot
 */
int NPC_Lua::ShipSetWeaponSlotStatus(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 3) {
		Ship* s = checkShip(L,1);
		int slotNum = luaL_checkinteger (L, 2);
		string weaponName = luaL_checkstring (L, 3);
		if(s==NULL){
			lua_pushstring(L, "");
			return 1;
		}
		Weapon* weapon = Scenario_Lua::GetScenario(L)->GetWeapons()->GetWeapon(weaponName);
		s->SetWeaponSlotContent(slotNum, weapon);
	} else {
		luaL_error(L, "Got %d arguments expected 3 (ship, slot, status)", n);
	}
	return 1;
}
/**\brief Lua callable function to set firing group of a weapon slot
 */
int NPC_Lua::ShipSetWeaponSlotFG(lua_State* L){
	int n = lua_gettop(L); // Number of arguments

	if (n == 3) {
		Ship* s = checkShip(L, 1);

		int slotNum = luaL_checkint (L, 2);
		short int fg = luaL_checkint (L, 3);

		if(s == NULL) {
			lua_pushstring(L, "");
			return 1;
		}

		s->SetWeaponSlotFG(slotNum, fg);
	} else {
		luaL_error(L, "Got %d arguments expected 3 (ship, slot, fg)", n);
	}

	return 1;
}

/**\brief Lua callable function to get firing group of a weapon slot
 */
int NPC_Lua::ShipGetWeaponSlotFG(lua_State* L){
	int n = lua_gettop(L); // Number of arguments

	if (n == 2) {
		Ship* s = checkShip(L, 1);
		int slotNum = luaL_checkint (L, 2);

		if(s == NULL) {
			lua_pushstring(L, "");
			return 1;
		}

		lua_pushinteger(L, (s)->GetWeaponSlotFG(slotNum) );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, slot)", n);
	}

	return 1;
}

/**\brief Lua callable function to get the ship's weapons as defined by the weapon slots
 * You don't normally want to use this function unless you are changing ships.
 */
int NPC_Lua::ShipGetWeaponSlotContents(lua_State* L) {
	int n = lua_gettop(L); // Number of arguments
	if (n != 1) {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}

	Ship* s = checkShip(L, 1);
	if(s == NULL) {
		return 0;
	}

	map<string,string> weaps = s->GetWeaponSlotContents();
	map<string,string>::iterator it = weaps.begin();

	lua_createtable(L, weaps.size(), 0);
	int newTable = lua_gettop(L);

	while( it != weaps.end() ) {
		lua_pushfstring(L, ((*it).first).c_str() );
		lua_pushfstring(L, ((*it).second).c_str() );
		lua_settable(L, newTable);
		++it;
	}

	return 1;
}

/**\brief Lua callable function to set the player's Lua control function
 */
int NPC_Lua::ShipSetLuaControlFunc(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 2) {
		Player *p = (Player *)checkShip(L, 1);
		if( p == NULL ) return 0;

		if( p->GetDrawOrder() != DRAW_ORDER_PLAYER ) {
			return luaL_error(L, "Only players may be controlled ");
		}

		string controlFunc = luaL_checkstring (L, 2);
		
		(p)->SetLuaControlFunc( controlFunc );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, controlFunc)", n);
	}

	return 0;
}


/**\brief Lua callable function to set the player's Lua control function
 */
int NPC_Lua::ShipLand(lua_State* L) {
	int n = lua_gettop(L); // Number of arguments

	if (n == 2) {
		// Get the Player
		Player *player = (Player *)checkShip(L, 1);
		if( player == NULL ) return 0;
		if( player->GetDrawOrder() != DRAW_ORDER_PLAYER ) {
			return luaL_error(L, "Only players may accept Missions");
		}

		// Get the Planet
		Planet *planet = Planets_Lua::checkPlanet(L, 2);
		if( planet == NULL ) return 0;
		
		player->Land( L, planet );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, controlFunc)", n);
	}

	return 0;
}

/**
 * "Docking" for NPCs is really waiting a certain number of seconds
 * while stopped before doing something else.
 * 
 * Sets a random duration to remain still for 'docking'.
 * npc.lua will check when this time expires and exit
 * the 'docking' state. */
int NPC_Lua::ShipDock(lua_State* L) {
	int n = lua_gettop(L); // Number of arguments

	if (n == 1) {
		// Get the ship
		Ship *ship = checkShip(L, 1);
		if( ship == NULL ) return 0;

		// Generate a random duration to dock between 5 and 15 seconds
		int duration = Timer::TimestampAfterSeconds( (rand() % 10) + 5 );

		lua_pushinteger(L, duration );
	} else {
		luaL_error(L, "Got %d arguments expected 1 (ship)", n);
	}

	return 1;
}

/** \brief Add an escort to the list to be put into the XML saved game file
 *  \details Keeps track of a bare minimum of information, but not details like hull integrity or non-standard outfits.
 */
int NPC_Lua::PlayerAddHiredEscort(lua_State* L) {
	int n = lua_gettop(L); // Number of arguments

	if (n == 4) {
			Player* p = (Player*)NPC_Lua::checkShip(L, 1);
			if(p == NULL) return 0;

			string type = luaL_checkstring (L, 2);
			int pay = luaL_checkint (L, 3);
			int spriteID = luaL_checkint (L, 4);

			(p)->AddHiredEscort(type, pay, spriteID);
	} else {
			luaL_error(L, "Got %d arguments expected 4 (player, type, pay, spriteID)", n);
	}

	return 0;
}

/**\brief Lua callable function to Store a number of Commodities on this ship
 * \sa Player::UpdateFavor()
 */
int NPC_Lua::ShipUpdateFavor(lua_State* L){
	int n = lua_gettop(L); // Number of arguments

	if (n != 3) {
		return luaL_error(L, "Got %d arguments expected 2 (ship, allianceName, value)", n);
	}

	// Get the Inputs
	Player* player = (Player*)NPC_Lua::checkShip(L, 1);
	if( player == NULL ) return 0;
	if( player->GetDrawOrder() != DRAW_ORDER_PLAYER ) {
		return luaL_error(L, "Only players may update their favor");
	}
	string allianceName = luaL_checkstring (L, 2);
	int value = luaL_checkint (L, 3);

	LogMsg(INFO, "Updating %s Favor by %d.", allianceName.c_str(), value);

	// Check Inputs
	if(player == NULL) { return 0; }

	Alliance *alliance = Scenario_Lua::GetScenario(L)->GetAlliances()->GetAlliance( allianceName );
	luaL_argcheck(L, alliance != NULL, 2, string("There is no alliance named `" + allianceName + "'").c_str());

	// Update the favor
	player->UpdateFavor( allianceName, value );

	return 0;
}

/**\brief Lua callable function to get the player's favor
 * \sa Ship::getCargo()
 */
int NPC_Lua::ShipGetFavor(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if ((n < 1) || (n>2))
		luaL_error(L, "Got %d arguments expected 1 (player) or 2 (player, allianceName)", n);

	// Get the Inputs
	Player* player = (Player*)NPC_Lua::checkShip(L,1);
	if( player == NULL ) return 0;
	if( player->GetDrawOrder() != DRAW_ORDER_PLAYER ) {
		return luaL_error(L, "Only players have favor");
	}

	if( n == 2 ) {
		// Return the Favor of this Alliance
		string allianceName = luaL_checkstring (L, 2);
		Alliance *alliance = Scenario_Lua::GetScenario(L)->GetAlliances()->GetAlliance( allianceName );
		luaL_argcheck(L, alliance != NULL, 2, string("There is no alliance named `" + allianceName + "'").c_str());

		lua_pushinteger(L, player->GetFavor(alliance) ); // Value
		return 1;
	} else {
		// Return table of favor values keyed by alliance name.
		Alliances *alliances = Scenario_Lua::GetScenario(L)->GetAlliances();
		list<string>* allianceNames = alliances->GetNames();

		lua_newtable(L);
		for( list<string>::iterator aName = allianceNames->begin(); aName != allianceNames->end(); ++aName){
			int favor = player->GetFavor( alliances->GetAlliance(*aName) );
			Lua::setField( (*aName).c_str(), favor );
		}

		return 1;
	}
}
