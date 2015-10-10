/**\file			scenario.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: July 2006
 * \date			Modified: Thursday, October 8, 2015
 * \brief			Contains the main game loop
 * \details
 */

#include "includes.h"
#include "common.h"
#include "audio/music.h"
#include "audio/audio_lua.h"
#include "engine/calendar.h"
#include "engine/hud.h"
#include "engine/scenario.h"
#include "engine/scenario_lua.h"
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

/**\class Scenario
 * \brief Handles main game loop. */

/**\brief Loads an empty Scenario.
 */
Scenario::Scenario( void ) {
	// Start the Lua Universe
	// Register these functions to their own lua namespaces
	Lua::Init();
	L = Lua::CurrentState();
	Scenario_Lua::StoreScenario(L,this);

	sprites = SpriteManager::Instance();
	commodities = Commodities::Instance();
	engines = Engines::Instance();
	planets = Planets::Instance();
	models = Models::Instance();
	weapons = Weapons::Instance();
	alliances = Alliances::Instance();
	technologies = Technologies::Instance();
	outfits = Outfits::Instance();
	sectors = Sectors::Instance();
	players = Players::Instance();
	player = NULL;

	camera = Camera::Instance();
	calendar = new Calendar();
	console = new Console( L );

	folderpath = "";

	currentFPS = 0.0f;
	paused = false;
	loaded = false;
	quit = false;

	mapScale = -1.0f;
}

bool Scenario::New( string newname ) {
	LogMsg(INFO, "New Scenario: '%s'.", newname.c_str() );

	folderpath = "data/scenario/" + newname + "/";

	XMLFile::New( folderpath + string("scenario.xml"), "scenario" );

	Set("scenario/name", newname );
	Set("scenario/description", "" );

	// Set the File Names
	commodities->SetFileName( folderpath + "commodities.xml" );
	engines->SetFileName( folderpath + "engines.xml" );
	planets->SetFileName( folderpath + "planets.xml" );
	models->SetFileName( folderpath + "models.xml" );
	weapons->SetFileName( folderpath + "weapons.xml" );
	alliances->SetFileName( folderpath + "alliances.xml" );
	technologies->SetFileName( folderpath + "technologies.xml" );
	outfits->SetFileName( folderpath + "outfits.xml" );
	sectors->SetFileName( folderpath + "sectors.xml" );

	Set("scenario/commodities", "commodities.xml" );
	Set("scenario/engines", "engines.xml" );
	Set("scenario/planets", "planets.xml" );
	Set("scenario/models", "models.xml" );
	Set("scenario/weapons", "weapons.xml" );
	Set("scenario/alliances", "alliances.xml" );
	Set("scenario/technologies", "technologies.xml" );
	Set("scenario/outfits", "outfits.xml" );
	Set("scenario/sectors", "sectors.xml" );

	Set("defaultPlayer/model", "");
	Set("defaultPlayer/engine", "");
	Set("defaultPlayer/credits", 0);
	Set("defaultPlayer/sector", "");

	Set("scenario/players", "saves/saved-games.xml" );

	loaded = true;

	return true;
}

/**\brief Loads the XML file.
 * \param filename Name of the file
 * \return true if success
 */
bool Scenario::Load( string simName ) {
	folderpath = "data/scenario/" + simName + "/";

	if( !Open( folderpath + string("scenario.xml") ) ) {
		return false;
	}

	loaded = Parse();

	return loaded;
}

/**\brief Pauses the scenario
 */
void Scenario::pause() {
	LogMsg(INFO, "Pausing.");
	paused = true;
	interpolateOn = false;
}

void Scenario::Save() {
	if( PHYSFS_mkdir( ("data/scenario/" + GetName() + "/").c_str() ) == 0) {
		LogMsg(ERR, "Cannot create folder '%s'.", folderpath.c_str() );
		return;
	}

	// Check Defaults
	if( sectors->Get( Get("defaultPlayer/sector")) == NULL)
		LogMsg(WARN, "Bad Default Player Start Location '%s'.", Get("defaultPlayer/sector").c_str() );
	if( models->Get( Get("defaultPlayer/model")) == NULL)
		LogMsg(WARN, "Bad Default Player Start Model '%s'.", Get("defaultPlayer/model").c_str() );
	if( engines->Get( Get("defaultPlayer/engine")) == NULL)
		LogMsg(WARN, "Bad Default Player Start Engine '%s'.", Get("defaultPlayer/engine").c_str() );
	// TODO Somehow check that starting credits is actually an integer.

	XMLFile::Save();
	GetAlliances()->Save();
	GetCommodities()->Save();
	GetModels()->Save();
	GetWeapons()->Save();
	GetEngines()->Save();
	GetPlanets()->Save();
	GetOutfits()->Save();
	GetSectors()->Save();
	GetTechnologies()->Save();
}

