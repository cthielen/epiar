/**\filename		simulation.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: July 2006
 * \date			Modified: Tuesday, June 23, 2009
 * \brief			Contains the main game loop
 * \details
 */

#ifndef __H_SIMULATION__
#define __H_SIMULATION__

#include "engine/calendar.h"
#include "engine/scenario.h"
#include "sprites/player.h"
#include "audio/music.h"
#include "engine/camera.h"
#include "input/input.h"
#include "engine/console.h"

class Simulation : public XMLFile {
	public:
		Simulation();

		//bool New( string _folderpath );
		bool LoadScenario( string _folderpath );

		bool SetupToRun();
		bool SetupToEdit();

		bool Run();
		bool Edit();

		void CreateDefaultPlayer(string name);
		void LoadPlayer(string name);

		void LuaRegisters(lua_State *L);

		void HandleInput();

		void Save();
		void pause();
		void unpause();
		bool isPaused() {return paused;}
		bool isLoaded() {return loaded;}

		SpriteManager *GetSpriteManager() { return sprites; }
		Scenario *GetScenario() { return scenario; }
		Players *GetPlayers() { return players; }
		Camera *GetCamera() { return camera; }
		Input *GetInput() { return &inputs; }
		Player *GetPlayer();

		void SetMusic( string _path ) { Set("music", _path); }
		void SetDefaultPlayer( string planetName, string modelName, string engineName, int credits );

		void SetMapScale( float scale ) { mapScale = scale; }

		void SetQuit( bool val ) { quit = val; }

	private:
		bool Parse( void );
		void CreateNavMap( void );

		// Pointers to Singletons
		///< TODO: These should all be rewritten to not be singletons
		lua_State *L;
		SpriteManager *sprites;

		Scenario *scenario;
		Players *players;
		Player *player;
		Camera *camera;
		Calendar *calendar;

		// Simulation specific variables
		Input inputs;
		Console *console;

		// State Variables
		float currentFPS;
		bool paused;
		bool loaded;
		bool quit;
		float mapScale;
};

#endif // __H_SIMULATION__
