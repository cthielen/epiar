/**\file			simulation.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: July 2006
 * \date			Modified: Sunday, August 17, 2014
 * \brief			Contains the main game loop
 * \details
 */

#include "includes.h"
#include "common.h"
#include "audio/music.h"
#include "audio/audio_lua.h"
#include "engine/calendar.h"
#include "engine/hud.h"
#include "engine/simulation.h"
#include "engine/simulation_lua.h"
#include "engine/commodities.h"
#include "engine/alliances.h"
#include "engine/technologies.h"
#include "engine/starfield.h"
#include "engine/console.h"
#include "graphics/video.h"
#include "sprites/ai.h"
#include "sprites/ai_lua.h"
#include "sprites/effects.h"
#include "sprites/player.h"
#include "sprites/planets.h"
#include "sprites/planets_lua.h"
#include "sprites/spritemanager.h"
#include "ui/ui.h"
#include "ui/widgets.h"
#include "utilities/file.h"
#include "utilities/log.h"
#include "utilities/timer.h"
#include "utilities/lua.h"

/**\class Simulation
 * \brief Handles main game loop. */

/**\brief Loads an empty Simulation.
 */
Simulation::Simulation( void ) {
	// Start the Lua Universe
	// Register these functions to their own lua namespaces
	Lua::Init();
	L = Lua::CurrentState();
	Simulation_Lua::StoreSimulation(L,this);

	sprites = SpriteManager::Instance();
	scenario = new Scenario();

	players = Players::Instance();
	player = NULL;

	camera = Camera::Instance();
	calendar = new Calendar();
	console = new Console( L );

	currentFPS = 0.0f;
	paused = false;
	loaded = false;
	quit = false;

	mapScale = -1.0f;
}

/*bool Simulation::New( string newname ) {
	LogMsg(INFO, "New Simulation: '%s'.", newname.c_str() );

	XMLFile::New( folderpath + string("scenario.xml"), "scenario" );

	Set("scenario/name", newname );
	Set("scenario/description", "" );

	// Set the File Names
	commodities->SetFileName( folderpath + "commodities.xml" );
	engines->SetFileName( folderpath + "engines.xml" );
	map->SetFileName( folderpath + "map.xml" );
	models->SetFileName( folderpath + "models.xml" );
	weapons->SetFileName( folderpath + "weapons.xml" );
	alliances->SetFileName( folderpath + "alliances.xml" );
	technologies->SetFileName( folderpath + "technologies.xml" );
	outfits->SetFileName( folderpath + "outfits.xml" );

	Set("scenario/commodities", "commodities.xml" );
	Set("scenario/engines", "engines.xml" );
	Set("scenario/map", "map.xml" );
	Set("scenario/models", "models.xml" );
	Set("scenario/weapons", "weapons.xml" );
	Set("scenario/alliances", "alliances.xml" );
	Set("scenario/technologies", "technologies.xml" );
	Set("scenario/outfits", "outfits.xml" );

	Set("defaultPlayer/start", "");
	Set("defaultPlayer/model", "");
	Set("defaultPlayer/engine", "");
	Set("defaultPlayer/credits", 0);

	Set("scenario/players", "resources/definitions/saved-games.xml" );

	loaded = true;
	return true;
}*/

/**\brief Loads the XML file.
 * \param filename Name of the file
 * \return true if success
 */
bool Simulation::LoadScenario( string scenarioName ) {
	if( !Open( "resources/scenarios/" + scenarioName + "/" + string("scenario.xml") ) ) {
		return false;
	}

	loaded = Parse();

	return loaded;
}

/**\brief Pauses the simulation
 */
void Simulation::pause() {
	LogMsg(INFO, "Pausing.");
	paused = true;
	interpolateOn = false;
}