/**\brief Unpauses the scenario
 */
void Scenario::unpause() {
	LogMsg(INFO, "Unpausing.");
	//Timer::Update();
	paused = false;
	interpolateOn = true;
}

bool Scenario::SetupToRun() {
	bool luaLoad = true;

	LogMsg(INFO, "Scenario setup started ...");

	// Load default Lua registers
	LuaRegisters(L);

	// Load ::Run()-specific Lua registers
	AI_Lua::RegisterAI(L);

	luaLoad = Lua::Load("data/scripts/utilities.lua")
	       && Lua::Load("data/scripts/universe.lua")
	       && Lua::Load("data/scripts/landing.lua")
	       && Lua::Load("data/scripts/commands.lua")
	       && Lua::Load("data/scripts/ai.lua")
	       && Lua::Load("data/scripts/missions.lua")
	       && Lua::Load("data/scripts/player.lua")
	       //&& Lua::Load("data/scripts/autopilot.lua")
	       && Lua::Load("data/scripts/fleet.lua");

	if (!luaLoad) {
		LogMsg(ERR, "Fatal error starting Lua.");
		return false;
	}

	// Add Planets
	list<string>* planetNames = planets->GetNames();
	for( list<string>::iterator pname = planetNames->begin(); pname != planetNames->end(); ++pname) {
		Planet* p = planets->GetPlanet(*pname);
		sprites->Add( p );
	}

	// Preload animation to prevent FPS drop on first ship explosion
	Ani::Get("data/animations/explosion1.ani");

	// Randomize the Lua Seed
	Lua::Call("randomizeseed");

	LogMsg(INFO, "Scenario setup completed.");

	return true;
}

/**\brief Callback for Death dialog UI
 * \return void
 */
void ConfirmDeath(void *scenarioInstance) {
	((Scenario *)scenarioInstance)->SetQuit(true);
}

void Pause(void *scenarioInstance) {
	((Scenario *)scenarioInstance)->pause();
}

void Unpause(void *scenarioInstance) {
	((Scenario *)scenarioInstance)->unpause();
}

void SaveMapScale( void *scenarioInstance ) {
	Map* map = (Map*)UI::Search("/Window'Navigation'/Map/");
	if( map != NULL) {
		((Scenario *)scenarioInstance)->SetMapScale( map->GetScale() );
	} else {
		LogMsg(WARN, "Could not saving scale because the Map could not be found.\n" );
	}
}

/**\brief Main game loop
 */
void Scenario::Run() {
	int fpsCount = 0;          // for FPS calculations
	int fpsTotal= 0;           // for FPS calculations
	Uint32 fpsTS = 0;          // timestamp of last FPS printing
	fpsTS = Timer::GetTicks();

	quit = false;

	if( player != NULL ) {
		Hud::Alert("Loading %s.", player->GetName().c_str() );
		Lua::Call("playerStart");
	} else {
		LogMsg(WARN, "No Player has been loaded!");
		assert( player != NULL );
		quit = true;
		return;
	}

	Timer::Update();

	LogMsg(INFO, "Scenario started.");
	Hud::Init();

	// Message appear in reverse order, so this is upside down
	Hud::Alert("Epiar is an unfinished product. Please report all bugs at epiar.net.");

	// Generate a starfield
	Starfield starfield( 1000 );

	// Load sample game music
	if(bgmusic && OPTION(int, "options/sound/background"))
		bgmusic->Play();

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

	LogMsg(INFO,"Scenario stopped. Average framerate: %f frames / second", 1000.0 *((float)fpsTotal / Timer::GetTicks() ) );
}

