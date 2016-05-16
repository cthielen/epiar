/**\file		menu.cpp
 * \author		Christopher Thielen (chris@epiar.net)
 * \author		and others.
 * \date		Created: Tuesday, April 26, 2011
 * \date		Modified: Saturday, April 2, 2016
 * \brief		Runs the main menu
 * \details
 */

#include "includes.h"
#include "engine/scenario.h"
#include "menu.h"
#include "ui/ui.h"
#include "ui/widgets.h"
#include "utilities/filesystem.h"
#include "utilities/timer.h"

bool Menu::quit = false;

Scenario* Menu::scenario = NULL;
PlayerInfo* Menu::playerToLoad = NULL;

Image* Menu::menuSplash = NULL;

Picture* Menu::play = NULL;
Picture* Menu::load = NULL;
Picture* Menu::options = NULL;
Picture* Menu::exit = NULL;

Song *Menu::bgMusic = NULL;

/**\class Menu
 *  \brief Epiar's Main Menu
 *
 * 	\details
 *  This runs a while(1) loop collecting user input and drawing the screen.
 *  While similar to the Run Loop in the Scenario, this should be simpler
 *  since there is no HUD, Console or Sprites.
 *
 *  The Main Menu will launch the Scenario with a new or loaded Player.
 *  It can also edit scenarios and option.
 *
 *  The Main Menu can be skipped by enabling the "automatic-load" option.
 *
 */

/** The Main Loop for the Menu screen.
 * \note Press ESC or hit the 'Quit' button to quit.
 */
void Menu::Run( void ) {
	Input inputs;
	list<InputEvent> events;
    
	quit = false;

	PlayerList *playerList = PlayerList::Instance();
	playerList->Load( "saves/saved-games.xml", true, true);

	if( OPTION(int,"options/scenario/automatic-load") ) {
		if( AutoLoad() ) {
			LogMsg(INFO,"Auto-loaded game finished. Quitting ...");
			return;
		}
	}

	// Set up music
	bgMusic = Song::Get( "data/audio/music/holst_mars.ogg" );
	if( bgMusic == NULL ) {
		LogMsg(WARN, "There was an error loading music from 'data/audio/music/holst_mars.ogg'." );
	}

	SetupUI();

	bgMusic->Play(); // play the music

	// Input Loop
	do {
		// Collect user input events
		events = inputs.Update();
		UI::HandleInput( events );

		// Draw Things
		int loops = Timer::Update();
		if(loops) {
			Video::Erase();
			UI::Draw();
			Video::Update();
		}

		if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, SDLK_ESCAPE ) ) ) {
			quit = true;
		}

		// Wait until the next click
		Timer::Delay();
	} while(quit == false);
}

/** Returns the currently loaded scenraio, if any.
 */
Scenario* Menu::GetCurrentScenario( void ) {
	return scenario;
}

/** Load the most recent Player
 * \note When the user leaves the Scenario, the game will quit.
 * \returns true if the player was loaded successfully.
 */
bool Menu::AutoLoad() {
	LogMsg(INFO,"Attempting to automatically load a player.");
	PlayerInfo* info = PlayerList::Instance()->LastPlayer();

	if( info != NULL )
	{
		LogMsg(INFO, "Automatically loading player.", info->GetName().c_str() );

		assert(scenario == NULL);
		scenario = new Scenario();

		if( !scenario->Load( info->scenario ) )
		{
			LogMsg(ERR,"Failed to load the scenario '%s'.", info->scenario.c_str() );
			delete scenario; scenario = NULL;
			return false;
		}

		if( !scenario->Initialize() )
		{
			LogMsg(ERR,"Failed to setup the scenario '%s'.", info->scenario.c_str() );
			delete scenario; scenario = NULL;
			return false;
		}

		scenario->LoadPlayer( info->GetName() );
		scenario->Setup();
		scenario->Run();

		return true;
	}

	LogMsg(WARN,"No available players to load.");

	return false;
}

/** Create the Basic Main Menu
 *  \details The Splash Screen is random.
 */