void Simulation::Save() {
	LogMsg(ERR, "Unimplemented.");

	//if( PHYSFS_mkdir( ("resources/scenarios/" + GetName() + "/").c_str() ) == 0) {
	//	LogMsg(ERR, "Cannot create folder '%s'.", folderpath.c_str() );
	//	return;
	//}

	// Check Defaults
	//if( planets->Get( Get("defaultPlayer/start")) == NULL)
	//	LogMsg(WARN, "Bad Default Player Start Location '%s'.", Get("defaultPlayer/start").c_str() );
	//if( models->Get( Get("defaultPlayer/model")) == NULL)
	//	LogMsg(WARN, "Bad Default Player Start Model '%s'.", Get("defaultPlayer/model").c_str() );
	//if( engines->Get( Get("defaultPlayer/engine")) == NULL)
	//	LogMsg(WARN, "Bad Default Player Start Engine '%s'.", Get("defaultPlayer/engine").c_str() );
	// TODO Somehow check that starting credits is actually an integer.

	/*XMLFile::Save();
	GetAlliances()->Save();
	GetCommodities()->Save();
	GetModels()->Save();
	GetWeapons()->Save();
	GetEngines()->Save();*/
	//GetPlanets()->Save();
	/*GetOutfits()->Save();
	GetTechnologies()->Save();*/
}

/**\brief Unpauses the simulation
 */
void Simulation::unpause() {
	LogMsg(INFO, "Unpausing.");
	//Timer::Update();
	paused = false;
	interpolateOn = true;
}

bool Simulation::SetupToRun() {
	bool luaLoad = true;

	LogMsg(INFO, "Simulation Setup Started");

	Timer::Update(); // Start the Timer

	// Load default Lua registers
	LuaRegisters(L);
	// Load ::Run()-specific Lua registers
	AI_Lua::RegisterAI(L);

	luaLoad = Lua::Load("resources/scripts/utilities.lua")
	       && Lua::Load("resources/scripts/universe.lua")
	       && Lua::Load("resources/scripts/landing.lua")
	       && Lua::Load("resources/scripts/commands.lua")
	       && Lua::Load("resources/scripts/ai.lua")
	       && Lua::Load("resources/scripts/missions.lua")
	       && Lua::Load("resources/scripts/player.lua")
	       && Lua::Load("resources/scripts/autopilot.lua")
	       && Lua::Load("resources/scripts/fleet.lua");

	if (!luaLoad) {
		LogMsg(ERR,"Fatal error starting Lua.");
		return false;
	}

	// Add Planets
    //list<string>* planetNames = planets->GetNames();
    //for( list<string>::iterator pname = planetNames->begin(); pname != planetNames->end(); ++pname) {
	//	Planet* p = planets->GetPlanet(*pname);
	 //   sprites->Add( p );
    //}

	// Preloading this animation prevents an FPS
	// drop the first time that a ship explodes.
	Ani::Get("resources/animations/explosion1.ani");

	// Randomize the Lua Seed
	Lua::Call("randomizeseed");

	LogMsg(INFO, "Simulation Setup Complete");

	return true;
}

/**\brief Callback for Death dialog UI
 * \return void
 */
void ConfirmDeath(void *simulationInstance) {
	((Simulation *)simulationInstance)->SetQuit(true);
}

void Pause(void *simulationInstance) {
	((Simulation *)simulationInstance)->pause();
}

void Unpause(void *simulationInstance) {
	((Simulation *)simulationInstance)->unpause();
}

void SaveMapScale( void *simulationInstance ) {
	NavMap* map = (NavMap*)UI::Search("/Window'Navigation'/NavMap/");
	if( map != NULL) {
		((Simulation *)simulationInstance)->SetMapScale( map->GetScale() );
	} else {
		LogMsg(WARN, "Could not saving scale because the Map could not be found.\n" );
	}
}

/**\brief Main game loop
 * \return true if the player is still alive
 */