bool Scenario::SetupToEdit() {
	bool luaLoad = true;

	LogMsg(INFO, "Scenario edit setup starting ...");

	// Load main Lua registers
	LuaRegisters(L);
	// Load ::Edit()-specific Lua registers
	Scenario_Lua::RegisterEditor(L);

	luaLoad = Lua::Load("data/scripts/utilities.lua")
	       && Lua::Load("data/scripts/universe.lua")
	       && Lua::Load("data/scripts/commands.lua")
	       && Lua::Load("data/scripts/editor.lua");

	if (!luaLoad) {
		LogMsg(ERR,"Fatal error starting Lua.");
		return false;
	}

	list<string>* planetNames = planets->GetNames();
	for( list<string>::iterator pname = planetNames->begin(); pname != planetNames->end(); ++pname){
		sprites->Add(  planets->GetPlanet(*pname) );
	}

	LogMsg(INFO, "Scenario edit setup completed.");

	return true;
}

bool Scenario::Edit() {
	quit = false;

	// Generate a starfield
	Starfield starfield( OPTION(int, "options/scenario/starfield-density") );

	LogMsg(INFO, "Scenario Edit Starting");

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

	LogMsg(INFO, "Scenario Edit Stopping");

	return true;
}

/**\brief Subroutine. Calls various Lua register functions needed by both Run and Edit
 * \return true if successful
 */
void Scenario::LuaRegisters(lua_State *L) {
	Scenario_Lua::RegisterScenario(L);
	UI_Lua::RegisterUI(L);
	Audio_Lua::RegisterAudio(L);
	Planets_Lua::RegisterPlanets(L);
	Hud::RegisterHud(L);
	Video::RegisterVideo(L);
}

/**\brief Parses an XML scenario file
 * \return true if successful
 */
bool Scenario::Parse( void ) {
	LogMsg(INFO, "Scenario version %s.%s.%s.", Get("version-major").c_str(), Get("version-minor").c_str(),  Get("version-macro").c_str());

	// Now load the various subsystems
	if( commodities->Load( (folderpath + Get("commodities")) ) != true ) {
		LogMsg(ERR, "There was an error loading the commodities from '%s'.", (folderpath + Get("commodities")).c_str() );
		return false;
	}
	if( engines->Load( (folderpath + Get("engines")) ) != true ) {
		LogMsg(ERR, "There was an error loading the engines from '%s'.", (folderpath + Get("engines")).c_str() );
		return false;
	}
	if( weapons->Load( (folderpath + Get("weapons")) ) != true ) {
		LogMsg(ERR, "There was an error loading the weapons from '%s'.", (folderpath + Get("weapons")).c_str() );
		return false;
	}
	if( models->Load( (folderpath + Get("models")) ) != true ) {
		LogMsg(ERR, "There was an error loading the models from '%s'.", (folderpath + Get("models")).c_str() );
		return false;
	}
	if( outfits->Load( (folderpath + Get("outfits")) ) != true ) {
		LogMsg(ERR, "There was an error loading the outfits from '%s'.", (folderpath + Get("outfits")).c_str() );
		return false;
	}
	if( technologies->Load( (folderpath + Get("technologies")) ) != true ) {
		LogMsg(ERR, "There was an error loading the technologies from '%s'.", (folderpath + Get("technologies")).c_str() );
		return false;
	}
	if( alliances->Load( (folderpath + Get("alliances")) ) != true ) {
		LogMsg(ERR, "There was an error loading the alliances from '%s'.", (folderpath + Get("alliances")).c_str() );
		return false;
	}
	if( sectors->Load( (folderpath + Get("sectors")) ) != true ) {
	    LogMsg(WARN, "There was an error loading the sectors from '%s'.", (folderpath + Get("sectors")).c_str() );
	    return false;
	}
	if( planets->Load( (folderpath + Get("planets")) ) != true ) {
	    LogMsg(WARN, "There was an error loading the planets from '%s'.", (folderpath + Get("planets")).c_str() );
	    return false;
	}

	// Check the Music
	bgmusic = Song::Get( Get("music") );
	if( bgmusic == NULL ) {
		LogMsg(WARN, "There was an error loading music from '%s'.", Get("music").c_str() );
	}

	// Check the Player Defaults
	if( sectors->Get( Get("defaultPlayer/sector")) == NULL) {
		LogMsg(ERR, "Bad Default Player Start Location '%s'.", Get("defaultPlayer/sector").c_str() );
		return false;
	}
	if( models->Get( Get("defaultPlayer/model")) == NULL) {
		LogMsg(ERR, "Bad Default Player Start Model '%s'.", Get("defaultPlayer/model").c_str() );
		return false;
	}
	if( engines->Get( Get("defaultPlayer/engine")) == NULL) {
		LogMsg(WARN, "Bad Default Player Start Engine '%s'.", Get("defaultPlayer/engine").c_str() );
		return false;
	}
	if( sectors->Get( Get("defaultPlayer/sector")) == NULL) {
		LogMsg(WARN, "Bad Default Player Start Sector '%s'.", Get("defaultPlayer/sector").c_str() );
		return false;
	}

	return true;
}

