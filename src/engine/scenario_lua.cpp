/**\file			scenario_lua.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Monday, August 18, 2014
 * \date			Modified: Monday, August 18, 2014
 * \brief			Scenario Managment from Lua
 * \details
 */

#include "includes.h"
#include "common.h"

#include "audio/audio.h"
#include "audio/audio_lua.h"
#include "engine/console.h"
#include "engine/scenario.h"
#include "engine/scenario_lua.h"
#include "engine/models.h"
#include "engine/alliances.h"
#include "utilities/log.h"
#include "utilities/lua.h"
#include "ui/ui_lua.h"
#include "ui/ui.h"
#include "ui/ui_window.h"
#include "ui/ui_label.h"
#include "ui/ui_button.h"
#include "graphics/video.h"
#include "sprites/ai_lua.h"
#include "sprites/player.h"
#include "sprites/sprite.h"
#include "sprites/planets.h"
#include "sprites/planets_lua.h"
#include "engine/camera.h"
#include "input/input.h"
#include "utilities/file.h"
#include "utilities/filesystem.h"
#include "engine/hud.h"

/** \class Scenario_Lua
 *  \brief Lua bridge for interacting with the Epiar scenario.
 */

void Scenario_Lua::RegisterScenario(lua_State *L) {
	static const luaL_Reg EngineFunctions[] = {
		// OPTION Functions
		{"getoption", &Scenario_Lua::Getoption},
		{"setoption", &Scenario_Lua::Setoption},

		// Game Component lists
		{"commodities", &Scenario_Lua::GetCommodityNames},
		{"alliances", &Scenario_Lua::GetAllianceNames},
		{"models", &Scenario_Lua::GetModelNames},
		{"weapons", &Scenario_Lua::GetWeaponNames},
		{"outfits", &Scenario_Lua::GetOutfitNames},
		{"engines", &Scenario_Lua::GetEngineNames},
		{"technologies", &Scenario_Lua::GetTechnologyNames},
		//{"planetNames", &Scenario_Lua::GetPlanetNames},

		// Sprite Searching Functions
		//{"planets", &Scenario_Lua::GetPlanets},
		{"sectors", &Scenario_Lua::GetSectors},

		// Game Component Information
		{"getMSRP", &Scenario_Lua::GetMSRP},
		{"getScenarioInfo", &Scenario_Lua::GetScenarioInfo},
		{"getCommodityInfo", &Scenario_Lua::GetCommodityInfo},
		{"getAllianceInfo", &Scenario_Lua::GetAllianceInfo},
		{"getModelInfo", &Scenario_Lua::GetModelInfo},
		{"getWeaponInfo", &Scenario_Lua::GetWeaponInfo},
		{"getEngineInfo", &Scenario_Lua::GetEngineInfo},
		{"getOutfitInfo", &Scenario_Lua::GetOutfitInfo},
		{"getTechnologyInfo", &Scenario_Lua::GetTechnologyInfo},

		{NULL, NULL}
	};
	luaL_register(L, "Epiar", EngineFunctions);

}

/** \brief Get an OPTION value
 *  \param [in] key Path to a specific OPTION.
 *  \returns string representation of the OPTION's value.
 */
int Scenario_Lua::Getoption(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (option)", n);
	string path = (string)lua_tostring(L, 1);
	string value = OPTION(string,path);
	lua_pushstring(L, value.c_str());
	return 1;
}

/** \brief Set an OPTION value
 *  \param [in] key Path to a specific OPTION.
 *  \param [in] value the OPTION's new value.
 *  \note All Lua primitives should support lua_tostring, so ints and floats can be used.
 */
int Scenario_Lua::Setoption(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 2)
		return luaL_error(L, "Got %d arguments expected 1 (option,value)", n);
	string path = (string)lua_tostring(L, 1);
	string value = (string)lua_tostring(L, 2);
	SETOPTION(path,value);
	return 0;
}

/** \brief Get Commodity names
 *  \returns list of names as strings
 */
int Scenario_Lua::GetCommodityNames(lua_State *L){
	list<string> *names = GetScenario(L)->GetCommodities()->GetNames();
	Lua::pushStringList(L,names);
	return 1;
}

/** \brief Get Alliance names
 *  \returns list of names as strings
 */
int Scenario_Lua::GetAllianceNames(lua_State *L){
	list<string> *names = GetScenario(L)->GetAlliances()->GetNames();
	Lua::pushStringList(L,names);
	return 1;
}

/** \brief Get Weapon names
 *  \returns list of names as strings
 */