bool Simulation::Run() {
	int fpsCount = 0; // for FPS calculations
	int fpsTotal= 0; // for FPS calculations
	Uint32 fpsTS = 0; // timestamp of last FPS printing
	fpsTS = Timer::GetTicks();

	quit = false;

	LogMsg(INFO, "Simulation Started");
	Hud::Init();

	if( player != NULL )
	{
		Hud::Alert("Loading %s.", player->GetName().c_str() );
		Lua::Call("playerStart");
	}
	else
	{
		LogMsg(WARN, "No Player has been loaded!");
		assert( player != NULL );
		quit = true;
	}

	// Message appear in reverse order, so this is upside down
	Hud::Alert("Epiar is currently under development. Please report all bugs to epiar.net");

	// Generate a starfield
	Starfield starfield( OPTION(int, "options/simulation/starfield-density") );

	// Load sample game music
	if(scenario->GetBgMusic() && OPTION(int, "options/sound/background"))
		scenario->GetBgMusic()->Play();

	// main game loop
	bool lowFps = false;
	int lowFpsFrameCount = 0;
	while( !quit ) {
		int logicLoops = Timer::Update();
		if( !paused ) {
			// Logical update cycle
			while(logicLoops--) {
				HandleInput();

				if (lowFps) {
					lowFpsFrameCount--;
				}

				sprites->Update( L, lowFps );
				camera->Update( sprites );
				sprites->UpdateScreenCoordinates();
				calendar->Update();
				starfield.Update( camera );
			}
		} else {
			// We prefer input to be inside the logic loop (for the Hz) but
			// we need to respond to input outside it as well.
			HandleInput();
		}

		Hud::Update( L );

		// Erase cycle
		Video::Erase();

		// Draw cycle
		Video::PreDraw();
		starfield.Draw();
		sprites->Draw( camera->GetFocusCoordinate() );
		Hud::Draw( HUD_ALL, currentFPS, camera, sprites );
		UI::Draw();
		console->Draw();
		Video::Update();

		Timer::Delay();

		// Counting Frames
		fpsCount++;
		fpsTotal++;

		// Update the fps once per second
		if( (Timer::GetTicks() - fpsTS) > 1000 ) {
			currentFPS = static_cast<float>(1000.0 *
					((float)fpsCount / (Timer::GetTicks() - fpsTS)));
			fpsTS = Timer::GetTicks();
			fpsCount = 0;
			if( currentFPS < -0.1f )
			{
				// The game has effectively stopped..
				LogMsg(ERR, "The framerate has dropped to zero. Please report this as a bug to 'epiar-devel@epiar.net'");
				UI::Save();
				sprites->Save();
				quit = true;
			}

			/**************************
			 * Low FPS calculation
			 *  - if fps goes below 15, set lowFps to true for 600 logical frames
			 *  - after 600 frames, either turn it off or leave it on for another 600
			 **************************/
			if (lowFps)
			{
				if (lowFpsFrameCount <= 0)
				{
					LogMsg (DEBUG4, "Turning off wave-updates for sprites as 600 frames have passed");
					lowFps = false;
				}
			}

			if (!lowFps && currentFPS < 15)
			{
				LogMsg (DEBUG4, "Turning on wave-updates for sprites as FPS has gone below 15");
				lowFps = true;			//if FPS has dropped below 15 then switch to wave-update method for 600 frames
				lowFpsFrameCount = 600;
			}
				/************************
				 * End Low FPS calculation
				 ************************/

			if( OPTION(int, "options/log/ui") )
			{
				UI::Save();
			}

			if( OPTION(int, "options/log/sprites") )
			{
				sprites->Save();
			}

			// Check to see if the player is dead
			if( player->GetHullIntegrityPct() <= 0 ) {
				if( UI::Search("/Window'Death'/") == NULL ) {
					Window* win = new Window( Video::GetWidth()/2-125, Video::GetHeight()/2-70, 250, 140, "Death");
					Button* ok = new Button(70, 85, 100, 30, "Dang!", ConfirmDeath, this);
					UI::Add( win );

					// Player Name
					win->AddChild( (new Label(80, 30, "You have died.")) );
					win->AddChild( ok );
					win->RegisterAction(Action_Close, new ObjectAction(ConfirmDeath, this) );
					win->SetFormButton( ok );
					UI::RegisterKeyboardFocus( win );
				}
			}
		}
	}

	Hud::Close();

	LogMsg(INFO,"Simulation Stopped: Average Framerate: %f Frames/Second", 1000.0 *((float)fpsTotal / Timer::GetTicks() ) );

	return (player->GetHullIntegrityPct() > 0);
}

bool Simulation::SetupToEdit() {
	bool luaLoad = true;

	LogMsg(INFO, "Simulation Edit Setup Starting");

	// Load main Lua registers
	LuaRegisters(L);
	// Load ::Edit()-specific Lua registers
	Simulation_Lua::RegisterEditor(L);

	luaLoad = Lua::Load("resources/scripts/utilities.lua")
	       && Lua::Load("resources/scripts/universe.lua")
	       && Lua::Load("resources/scripts/commands.lua")
	       && Lua::Load("resources/scripts/editor.lua");

	if (!luaLoad) {
		LogMsg(ERR,"Fatal error starting Lua.");
		return false;
	}

	//list<string>* planetNames = planets->GetNames();
	//for( list<string>::iterator pname = planetNames->begin(); pname != planetNames->end(); ++pname){
	//	sprites->Add(  planets->GetPlanet(*pname) );
	//}

	LogMsg(INFO, "Simulation Edit Setup Complete");

	return true;
}