void Menu::SetupUI() {
	int button_x = Video::GetWidth() - 300;
	int button_y = Video::GetHalfHeight() - 175; // the menu buttons are about 500px tall

	string splashScreens[] = {
		"data/art/menu1.png",
		"data/art/menu2.png",
		"data/art/menu3.png",
		"data/art/menu4.png",
		"data/art/menu5.png",
		"data/art/menu6.png",
	};

	int numScreens = (sizeof(splashScreens) / sizeof(splashScreens[0]));
	menuSplash = Image::Get( splashScreens[rand() % numScreens] );

	// Add the splash screen
	UI::Add( new Picture( 0, 0, Video::GetWidth(), Video::GetHeight(), menuSplash, true ) );

	// Add the logo
	UI::Add( new Picture( 20, Video::GetHeight() - 120, Image::Get("data/art/logo.png" ) ) );

	// New Button
	play = PictureButton( button_x, button_y + 50, Menu::CreateNewWindow,
	                      Image::Get( "data/graphics/txt_new_game_active.png"),
	                      Image::Get( "data/graphics/txt_new_game_inactive.png") );

	// Load Button
	load = PictureButton( button_x, button_y + 100, Menu::CreateLoadWindow,
	                       Image::Get( "data/graphics/txt_load_game_active.png"),
	                       Image::Get( "data/graphics/txt_load_game_inactive.png") );

	// Options Button
	options = PictureButton( button_x, button_y + 150, Dialogs::OptionsWindow,
	                          Image::Get( "data/graphics/txt_options_active.png"),
	                          Image::Get( "data/graphics/txt_options_inactive.png") );

	// Exit Button
	exit = PictureButton( button_x, button_y + 200, QuitMenu,
	                      Image::Get( "data/graphics/txt_exit_active.png"),
	                      Image::Get( "data/graphics/txt_exit_inactive.png") );
}

/** This Window is used to create new Players.
 */
void Menu::CreateNewWindow() {
	if( UI::Search("/Window'New Game'/") != NULL ) return;

	Window* win = new Window(250, 180, "New Game");
	UI::Add( win );

	// Player Name
	win->AddChild( (new Label(50, 45, "Name:")) )
		->AddChild( (new Textbox(90, 41, 130, 1, "Avatar", "Player Name:")) );

	// Scenario Picker
	win->AddChild( (new Label(32, 80, "Scenario:")) )
		->AddChild( (new Dropdown( 90, 75, 130, 25 ))
			->AddOptions( Filesystem::Enumerate("data/scenario/") )
	);

	win->AddChild( (new Button( 20, 130, 100, 30, "Cancel", &UI::Close, win)) );
	win->AddChild( (new Button(130, 130, 100, 30, "Start", &CreateNewPlayer )) );
}

/** This Window shows a list of potential Players.
 */
void Menu::CreateLoadWindow() {
	int infoRows = 0;

	if( UI::Search("/Window'Load Game'/") != NULL ) return;

	list<string> *names = PlayerList::Instance()->GetNames();

	infoRows = names->size() > 0 ? names->size() : 1;

	Window* win = new Window(450, 95 + (infoRows * 130), "Load Game");
	UI::Add( win );

	PlayerInfo* last = PlayerList::Instance()->LastPlayer();

	// Create a new Frame for each Player
	int p = 0;
	list<string>::iterator iter;

	for( iter = names->begin(); iter != names->end(); ++iter, ++p ) {
		PlayerInfo *info = PlayerList::Instance()->GetPlayerInfo( *iter );

		win->AddChild( (new Frame( 25, 130 * p + 40, 400, 120 ))
			->AddChild( (new Picture(35, 35, 80, 80, info->avatar, false, true) ) )
			->AddChild( (new Label(115, 10, "Player Name:" )) ) ->AddChild( (new Label(210, 10, info->GetName() )) )
			->AddChild( (new Label(115, 35, "Scenario:" )) ) ->AddChild( (new Label(210, 35, info->scenario )) )
			->AddChild( (new Button(270, 70, 100, 25, "Play", StartGame, info )) )
			->AddChild( (new Button(15, 70, 100, 25, "Erase", ErasePlayer, info ) ) )
		);
		if( info == last ) {
			win->SetFormButton( (Button*) win->Search("/Frame/Button'Play'/") );
		}
	}

	if( names->size() == 0 ) {
		win->AddChild( (new Frame( 25, 130 * p + 40, 400, 120 ))
			->AddChild( (new Label(132, 42, "No saved games exist." )))
		);
	}

	// Add the 'Cancel' button
	win->AddChild( (new Button( 175, 130 * infoRows + 45, 100, 30, "Cancel", &UI::Close, win)) );

	UI::RegisterKeyboardFocus(win);
}