int Scenario_Lua::GetWeaponNames(lua_State *L){
	list<string> *names = GetScenario(L)->GetWeapons()->GetNames();
	Lua::pushStringList(L,names);
	return 1;
}

/** \brief Get Outfit names
 *  \returns list of names as strings
 */
int Scenario_Lua::GetOutfitNames(lua_State *L){
	list<string> *names = GetScenario(L)->GetOutfits()->GetNames();
	Lua::pushStringList(L,names);
	return 1;
}

/** \brief Get Model names
 *  \returns list of names as strings
 */
int Scenario_Lua::GetModelNames(lua_State *L){
	list<string> *names = GetScenario(L)->GetModels()->GetNames();
	Lua::pushStringList(L,names);
	return 1;
}

/** \brief Get Engine names
 *  \returns list of names as strings
 */
int Scenario_Lua::GetEngineNames(lua_State *L){
	list<string> *names = GetScenario(L)->GetEngines()->GetNames();
	Lua::pushStringList(L,names);
	return 1;
}

/** \brief Get Technology names
 *  \returns list of names as strings
 */
int Scenario_Lua::GetTechnologyNames(lua_State *L){
	list<string> *names = GetScenario(L)->GetTechnologies()->GetNames();
	Lua::pushStringList(L, names);
	return 1;
}

/** \brief Get Planet names
 *  \returns list of names as strings
 */
//int Scenario_Lua::GetPlanetNames(lua_State *L){
//	list<string> *names = GetScenario(L)->GetPlanets()->GetNames();
//	Lua::pushStringList(L, names);
//	return 1;
//}

/** \brief Get the MSRP of a Game Component
 *  \details  Searches all saleable Component collections for a Component by the given name.
 *  \param[in] Name of a Game Component
 *  \returns credit value of the Component
 */
int Scenario_Lua::GetMSRP(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (  )", n);
	string name = (string)luaL_checkstring(L,1);

	// Is there a priced Component named 'name'?
	Component* comp = NULL;
	if( (comp = GetScenario(L)->GetModels()->Get(name)) != NULL )
		lua_pushinteger(L,((Model*)comp)->GetMSRP() );
	else if( (comp = GetScenario(L)->GetEngines()->Get(name)) != NULL )
		lua_pushinteger(L,((Engine*)comp)->GetMSRP() );
	else if( (comp = GetScenario(L)->GetWeapons()->Get(name)) != NULL )
		lua_pushinteger(L,((Weapon*)comp)->GetMSRP() );
	else if( (comp = GetScenario(L)->GetCommodities()->Get(name)) != NULL )
		lua_pushinteger(L,((Commodity*)comp)->GetMSRP() );
	else if( (comp = GetScenario(L)->GetOutfits()->Get(name)) != NULL )
		lua_pushinteger(L,((Outfit*)comp)->GetMSRP() );
	else {
		return luaL_error(L, "Couldn't find anything by the name: '%s'", name.c_str());
	}
	// One of those should have worked or we would have hit the above else
	assert(comp!=NULL);
	return 1;
}

/** Get Lua references to All Planets
 * \returns list of Planet References
 */
//int Scenario_Lua::GetPlanets(lua_State *L){
//	Planets *planets = GetScenario(L)->GetPlanets();
//	list<string>* planetNames = planets->GetNames();
//
//	lua_createtable(L, planetNames->size(), 0);
//
//	int newTable = lua_gettop(L);
//	int index = 1;
//
//	for( list<string>::iterator pname = planetNames->begin(); pname != planetNames->end(); ++pname){
//		PushSprite(L, planets->GetPlanet(*pname));
//		lua_rawseti(L, newTable, index);
//		++index;
//	}
//	return 1;
//}

/** \brief Get Information about the Simulation
 *  \returns Lua table of Information
 */
int Scenario_Lua::GetScenarioInfo(lua_State *L) {
	lua_newtable(L);

	Lua::setField("Name", GetScenario(L)->GetName().c_str() );
	Lua::setField("Description", GetScenario(L)->GetDescription().c_str() );

	return 1;
}

/** \brief Retrieve the Scenario pointer associated with a specific lua_State.
 *  \see Scenario_Lua::GetScenario
 */
Scenario *Scenario_Lua::GetScenario(lua_State *L) {
	Scenario *scenario = NULL;

	// A pointer to the simulation is stored in the LUA_REGISTRYINDEX table.
	lua_pushstring(L, "EPIAR_SIMULATION"); // Key
	lua_gettable(L, LUA_REGISTRYINDEX);

	scenario = (Scenario *)lua_topointer(L,-1);
	lua_pop(L,1);

	return scenario;
}