bool Simulation::Edit() {
	quit = false;

	// Generate a starfield
	Starfield starfield( OPTION(int, "options/simulation/starfield-density") );

	LogMsg(INFO, "Simulation Edit Starting");

	Lua::Call("componentDebugger");

	while( !quit ) {
		HandleInput();

		Timer::Update();

		sprites->Update( L, true );
		camera->Update( sprites );
    sprites->UpdateScreenCoordinates();
    starfield.Update( camera );
		Hud::Update( L );

		// Erase cycle
		Video::Erase();

		// Draw cycle
		starfield.Draw();
		sprites->Draw( camera->GetFocusCoordinate() );
		UI::Draw();
		Hud::Draw( HUD_Target | HUD_Map, 0.0f, camera, sprites );
		console->Draw();
		Video::Update();

		Timer::Delay();
	}

	LogMsg(INFO, "Simulation Edit Stopping");

	return true;
}

/**\brief Subroutine. Calls various Lua register functions needed by both Run and Edit
 * \return true if successful
 */
void Simulation::LuaRegisters(lua_State *L) {
	Simulation_Lua::RegisterSimulation(L);
	UI_Lua::RegisterUI(L);
	Audio_Lua::RegisterAudio(L);
	Planets_Lua::RegisterPlanets(L);
	Hud::RegisterHud(L);
	Video::RegisterVideo(L);
}

/**\brief Parses an XML simulation file
 * \return true if successful
 */
bool Simulation::Parse( void ) {
	assert(false);

	LogMsg(INFO, "Simulation version %s.%s.%s.", Get("version-major").c_str(), Get("version-minor").c_str(),  Get("version-macro").c_str());

	return false;
}

/**\brief Handle User Input
 */
void Simulation::HandleInput() {
	list<InputEvent> events;

	// Collect user input events
	events = inputs.Update();

	// Pass the Events to the systems that handle them.
	UI::HandleInput( events );
	console->HandleInput( events );
	Hud::HandleInput( events, camera, sprites );

	if( !paused )
	{
		inputs.HandleLuaCallBacks( events );
	}

	//if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, SDLK_PERIOD ) ) )
	//{
		//Video::SaveScreenshot();
	//}

	if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, 'n') ) )
	{
		CreateNavMap();
	}

	if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, 'p') ) )
	{
		if ( UI::Search("/Window'Epiar is Paused'/") )
		{
			UI::Close( UI::Search("/Window'Epiar is Paused'/") );
		}
		else
		{
			Window* win = new Window(
				TO_INT(Video::GetWidth() * 0.4),
				TO_INT(Video::GetHeight() * 0.4),
				TO_INT(Video::GetWidth() * 0.2),
				TO_INT(Video::GetHeight() * 0.2),
				"Epiar is Paused" );
			Button* ok = new Button(
				TO_INT(win->GetW()/2) -50,
				TO_INT(win->GetH()/2) -15,
				100,
				30,
				"Unpause", UI::Close, win);
			win->AddChild( ok );
			win->RegisterAction(Action_Close, new ObjectAction(Unpause, this) );
			win->SetFormButton( ok );

			pause();
			UI::Add( win );
			UI::RegisterKeyboardFocus( win );
		}
	}

	if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, '?') ) )
	{
		Dialogs::OptionsWindow();
	}

	if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, '/') ) )
	{
		Lua::Call("keyboardCommands");
	}

	//if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, 'j') ) )
	//{
	//	list<string>* names = planets->GetNames();
	//	int i = 0;
	//	int x = rand() % names->size();
	//	list<string>::iterator pName = names->begin();
	//	while( i++ < x ){ pName++; }
	//	Planet* p = planets->GetPlanet(*pName);
	//	player->Jump( p->GetWorldPosition() + GaussianCoordinate()*p->GetInfluence(), true );
	//}

	if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, SDLK_ESCAPE ) ) )
	{
		quit = true;
	}
}