/** Starts a Game Scenario
 *  \details
 *  The PlayerInfo describes which Scenario to load and which Player to load.
 *  If the Player doesn't already exist, a default player is created by Scenario.
 */
void Menu::StartGame( void *info ) {
	PlayerList* playerList = PlayerList::Instance();

	playerToLoad = (PlayerInfo*)info;

	// Gather Player Information
	string simName = playerToLoad->scenario;
	string playerName = playerToLoad->GetName();

	assert(scenario == NULL);
	scenario = new Scenario();

	// Load the Scenario from file
	if( !scenario->Load( simName ) ) {
		LogMsg(ERR, "Failed to load the scenario '%s'.", simName.c_str());
		Dialogs::Alert("Failed to load scenario!");
		return;
	}

	if( !scenario->Initialize() ) {
		LogMsg(ERR, "Failed to initialize the scenario '%s'.", simName.c_str());
		Dialogs::Alert("Scenario loaded but failed to setup!");
		return;
	}

	// Create or Load the Player
	if( playerList->PlayerExists(playerName) ) {
		scenario->LoadPlayer( playerName );
	} else{
		scenario->CreateDefaultPlayer( playerName );
		//Lua::Call("intro");
	}

	if( !scenario->Setup() ) {
		LogMsg(ERR, "Failed to setup the scenario '%s'.", simName.c_str());
		Dialogs::Alert("Scenario loaded but failed to setup!");
		return;
	}

	// Close all Windows
	while( UI::Search("/Window/") ) {
		UI::Close( UI::Search("/Window/") );
	}

	UI::SwapScreens( "In Game" );

	// Run the scenario
	scenario->Run();

	UI::SwapScreens( "Main Screen" );

	// Close the scenario
	delete scenario; scenario = NULL;
}

/** Erase a Player
 * \note The user must confirm their choice first.
 */
void Menu::ErasePlayer( void* playerInfo ) {
	bool choice = Dialogs::Confirm("Are you sure you want erase this player?");

	if(choice) {
		string playerName = ((PlayerInfo*)playerInfo)->GetName();
		PlayerList* players = PlayerList::Instance();

		if(players->DeletePlayer(playerName)) {
			Dialogs::Alert("Player deleted successfully.");
		} else {
			Dialogs::Alert("A problem occurred while deleting the player.");
		}

	    UI::Close( UI::Search("/Window'Load Game'/") );
	}
}

/** Create a new Player
 *  \details
 *  This Validates that the player names is valid.
 *  - Player names cannot be duplicates of another Player.
 *  - Player names cannot include characters reserved by filesystems.
 *  \note This doesn't actually create the Payer, this creates a PlayerInfo and
 *  selects the Scenario.  StartGame is where the Player object is first created.
 */
void Menu::CreateNewPlayer() {
	PlayerList* players = PlayerList::Instance();

	string playerName = ((Textbox*)UI::Search("/Window'New Game'/Textbox'Player Name:'/"))->GetText();
	string simName = ((Dropdown*)UI::Search("/Window'New Game'/Dropdown/"))->GetText();

	if(players->PlayerExists(playerName)) {
		Dialogs::Alert("A player with that name exists.");
		return;
	}

	if(Filesystem::FilenameIsSafe(playerName) == false) {
		Dialogs::Alert("The following cannot be used: <>:\"/\\|?*");
		return;
	}

	playerToLoad = new PlayerInfo( playerName, simName );

	StartGame( playerToLoad );
}

/** Change a Picture to a different Image
 */
void Menu::ChangePicture( void* picture, void* image) {
	((Picture*)picture)->Set( (Image*)image );
}

/** Make a Picture change images when the mouse hovers over it.
 */
Picture* Menu::PictureButton( int x, int y, void (*callback)(), Image* activeImage, Image* inactiveImage) {
	Picture* pic = new Picture( x, y, inactiveImage );

	pic->RegisterAction( Action_MouseLUp, new VoidAction( callback ) );
	pic->RegisterAction( Action_MouseEnter, new MessageAction( ChangePicture, pic,   activeImage) );
	pic->RegisterAction( Action_MouseLeave, new MessageAction( ChangePicture, pic, inactiveImage) );

	UI::Add( pic );

	return pic;
}

/** Quit the Main Menu
 */
void Menu::QuitMenu() {
    quit = true;
}