/** \brief Get Information about a Commodity
 *  \returns Lua table of Commodity Information
 */
int Scenario_Lua::GetCommodityInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (AllianceName)", n);
	string name = (string)luaL_checkstring(L,1);
	Commodity *commodity = GetScenario(L)->GetCommodities()->GetCommodity(name);
	if(commodity==NULL){ commodity = new Commodity(); }

	lua_newtable(L);
	Lua::setField("Name", commodity->GetName().c_str());
	Lua::setField("MSRP", commodity->GetMSRP());

	return 1;
}

/** \brief Get Information about a Alliance
 *  \returns Lua table of Alliance Information
 */
int Scenario_Lua::GetAllianceInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (AllianceName)", n);
	Color color;
	char color_buffer[9];
	string name = (string)luaL_checkstring(L,1);
	Alliance *alliance = GetScenario(L)->GetAlliances()->GetAlliance(name);
	if(alliance==NULL){ alliance = new Alliance(); }

	color = alliance->GetColor();
	snprintf(color_buffer, sizeof(color_buffer), "0x%02X%02X%02X", int(0xFF*color.r), int(0xFF*color.g), int(0xFF*color.b) );

	lua_newtable(L);
	Lua::setField("Name", alliance->GetName().c_str());
	Lua::setField("AttackSize", alliance->GetAttackSize());
	Lua::setField("Aggressiveness", alliance->GetAggressiveness());
	Lua::setField("Currency", alliance->GetCurrency().c_str() );
	Lua::setField("Color", color_buffer );

	return 1;
}

/** \brief Get Information about a Model
 *  \returns Lua table of Model Information
 */
int Scenario_Lua::GetModelInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (modelName)", n);
	string modelName = (string)luaL_checkstring(L,1);
	Model *model = GetScenario(L)->GetModels()->GetModel(modelName);
	if(model==NULL){ model = new Model(); }

	lua_newtable(L);
	Lua::setField("Name", model->GetName().c_str());
	Lua::setField("Description", model->GetDescription().c_str());
	Lua::setField("Image", (model->GetImage()!=NULL)
	                ? (model->GetImage()->GetPath().c_str())
	                : "" );
	Lua::setField("Mass", model->GetMass());
	Lua::setField("Thrust", model->GetThrustOffset());
	Lua::setField("Rotation", model->GetRotationsPerSecond());
	Lua::setField("MaxSpeed", model->GetMaxSpeed());
	Lua::setField("MaxHull", model->GetHullStrength());
	Lua::setField("MaxShield", model->GetShieldStrength());
	Lua::setField("MSRP", model->GetMSRP());
	Lua::setField("Cargo", model->GetCargoSpace());
	Lua::setField("Engine", (model->GetDefaultEngine() != NULL)
	                      ? (model->GetDefaultEngine()->GetName().c_str() )
	                      : "");
	Lua::setField("SurfaceArea", model->GetSurfaceArea());

	/* May want to move this to a helper function (but in which file?) */
	vector<WeaponSlot> slots = model->GetWeaponSlots();
	lua_pushstring(L, "weaponSlots");
	lua_newtable(L);

	int table = lua_gettop(L);

	const short int numFields = 9;

	Lua::setField("length", (int)slots.size());
	Lua::setField("fields", (int)numFields);

	char *rowKey = (char*)malloc(6);

	for(unsigned int i = 0; i < slots.size(); i++){
		WeaponSlot s = slots[i];

		snprintf(rowKey, 6, "%d", i);
		lua_pushstring(L, rowKey);

		lua_createtable(L, 0, numFields); // create a slot table

		int rowTable = lua_gettop(L);

		Lua::setField("enabled", "yes");
		Lua::setField("name", s.name.c_str() );
		Lua::setField("x", (float)s.x);
		Lua::setField("y", (float)s.y);
		Lua::setField("angle", (float)s.angle);
		Lua::setField("motionAngle", (float)s.motionAngle);
		Lua::setField("content", (s.content == NULL)
			                   ? ""
			                   : s.content->GetName().c_str() );
		Lua::setField("firingGroup", s.firingGroup);

		// keep in mind that the above field data has been popped off of the Lua state at this point

		assert( rowTable == lua_gettop(L) );

		lua_settable(L, -3);

		assert( table == lua_gettop(L) );

	}

	free(rowKey);

	lua_settable(L, -3);

	return 1;
}

/** \brief Get Information about a Planet
 *  \returns Lua table of Planet Information
 */