void Simulation::CreateNavMap( void )
{
	// Toggle NavMap off if it already exists
	if( UI::Search("/Window'Navigation'/") )
	{
		UI::Close( UI::Search("/Window'Navigation'/") );
		return;
	}

	Window* win = new Window(
		TO_INT(Video::GetWidth() * 0.1),
		TO_INT(Video::GetHeight() * 0.1),
		TO_INT(Video::GetWidth() * 0.8),
		TO_INT(Video::GetHeight() * 0.8),
		"Navigation" );

	NavMap* map = new NavMap( 10, 26,
		TO_INT(win->GetW()) - 18,
		TO_INT(win->GetH()) - 33,
		camera->GetFocusCoordinate(),
		sprites );

	// Restore Saved Scale
	if( mapScale > 0.0f ) {
		map->SetScale( mapScale );
	}
	map->RegisterAction(Action_MouseWDown, new ObjectAction(SaveMapScale, this) );
	map->RegisterAction(Action_MouseWUp, new ObjectAction(SaveMapScale, this) );

	win->AddChild( map );
	win->AddCloseButton();


	// Pause now, but unpause when this window is closed.
	pause();
	win->RegisterAction(Action_Close, new ObjectAction(Unpause, this) );
	UI::Add( win );

	// Alternatively: make the Map a Modal Widget.  But this is not as nice.
	//win->RegisterAction(Widget::Action_Close, new VoidAction(UI::ReleaseModality) );
	//UI::ModalDialog( win );
}

/**\fn Simulation::isPaused()
 * \brief Checks to see if Simulation is paused
 * \fn Simulation::isLoaded()
 * \brief Checks to see if Simulation is Loaded Successfully
 */

/**\brief Define the new Player Attributes
 * \warn This will log any issues, but it will let
 * \param[in] playerName The player's name.
 */
void Simulation::SetDefaultPlayer( string startPlanet, string modelName, string engineName, int credits ) {
	LogMsg(INFO, "Setting the Player Defaults" );

	assert(false);

	// Log disrepencies, but don't fix.
	//if( planets->Get( startPlanet ) == NULL )
	//	LogMsg(WARN, "Setting the Player's start planet to '%s', but this planet does not exist.", startPlanet.c_str() );
	//if( models->Get( modelName ) == NULL )
	//	LogMsg(WARN, "Setting the Player's start model to '%s', but this model does not exist.", modelName.c_str() );
	//if( engines->Get( engineName ) == NULL )
	//	LogMsg(WARN, "Setting the Player's start engine to '%s', but this engine does not exist.", engineName.c_str() );

	// Set player defaults in the scenario xml
	//Set("defaultPlayer/start", startPlanet);
	//Set("defaultPlayer/model", modelName);
	//Set("defaultPlayer/engine", engineName);
	//Set("defaultPlayer/credits", credits);
}

/**\brief Create and Remember a new Player
 * \note This does not run the player related Lua code.
 * \warn Don't calling this more than once.
 * \param[in] playerName The player's name.
 */
void Simulation::CreateDefaultPlayer(string playerName) {
	assert(false);

	//Coordinate startPos(0,0);

	//string startPlanet = Get("defaultPlayer/start");
	//if( planets->GetPlanet( startPlanet ) ) {
	//	startPos = planets->GetPlanet( startPlanet )->GetWorldPosition();
	//}

	//assert( player == NULL );
	//assert( models->GetModel( Get("defaultPlayer/model") ) );
	//assert( engines->GetEngine( Get("defaultPlayer/engine") ) );

	//player = players->CreateNew(
	//	GetName(),
	//	playerName,
	//	models->GetModel( Get("defaultPlayer/model") ),
	//	engines->GetEngine( Get("defaultPlayer/engine") ),
	//	convertTo<int>( Get("defaultPlayer/credits") ),
	//	startPos
	//);

	//sprites->AddPlayer( player );
	//camera->Focus( player );
}

/**\brief Load Create and Remember a new Player
 * \note This does not run any of the Lua code.
 * \warn Don't calling this more than once.
 * \param[in] playerName The player's name.
 */
void Simulation::LoadPlayer(string playerName) {
	assert( player == NULL );
	player = players->LoadPlayer( playerName );
	sprites->Add( player );
	camera->Focus( player );
}

/**\brief
 * \return true if the player wants to quit
 */
Player *Simulation::GetPlayer() {
	if ( player == NULL ) {
	    LogMsg(WARN, "No Player has been loaded!");
	}
	assert( player != NULL );
	return player;
}