/**\brief Handle User Input
 */
void Scenario::HandleInput() {
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

	if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, 'j') ) )
	{
		list<string>* names = planets->GetNames();
		int i = 0;
		int x = rand() % names->size();
		list<string>::iterator pName = names->begin();
		while( i++ < x ){ pName++; }
		Planet* p = planets->GetPlanet(*pName);
		player->Jump( p->GetWorldPosition() + GaussianCoordinate(), true );
	}

	if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, SDLK_ESCAPE ) ) )
	{
		quit = true;
	}
}

void Scenario::CreateNavMap( void )
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

	Map* map = new Map( 10, 26,
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

/**\fn Scenario::isPaused()
 * \brief Checks to see if Scenario is paused
 * \fn Scenario::isLoaded()
 * \brief Checks to see if Scenario is Loaded Successfully
 */

/**\brief Define the new Player Attributes
 * \warn This will log any issues, but it will let
 * \param[in] playerName The player's name.
 */
void Scenario::SetDefaultPlayer( string startSector, string modelName, string engineName, int credits ) {
	LogMsg(INFO, "Setting the Player Defaults" );

	// Log disrepencies, but don't fix.
	if( sectors->Get( startSector ) == NULL )
		LogMsg(WARN, "Setting the Player's start sector to '%s', but the sector does not exist.", startSector.c_str() );
	if( models->Get( modelName ) == NULL )
		LogMsg(WARN, "Setting the Player's start model to '%s', but the model does not exist.", modelName.c_str() );
	if( engines->Get( engineName ) == NULL )
		LogMsg(WARN, "Setting the Player's start engine to '%s', but the engine does not exist.", engineName.c_str() );

	// Set player defaults in the scenario xml
	Set("defaultPlayer/sector", startSector);
	Set("defaultPlayer/model", modelName);
	Set("defaultPlayer/engine", engineName);
	Set("defaultPlayer/credits", credits);
}

/**\brief Create and Remember a new Player
 * \note This does not run the player related Lua code.
 * \warn Don't calling this more than once.
 * \param[in] playerName The player's name.
 */
void Scenario::CreateDefaultPlayer(string playerName) {
	Coordinate startPos(0,0);

	string startSector = Get("defaultPlayer/sector");
	if( sectors->GetSector( startSector ) ) {
		//startPos = planets->GetPlanet( startPlanet )->GetWorldPosition();
	}

	assert( player == NULL );
	assert( models->GetModel( Get("defaultPlayer/model") ) );
	assert( engines->GetEngine( Get("defaultPlayer/engine") ) );

	player = players->CreateNew(
		GetName(),
		playerName,
		models->GetModel( Get("defaultPlayer/model") ),
		engines->GetEngine( Get("defaultPlayer/engine") ),
		convertTo<int>( Get("defaultPlayer/credits") ),
		startPos
	);

	sprites->AddPlayer( player );
	camera->Focus( player );
}

/**\brief Load Create and Remember a new Player
 * \note This does not run any of the Lua code.
 * \warn Don't calling this more than once.
 * \param[in] playerName The player's name.
 */
void Scenario::LoadPlayer(string playerName) {
	assert( player == NULL );
	player = players->LoadPlayer( playerName );
	sprites->Add( player );
	camera->Focus( player );
}

/**\brief
 * \return true if the player wants to quit
 */
Player *Scenario::GetPlayer() {
	if ( player == NULL ) {
	    LogMsg(ERR, "No Player has been loaded!");
	}
	assert( player != NULL );
	return player;
}