/*
int Scenario_Lua::GetPlanetInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (planetID)", n);

	// Figure out which planet we're fetching
	Planet* p = NULL;
	if( lua_isnumber(L,1)){
		int id = luaL_checkinteger(L,1);
		Sprite* sprite = GetScenario(L)->GetSpriteManager()->GetSpriteByID(id);
		if( sprite->GetDrawOrder() != DRAW_ORDER_PLANET)
			return luaL_error(L, "ID #%d does not point to a Planet", id);
		p = (Planet*)(sprite);
	} else if( lua_isstring(L,1)){
		string name = luaL_checkstring(L,1);
		p = GetScenario(L)->GetPlanets()->GetPlanet(name);
	}
	if(p==NULL){ p = new Planet(); }

	// Populate the Info Table.
	lua_newtable(L);
	Lua::setField("Name", p->GetName().c_str());
	Lua::setField("X", static_cast<float>(p->GetWorldPosition().GetX()));
	Lua::setField("Y", static_cast<float>(p->GetWorldPosition().GetY()));
	Lua::setField("Image", (p->GetImage()!=NULL)
	                ? (p->GetImage()->GetPath().c_str())
	                : "" );
	Lua::setField("Alliance", (p->GetAlliance()!=NULL)
	                ? (p->GetAlliance()->GetName().c_str())
	                : "" );
	Lua::setField("Traffic", p->GetTraffic());
	Lua::setField("Militia", p->GetMilitiaSize());
	Lua::setField("Landable", p->GetLandable());
	Lua::setField("Influence", p->GetInfluence());
	Lua::setField("Surface", (p->GetSurfaceImage()!=NULL)
	                ? (p->GetSurfaceImage()->GetPath().c_str())
	                : "" );
	Lua::setField("Summary", p->GetSummary().c_str());
	lua_pushstring(L, "Technologies");
	list<Technology*> techs =  p->GetTechnologies();
	PushComponents(L,  (list<Component*>*)&techs );
	lua_settable(L, -3);
	return 1;
}*/

/** \brief Get Information about a Weapon
 *  \returns Lua table of Weapon Information
 */
int Scenario_Lua::GetWeaponInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (weaponName)", n);
	string weaponName = (string)luaL_checkstring(L,1);
	Weapon* weapon = GetScenario(L)->GetWeapons()->GetWeapon(weaponName);
	if(weapon==NULL){ weapon = new Weapon(); }

	lua_newtable(L);
	Lua::setField("Name", weapon->GetName().c_str());
	Lua::setField("Description", weapon->GetDescription().c_str());
	Lua::setField("Image", (weapon->GetImage()!=NULL)
	                ? (weapon->GetImage()->GetPath().c_str())
	                : "" );
	Lua::setField("Picture", (weapon->GetPicture()!=NULL)
	                  ? (weapon->GetPicture()->GetPath().c_str())
	                  : "" );
	Lua::setField("Payload", weapon->GetPayload());
	Lua::setField("Velocity", weapon->GetVelocity());
	Lua::setField("Acceleration", weapon->GetAcceleration());
	Lua::setField("FireDelay", weapon->GetFireDelay());
	Lua::setField("Lifetime", weapon->GetLifetime());
	Lua::setField("Tracking", weapon->GetTracking());
	Lua::setField("MSRP", weapon->GetMSRP());
	Lua::setField("Type", weapon->GetType());
	Lua::setField("Ammo Type", Weapon::AmmoTypeToName(weapon->GetAmmoType()).c_str() );
	Lua::setField("Ammo Consumption", weapon->GetAmmoConsumption());
	Lua::setField("Sound", (weapon->GetSound()!=NULL)
	                ? weapon->GetSound()->GetPath().c_str()
	                : "" );
	return 1;
}

/** \brief Get Information about a Engine
 *  \returns Lua table of Engine Information
 */
int Scenario_Lua::GetEngineInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (outfitName)", n);
	string engineName = (string)luaL_checkstring(L,1);
	Engine* engine = GetScenario(L)->GetEngines()->GetEngine(engineName);
	if(engine==NULL){ engine = new Engine(); }

	lua_newtable(L);
	Lua::setField("Name", engine->GetName().c_str());
	Lua::setField("Description", engine->GetDescription().c_str());
	Lua::setField("Picture", (engine->GetPicture()!=NULL)
	                  ? (engine->GetPicture()->GetPath().c_str())
	                  : "" );
	Lua::setField("Force", engine->GetForceOutput());
	Lua::setField("Animation", engine->GetFlareAnimation().c_str());
	Lua::setField("MSRP", engine->GetMSRP());
	Lua::setField("Fold Drive", engine->GetFoldDrive());
	Lua::setField("Sound", (engine->GetSound() != NULL)
	                ? (engine->GetSound()->GetPath().c_str())
	                : "");
	return 1;
}

