/**\filename			scenario.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: July 2006
 * \date			Modified: Thursday, December 3, 2015
 * \brief			Contains the main game loop
 * \details
 */

#ifndef __H_SCENARIO__
#define __H_SCENARIO__

#include "engine/commodities.h"
#include "engine/alliances.h"
#include "engine/engines.h"
#include "engine/models.h"
#include "engine/calendar.h"
#include "sprites/planets.h"
#include "engine/weapons.h"
#include "engine/technologies.h"
#include "engine/sectors.h"
#include "sprites/player.h"
#include "audio/music.h"
#include "engine/camera.h"
#include "input/input.h"
#include "engine/console.h"

class Scenario : public XMLFile {
	public:
		Scenario();
		~Scenario();

		bool New( string _folderpath );
		bool Load( string _folderpath );

		bool Initialize();
		bool Setup();

		void Run();

		void CreateDefaultPlayer(string name);
		void LoadPlayer(string name);

		void LuaRegisters(lua_State *L);

		void HandleInput();

		void Save();
		void pause();
		void unpause();

		bool isPaused() { return paused; }
		bool isLoaded() { return loaded; }

		SpriteManager *GetSpriteManager() { return sprites; }
		Commodities *GetCommodities() { return commodities; }
		Planets *GetPlanets() { return planets; }
		Engines *GetEngines() { return engines; }
		Models *GetModels() { return models; }
		Weapons *GetWeapons() { return weapons; }
		Alliances *GetAlliances() { return alliances; }
		Technologies *GetTechnologies() { return technologies; }
		Outfits *GetOutfits() { return outfits; }
		Sectors *GetSectors() { return sectors; }
		PlayerList *GetPlayerList() { return playerList; }
		Camera *GetCamera() { return camera; }
		Player *GetPlayer();

		Sector* GetCurrentSector();

		void ResetSector( Sector *s );

		string GetName() { return Get("scenario/name"); }
		string GetDescription() { return Get("scenario/description"); }

		void SetDescription( string _desc ) { Set("scenario/description", _desc ); }
		void SetMusic( string _path ) { Set("music", _path); }
		void SetDefaultPlayer( string planetName, string modelName, string engineName, int credits );

		void SetMapScale( float scale ) { mapScale = scale; }

		void SetQuit( bool val ) { quit = val; }

	private:
		bool ParseXML( void );
		void CreateNavMap( void );

		// Pointers to Singletons
		lua_State *luaState;
		SpriteManager *sprites;

		Commodities *commodities;
		Planets *planets;
		Engines *engines;
		Models *models;
		Weapons *weapons;
		Alliances *alliances;
		Technologies *technologies;
		Outfits *outfits;
		Sectors *sectors;
		PlayerList *playerList;
		Player *player;
		Camera *camera;
		Calendar *calendar;

		// Scenario specific variables
		Song* bgmusic;
		Console *console;
		Sector* currentSector;

		// Description of this scenario
		string folderpath;

		// State Variables
		float currentFPS;
		bool paused;
		bool loaded;
		bool quit;
		float mapScale;
};

#endif // __H_SCENARIO__