/** \brief Get Information about a Outfit
 *  \returns Lua table of Outfit Information
 */
int Scenario_Lua::GetOutfitInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (outfitName)", n);
	string outfitName = (string)luaL_checkstring(L,1);
	Outfit* outfit = GetScenario(L)->GetOutfits()->GetOutfit(outfitName);
	if(outfit==NULL){ outfit = new Outfit(); }

	lua_newtable(L);
	Lua::setField("Name", outfit->GetName().c_str());
	Lua::setField("Picture", (outfit->GetPicture()!=NULL)
	                  ? (outfit->GetPicture()->GetPath().c_str())
	                  : "" );
	Lua::setField("Description", outfit->GetDescription().c_str());
	Lua::setField("Force", outfit->GetForceOutput());
	Lua::setField("Mass", outfit->GetMass());
	Lua::setField("Rotation", outfit->GetRotationsPerSecond());
	Lua::setField("MaxSpeed", outfit->GetMaxSpeed());
	Lua::setField("MaxHull", outfit->GetHullStrength());
	Lua::setField("MaxShield", outfit->GetShieldStrength());
	Lua::setField("MSRP", outfit->GetMSRP());
	Lua::setField("Cargo", outfit->GetCargoSpace());
	Lua::setField("SurfaceArea", outfit->GetSurfaceArea());
	return 1;
}

/** \brief Push a list of names for a component list.
 */
void Scenario_Lua::PushComponents(lua_State *L, list<Component*> *components) {
	lua_createtable(L, components->size(), 0);

	int newTable = lua_gettop(L);
	int index = 1;

	list<Component*>::const_iterator iter = components->begin();

	while(iter != components->end()) {
		lua_pushstring(L, (*iter)->GetName().c_str());
		lua_rawseti(L, newTable, index);
		++iter;
		++index;
	}
}

/** \brief Get Information about a Technology
 *  \returns Lua table of Technology Information
 */
int Scenario_Lua::GetTechnologyInfo(lua_State *L) {
	int newTable;
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (techName)", n);
	string techName = (string)luaL_checkstring(L,1);
	Technology* tech = GetScenario(L)->GetTechnologies()->GetTechnology(techName);
	if( tech == NULL)
	{
		lua_createtable(L, 4, 0);
		newTable = lua_gettop(L);

		lua_createtable(L, 0, 0);
		lua_rawseti(L, newTable, 1);

		lua_createtable(L, 0, 0);
		lua_rawseti(L, newTable, 2);

		lua_createtable(L, 0, 0);
		lua_rawseti(L, newTable, 3);

		lua_createtable(L, 0, 0);
		lua_rawseti(L, newTable, 4);

		return 1;
	}

	lua_createtable(L, 4, 0);
	newTable = lua_gettop(L);

	// Push the Models Table
	list<Model*> models = tech->GetModels();
	PushComponents(L, (list<Component*>*) &models );
	lua_rawseti(L, newTable, 1);

	// Push the Weapons Table
	list<Weapon*> weapons = tech->GetWeapons();
	PushComponents(L, (list<Component*>*) &weapons );
	lua_rawseti(L, newTable, 2);

	// Push the Engines Table
	list<Engine*> engines = tech->GetEngines();
	PushComponents(L, (list<Component*>*) &engines );
	lua_rawseti(L, newTable, 3);

	// Push the Outfits Table
	list<Outfit*> outfits = tech->GetOutfits();
	PushComponents(L, (list<Component*>*) &outfits );
	lua_rawseti(L, newTable, 4);

	return 1;
}

/** \brief Change the Information about a Game Component
 *  \param[in] Kind of Component
 *  \param[in] Lua table of Component Information
 */
/*int Scenario_Lua::SetInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( !(n==3||n==7)  )
		return luaL_error(L, "Got %d arguments expected 1 (oldname, infoType,infoTable)", n);
	string oldname = luaL_checkstring(L,1);
	string kind = luaL_checkstring(L,2);

	if(kind == "Alliance"){
		string name = Lua::getStringField(3,"Name");
		int attack = Lua::getIntField(3,"AttackSize");
		float aggressiveness = Lua::getNumField(3,"Aggressiveness");
		string currency = Lua::getStringField(3,"Currency");
		Color color = Color( Lua::getStringField(3,"Color") );

		Alliance* thisAlliance = new Alliance(name,attack,aggressiveness,currency,color);
		GetScenario(L)->GetAlliances()->AddOrReplace( oldname, thisAlliance );

	} else if(kind == "Commodity"){
		string name = Lua::getStringField(3,"Name");
		int msrp = Lua::getIntField(3,"MSRP");

		Commodity* thisCommodity= new Commodity(name,msrp);
		GetScenario(L)->GetCommodities()->AddOrReplace( oldname, thisCommodity );

	} else if(kind == "Engine"){
		string name = Lua::getStringField(3,"Name");
		string pictureName = Lua::getStringField(3,"Picture");
		string description = Lua::getStringField(3,"Description");
		int force = Lua::getIntField(3,"Force");
		string flare = Lua::getStringField(3,"Animation");
		int msrp = Lua::getIntField(3,"MSRP");
		int foldDrive = Lua::getIntField(3,"Fold Drive");
		string soundName = Lua::getStringField(3,"Sound");

		Sound *sound = Sound::Get(soundName);
		Image *picture = Image::Get(pictureName);
		if(sound==NULL)
			LogMsg(NOTICE, "Could not create engine: there is no sound file '%s'.",soundName.c_str());
		if(picture==NULL)
			LogMsg(NOTICE, "Could not create engine: there is no image file '%s'.",pictureName.c_str());
		if((sound==NULL) || (picture==NULL))
			return 0;

		Engine* thisEngine = new Engine(name, picture, description, sound, static_cast<float>(force), msrp, TO_BOOL(foldDrive), flare);
		GetScenario(L)->GetEngines()->AddOrReplace( oldname, thisEngine );

	} else if(kind == "Model"){
		string name = Lua::getStringField(3,"Name");
		string imageName = Lua::getStringField(3,"Image");
		string description = Lua::getStringField(3,"Description");
		string engineName = Lua::getStringField(3,"Engine");
		float mass = Lua::getNumField(3,"Mass");
		int thrust = Lua::getIntField(3,"Thrust");
		float rot = Lua::getNumField(3,"Rotation");
		float speed = Lua::getNumField(3,"MaxSpeed");
		int hull = Lua::getIntField(3,"MaxHull");
		int shield = Lua::getIntField(3,"MaxShield");
		int msrp = Lua::getIntField(3,"MSRP");
		int cargo = Lua::getIntField(3,"Cargo");

		Image *image = Image::Get(imageName);
		if(image == NULL)
		{
			LogMsg(NOTICE, "Could not create model: there is no image file '%s'.",imageName.c_str());
			return 0;
		}

		Engine *engine = GetScenario(L)->GetEngines()->GetEngine( engineName );
		if(engine == NULL)
		{
			LogMsg(NOTICE, "Could not create model: there is no engine named '%s'.",imageName.c_str());
			return 0;
		}

		LogMsg(INFO, "Scenario_Lua: About to try fetching the slot table...");

		int wsTable;
		lua_pushstring(L, "weaponSlots");
		assert( lua_istable(L, 3) );
		lua_gettable(L,3);
		wsTable = lua_gettop(L);
		assert( lua_istable(L, wsTable) );
		// don't pop this table yet!

		int wsDesiredLength = Lua::getIntField(wsTable,"desiredLength");
		vector<WeaponSlot> weaponSlots;
		char *rowKey = (char*)malloc(6);
		for(short int i = 0; i < wsDesiredLength; i++){
			snprintf(rowKey, 6, "%d", i);

			//short int row = Lua::getIntField(wsTable, rowKey);
			int row;
			lua_pushstring(L, rowKey);
			assert( lua_istable(L, wsTable) );
			lua_gettable(L,wsTable);
			row = lua_gettop(L);
			// don't pop this table yet either!

			if( lua_istable(L, row) ){
				WeaponSlot s;
				s.name = Lua::getStringField(row, "name");
				s.x = Lua::getNumField(row, "x");
				s.y = Lua::getNumField(row, "y");
				s.angle = Lua::getNumField(row, "angle");
				s.motionAngle = Lua::getNumField(row, "motionAngle");
				string contentName = Lua::getStringField(row, "content");
				s.content = GetScenario(L)->GetWeapons()->GetWeapon( contentName );
				s.firingGroup = Lua::getIntField(row, "firingGroup");

				if(Lua::getStringField(row, "enabled") == "yes")
					weaponSlots.push_back(s);
			}
			// else: it's an empty row
			lua_pop(L,1);
		}
		free(rowKey);

		lua_pop(L,1);

		Model* thisModel = new Model(name, image, description, engine, mass, thrust, rot, speed, hull, shield, msrp, cargo, weaponSlots);

		GetScenario(L)->GetModels()->AddOrReplace( oldname, thisModel );

	} else if(kind == "Planet"){
		string name = Lua::getStringField(3,"Name");
		int x = Lua::getIntField(3,"X");
		int y = Lua::getIntField(3,"Y");
		string imageName = Lua::getStringField(3,"Image");
		string allianceName = Lua::getStringField(3,"Alliance");
		int traffic = Lua::getIntField(3,"Traffic");
		int militia = Lua::getIntField(3,"Militia");
		int landable = Lua::getIntField(3,"Landable");
		int influence = Lua::getIntField(3,"Influence");
		string surfaceName = Lua::getStringField(3,"Surface");
		string summary = Lua::getStringField(3,"Summary");
		list<string> techNames = Lua::getStringListField(3,"Technologies");

		// Process the Tech List
		list<Technology*> techs;
		list<string>::iterator i;
		for(i = techNames.begin(); i != techNames.end(); ++i) {
			if( NULL != GetScenario(L)->GetTechnologies()->GetTechnology(*i) ) {
				 techs.push_back( GetScenario(L)->GetTechnologies()->GetTechnology(*i) );
			} else {
				LogMsg(NOTICE, "Could not create planet: there is no Technology Group '%s'.",(*i).c_str());
				return 0;
			}
		}

		// Check that the String values actually match real hash keys.
		if(Image::Get(imageName)==NULL){
			 LogMsg(NOTICE, "Could not create planet: there is no Image at '%s'.",imageName.c_str());
			 return 0;
		}

		if(GetScenario(L)->GetAlliances()->GetAlliance(allianceName)==NULL){
			 LogMsg(NOTICE, "Could not create planet: there is no Alliance named '%s'.",allianceName.c_str());
			 return 0;
		}

		if(Image::Get(surfaceName)==NULL){
			 LogMsg(NOTICE, "Could not create planet: there is no surface at '%s'.",surfaceName.c_str());
			 return 0;
		}

		Planet thisPlanet(name,
				TO_FLOAT(x),
				TO_FLOAT(y),
				Image::Get(imageName),
				GetScenario(L)->GetAlliances()->GetAlliance(allianceName),
				TO_BOOL(landable),
				traffic,
				militia,
				influence,
				Image::Get(surfaceName),
				summary,
				techs);

		//Planet* oldPlanet = GetScenario(L)->GetPlanets()->GetPlanet( oldname );
		//if(oldPlanet!=NULL) {
		//	LogMsg(INFO,"Saving changes to '%s'",thisPlanet.GetName().c_str());
		//	*oldPlanet = thisPlanet;
		//} else {
			//LogMsg(INFO,"Creating new Planet '%s'",thisPlanet.GetName().c_str());
			//Planet* newPlanet = new Planet(thisPlanet);
			//GetScenario(L)->GetPlanets()->Add(newPlanet);
			//GetScenario(L)->GetSpriteManager()->Add(newPlanet);
		//}

	} else if(kind == "Technology"){
		list<string>::iterator iter;
		list<Model*> models;
		list<Weapon*> weapons;
		list<Engine*> engines;
		list<Outfit*> outfits;

		string name = luaL_checkstring(L,3);

		list<string> modelNames = Lua::getStringListField(4);
		for(iter=modelNames.begin();iter!=modelNames.end();++iter){
			if(GetScenario(L)->GetModels()->GetModel(*iter))
				models.push_back( GetScenario(L)->GetModels()->GetModel(*iter) );
		}

		list<string> weaponNames = Lua::getStringListField(5);
		for(iter=weaponNames.begin();iter!=weaponNames.end();++iter){
			if(GetScenario(L)->GetWeapons()->GetWeapon(*iter))
				weapons.push_back( GetScenario(L)->GetWeapons()->GetWeapon(*iter) );
		}

		list<string> engineNames = Lua::getStringListField(6);
		for(iter=engineNames.begin();iter!=engineNames.end();++iter){
			if(GetScenario(L)->GetEngines()->GetEngine(*iter))
				engines.push_back( GetScenario(L)->GetEngines()->GetEngine(*iter) );
		}

		list<string> outfitNames = Lua::getStringListField(7);
		for(iter=outfitNames.begin();iter!=outfitNames.end();++iter){
			if(GetScenario(L)->GetOutfits()->GetOutfit(*iter))
				outfits.push_back( GetScenario(L)->GetOutfits()->GetOutfit(*iter) );
		}

		Technology* thisTechnology = new Technology(name,models,engines,weapons,outfits);
		GetScenario(L)->GetTechnologies()->AddOrReplace( oldname, thisTechnology );

	} else if(kind == "Weapon"){
		string name = Lua::getStringField(3,"Name");
		string imageName = Lua::getStringField(3,"Image");
		string pictureName = Lua::getStringField(3,"Picture");
		string description = Lua::getStringField(3,"Description");
		int payload = Lua::getIntField(3,"Payload");
		int velocity = Lua::getIntField(3,"Velocity");
		int acceleration = Lua::getIntField(3,"Acceleration");
		int fireDelay = Lua::getIntField(3,"FireDelay");
		int lifetime = Lua::getIntField(3,"Lifetime");
		float tracking = Lua::getNumField(3,"Tracking");
		int msrp = Lua::getIntField(3,"MSRP");
		int type = Lua::getIntField(3,"Type");
		string ammoTypeName = Lua::getStringField(3,"Ammo Type");
		int ammoConsumption = Lua::getIntField(3,"Ammo Consumption");
		string soundName = Lua::getStringField(3,"Sound");

		Image *picture = Image::Get(pictureName);
		if(picture==NULL)
			return luaL_error(L, "Could not create weapon: there is no image file '%s'.",pictureName.c_str());
		Image *image = Image::Get(imageName);
		if(image==NULL)
			return luaL_error(L, "Could not create weapon: there is no image file '%s'.",imageName.c_str());
		if(Weapon::AmmoNameToType(ammoTypeName)==max_ammo)
			return luaL_error(L, "Could not create weapon: there is no ammo type '%s'.",ammoTypeName.c_str());
		Sound *sound = Sound::Get(soundName);
		if(sound==NULL)
			return luaL_error(L, "Could not create weapon: there is no sound file '%s'.",soundName.c_str());

		Weapon* thisWeapon = new Weapon(name, image, picture, description, type, payload, velocity, acceleration, Weapon::AmmoNameToType(ammoTypeName), ammoConsumption, fireDelay, lifetime, sound, tracking, msrp);
		GetScenario(L)->GetWeapons()->AddOrReplace( oldname, thisWeapon );

	} else if(kind == "Outfit"){
		string name = Lua::getStringField(3,"Name");
		string pictureName = Lua::getStringField(3,"Picture");
		string description = Lua::getStringField(3,"Description");
		float force = Lua::getNumField(3,"Force");
		float mass = Lua::getNumField(3,"Mass");
		float rot = Lua::getNumField(3,"Rotation");
		float speed = Lua::getNumField(3,"MaxSpeed");
		int hull = Lua::getIntField(3,"MaxHull");
		int shield = Lua::getIntField(3,"MaxShield");
		int msrp = Lua::getIntField(3,"MSRP");
		int cargo = Lua::getIntField(3,"Cargo");
		int area = Lua::getIntField(3,"SurfaceArea");

		Image *picture = Image::Get(pictureName);
		if(picture==NULL)
			return luaL_error(L, "Could not create weapon: there is no image file '%s'.",pictureName.c_str());

		Outfit* thisOutfit = new Outfit( msrp, picture, description, rot, speed, force, mass, cargo, area, hull, shield );
		thisOutfit->SetName( name );

		GetScenario(L)->GetOutfits()->AddOrReplace( oldname, thisOutfit );

	} else {
		return luaL_error(L, "Cannot set Info for kind '%s' must be one of {Alliance, Engine, Model, Planet, Technology, Weapon} ",kind.c_str());
	}
	return 0;
}*/

/** \brief Get the settings for the default player
 */
//int Scenario_Lua::GetDefaultPlayer(lua_State *L) {
	//Scenario *scenario = GetScenario(L);

	//lua_newtable(L);
	//Lua::setField("start", scenario->Get("defaultPlayer/start").c_str() );
	//Lua::setField("model", scenario->Get("defaultPlayer/model").c_str() );
	//Lua::setField("engine", scenario->Get("defaultPlayer/engine").c_str() );
	//int credits = convertTo<int>( scenario->Get("defaultPlayer/credits") );
	//Lua::setField("credits", credits );

	//return 1;
//}

//int Scenario_Lua::SetDescription(lua_State *L) {
//	string description= (string)lua_tostring(L, 1);
//	Simulation* sim = GetScenario(L);
//	sim->SetDescription( description );
//	return 0;
//}
