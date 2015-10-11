/**\file		menu.cpp
 * \author		Christopher Thielen (chris@epiar.net)
 * \author		and others.
 * \date		Created: Tuesday, April 26, 2011
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

bool Menu::quitSignal = false;

Scenario Menu::scenario;
PlayerInfo* Menu::playerToLoad = NULL;

Image* Menu::menuSplash = NULL;
Image* Menu::gameSplash = NULL;
Image* Menu::editSplash = NULL;

Picture *Menu::play = NULL;
Picture *Menu::load = NULL;
Picture *Menu::edit = NULL;
Picture *Menu::options = NULL;
Picture *Menu::exit = NULL;

Song *Menu::bgMusic = NULL;

//if(OPTION(int, "options/sound/buttons")) Sound::Get( "data/audio/Interface/28853__junggle__btn043.ogg" )->Play();

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
void Menu::Main_Menu( void )
{
	Input inputs;
	list<InputEvent> events;
    
	quitSignal = false;

	Players *players = Players::Instance();
	players->Load( "saves/saved-games.xml", true, true);

	if( OPTION(int,"options/scenario/automatic-load") )
	{
		if( AutoLoad() )
		{
			LogMsg(INFO,"AutoLoaded Game Complete. Quitting Epiar.");
			return;
		}
	}

	// Set up music
	bgMusic = Song::Get( "data/audio/Music/holst_mars.ogg" );
	if( bgMusic == NULL ) {
		LogMsg(WARN, "There was an error loading music from 'data/audio/Music/holst_mars.ogg'." );
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
			Video::PreDraw();
			UI::Draw();
			Video::Update();
		}

		if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, SDLK_ESCAPE ) ) ) {
			quitSignal = true;
		}

		// Wait until the next click
		Timer::Delay();
	} while(!quitSignal);
}

/** Load the most recent Player
 * \note When the user leaves the Scenario, the game will quit.
 * \returns true if the player was loaded successfully.
 */
bool Menu::AutoLoad()
{
	LogMsg(INFO,"Attempting to automatically load a player.");
	PlayerInfo* info = Players::Instance()->LastPlayer();

	if( info != NULL )
	{
		LogMsg(INFO, "Automatically loading player.", info->GetName().c_str() );

		if( !scenario.Load( info->scenario ) )
		{
			LogMsg(ERR,"Failed to load the scenario '%s'.", info->scenario.c_str() );
			return false;
		}

		if( !scenario.Setup() )
		{
			LogMsg(ERR,"Failed to setup the scenario '%s'.", info->scenario.c_str() );
			return false;
		}

		scenario.LoadPlayer( info->GetName() );
		scenario.Run();

		return true;
	}

	LogMsg(WARN,"No available players to load.");

	return false;
}

/** Create the Basic Main Menu
 *  \details The Splash Screen is random.
 */
void Menu::SetupUI()
{
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
	int screenNum = rand();
	menuSplash = Image::Get( splashScreens[(screenNum+0) % numScreens] );
	gameSplash = Image::Get( splashScreens[(screenNum+1) % numScreens] );
	editSplash = Image::Get( splashScreens[(screenNum+2) % numScreens] );

	// Add the splash screen
	UI::Add( new Picture( 0, 0, Video::GetWidth(), Video::GetHeight(), menuSplash, true ) );

	// Add the logo
	UI::Add( new Picture(20, Video::GetHeight() - 120, Image::Get("data/art/logo.png") ) );

	// New Button
	play = PictureButton( button_x, button_y + 50, Menu::CreateNewWindow,
	                      Image::Get( "data/graphics/txt_new_game_active.png"),
	                      Image::Get( "data/graphics/txt_new_game_inactive.png") );

	// Load Button
	if( (Players::Instance()->Size() > 0) )
	{
		load = PictureButton( button_x, button_y + 100, Menu::CreateLoadWindow,
		                       Image::Get( "data/graphics/txt_load_game_active.png"),
		                       Image::Get( "data/graphics/txt_load_game_inactive.png") );
	}

	// Editor Button
	edit = PictureButton( button_x, button_y + 150, Menu::CreateEditWindow,
	                       Image::Get( "data/graphics/txt_editor_active.png"),
	                       Image::Get( "data/graphics/txt_editor_inactive.png") );

	// Options Button
	options = PictureButton( button_x, button_y + 200, Dialogs::OptionsWindow,
	                          Image::Get( "data/graphics/txt_options_active.png"),
	                          Image::Get( "data/graphics/txt_options_inactive.png") );

	// Exit Button
	exit = PictureButton( button_x, button_y + 250, QuitMenu,
	                      Image::Get( "data/graphics/txt_exit_active.png"),
	                      Image::Get( "data/graphics/txt_exit_inactive.png") );

#ifdef EPIAR_UI_TESTS
	// Test that the GUI features work
	UI_Test();
#endif // EPIAR_UI_TESTS

}

/** This Window is used to create new Players.
 */
void Menu::CreateNewWindow()
{
	if( UI::Search("/Window'New Game'/") != NULL ) return;

	Window* win = new Window(250, 180, "New Game");
	UI::Add( win );

	// Player Name
	win->AddChild( (new Label(50, 40, "Name:")) )
		->AddChild( (new Textbox(90, 43, 130, 1, "", "Player Name:")) );

	// Scenario Picker
	win->AddChild( (new Label(25, 75, "Scenario:")) )
		->AddChild( (new Dropdown( 90, 75, 130, 25 ))
			->AddOptions( Filesystem::Enumerate("data/scenario/") )
	);

	win->AddChild( (new Button( 20, 130, 100, 30, "Cancel", &UI::Close, win)) );
	win->AddChild( (new Button(130, 130, 100, 30, "Start", &CreateNewPlayer )) );
}

/** This Window shows a list of potential Players.
 */
void Menu::CreateLoadWindow()
{
	if( UI::Search("/Window'Load Game'/") != NULL ) return;

	list<string> *names = Players::Instance()->GetNames();

	Window* win = new Window(450, 95 + (names->size() * 130), "Load Game");
	UI::Add( win );

	PlayerInfo* last = Players::Instance()->LastPlayer();

	// Create a new Frame for each Player
	int p = 0;
	list<string>::iterator iter;

	for( iter = names->begin(); iter != names->end(); ++iter, ++p ) {
		PlayerInfo *info = Players::Instance()->GetPlayerInfo( *iter );

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

	// Add the 'Cancel' button
	win->AddChild( (new Button( 175, 130 * names->size() + 45, 100, 30, "Cancel", &UI::Close, win)) );

	UI::RegisterKeyboardFocus(win);
}

/** Starts a Game Scenario
 *  \details
 *  The PlayerInfo describes which Scenario to load and which Player to load.
 *  If the Player doesn't already exist, a default player is created by Scenario.
 */
void Menu::StartGame( void *info )
{
	Players *players = Players::Instance();

	playerToLoad = (PlayerInfo*)info;

	// Gather Player Information
	string simName = playerToLoad->scenario;
	string playerName = playerToLoad->GetName();

	// Load the Scenario
	if( !scenario.Load( simName ) )
	{
		LogMsg(ERR, "Failed to load the scenario '%s'.", simName.c_str());
		Dialogs::Alert("Failed to load scenario!");
		return;
	}

	// Create or Load the Player
	if( players->PlayerExists(playerName) ) {
		scenario.LoadPlayer( playerName );
	} else{
		scenario.CreateDefaultPlayer( playerName );
		Lua::Call("intro");
	}

	if( !scenario.Setup() )
	{
		LogMsg(ERR, "Failed to setup the scenario '%s'.", simName.c_str());
		Dialogs::Alert("Scenario loaded but failed to setup!");
		return;
	}

	// Close all Windows
	while( UI::Search("/Window/") )
		UI::Close( UI::Search("/Window/") );

	UI::SwapScreens( "In Game", menuSplash, gameSplash );
	
	// Run the scenario
	scenario.Run();

	UI::SwapScreens( "Main Screen", gameSplash, menuSplash );
}

/** This Window will launch the editor.
 *  \details The User can either create a new scenario from scratch, or edit an existing scenario.
 */
void Menu::CreateEditWindow()
{
	// Return to Editor if it has alread been loaded
	if( scenario.isLoaded() ) {
		UI::SwapScreens( "Editor", menuSplash, editSplash );
		scenario.Edit();
		UI::SwapScreens( "Main Screen", editSplash, menuSplash );
		return;
	}

	// Don't create a Window if it already exists
	if( UI::Search("/Window'Editor'/") != NULL ) {
		return;
	}

	Window *editorWnd = NULL;
	UI::Add( editorWnd = (new Window(250, 300, "Editor"))
		->AddChild( (new Tabs( 10, 40, 230, 210, "EDIT TABS"))
			->AddChild( (new Tab( "Edit" ))
				->AddChild( (new Label(15, 15, "Pick the Scenario to Edit:")) )
				->AddChild( (new Dropdown( 45, 45, 100, 30 ))
					->AddOptions( Filesystem::Enumerate("data/scenario/") ) )
			)
			->AddChild( (new Tab( "Create" ))
				->AddChild( (new Label(15, 10, "Scenario Name:")) )
				->AddChild( (new Textbox(40, 40, 80, 1, "", "Scenario Name")) )
			)
		)
	);
	editorWnd->AddChild( new Button(10, 260, 100, 30, "Cancel", &UI::Close, editorWnd ) );
	editorWnd->AddChild( new Button(140, 260, 100, 30, "Edit", Menu::StartEditor ) );
	editorWnd->AddCloseButton();
}

/** Start an Editor Scenario
 */
void Menu::StartEditor()
{
	string simName = "default";
	assert( UI::Search("/Window'Editor'/Tabs/Tab/") != NULL );
	assert( false == scenario.isLoaded() );

	UI::Close( play ); // Play
	UI::Close( load ); // Load
	play = NULL;
	load = NULL;

	// Since the Random Universe Editor is currently broken, disable this feature here.
	//SETOPTION( "options/scenario/random-universe", 0 );

	Tab* activeTab = ((Tabs*)UI::Search("/Window'Editor'/Tabs/"))->GetActiveTab();
	if( activeTab->GetName() == "Edit" ) {
		simName = ((Dropdown*)activeTab->Search("/Dropdown/"))->GetText();
		if( !scenario.Load( simName ) )
		{
			LogMsg(ERR,"Failed to load '%s' successfully",simName.c_str());
			return;
		}
	} else { // Create
		simName = ((Textbox*)activeTab->Search("/Textbox'Scenario Name'/"))->GetText();

		scenario.New( simName );
	}

	if( !scenario.SetupToEdit() )
	{
		LogMsg(ERR,"Failed to setup the scenario '%s'.", simName.c_str());
		return;
	}

	// Only attempt to Edit if the Scenario has loaded
	assert( scenario.isLoaded() );

	// Close all Windows
	while( UI::Search("/Window/") )
		UI::Close( UI::Search("/Window/") );
	
	UI::SwapScreens( "Editor", menuSplash, editSplash );
	scenario.Edit();
	UI::SwapScreens( "Main Screen", editSplash, menuSplash );
}

/** Erase a Player
 * \note The user must confirm their choice first.
 */
void Menu::ErasePlayer( void* playerInfo )
{
	bool choice = Dialogs::Confirm("Are you sure you want erase this player?");

	if(choice) {
		string playerName = ((PlayerInfo*)playerInfo)->GetName();
		Players *players = Players::Instance();

		if(players->DeletePlayer(playerName))
			Dialogs::Alert("Player deleted successfully.");
		else
			Dialogs::Alert("A problem occurred while deleting the player.");

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
void Menu::CreateNewPlayer( )
{
	Players *players = Players::Instance();

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
void Menu::ChangePicture( void* picture, void* image)
{
	((Picture*)picture)->Set( (Image*)image );
}

/** Make a Picture change images when the mouse hovers over it.
 */
Picture* Menu::PictureButton( int x, int y, void (*callback)(), Image* activeImage, Image* inactiveImage)
{
	Picture* pic = new Picture( x, y, inactiveImage );
	pic->RegisterAction( Action_MouseLUp, new VoidAction( callback ) );
	pic->RegisterAction( Action_MouseEnter, new MessageAction( ChangePicture, pic,   activeImage) );
	pic->RegisterAction( Action_MouseLeave, new MessageAction( ChangePicture, pic, inactiveImage) );
	UI::Add( pic );
	return pic;
}

/** Quit the Main Menu
 */
void Menu::QuitMenu()
{
    quitSignal = true;
}

#ifdef EPIAR_UI_TESTS

void AddImage( void*widget, void*image, int x, int y )
{
	Container* container = ((Container*)widget);
	container->AddChild( new Picture( x, y, (Image*)image ) );
}

string LOREM =
	"Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor"
	" incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis "
	"nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat."
	" Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu"
	" fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in"
	" culpa qui officia deserunt mollit anim id est laborum.";


void ModalityTest() {
	Window* window = new Window( Video::GetWidth()/2-150, Video::GetHeight()/2-150, 300, 300, "Dialog" );
	window->AddChild( (new Paragraph(30, 30, 250, 30, "This is a Modal Dialog.  You should be unable to click elsewhere until you click the 'Release' button below." )) );
	window->AddChild( (new Button(100, 135, 100, 30, "Release", UI::ReleaseModality )) );
	UI::ModalDialog( window );
}
void TestConfirm()
{
	Dialogs::Confirm( LOREM );
}
void TestAlert()
{
	Dialogs::Alert( LOREM );
}

void UI_Test() {
	// Example of Nestable UI Creation
	UI::Add(
		(new Window( 20, 20, 600, 600, "A Window"))
		->AddChild( (new Tabs( 50, 50, 500, 500, "TEST TABS"))
			->AddChild( (new Tab( "Nested Frames" ))
				->AddChild( (new Button(10, 10, 100, 30, "Dummy 1",    NULL    )) )
				->AddChild( (new Frame( 50,50,400,400 ))
					->AddChild( (new Button(10, 10, 100, 30, "Dummy 2",    NULL    )) )
					->AddChild( (new Frame( 50,50,300,300 ))
						->AddChild( (new Button(10, 10, 100, 30, "Dummy 3",    NULL    )) )
						->AddChild( (new Frame( 50,50,200,200 ))
							->AddChild( (new Button(10, 10, 100, 30, "Dummy 4",    NULL    )) )
						)
					)
				)
			)
			->AddChild( (new Tab( "Scoll to Buttons" ))
				->AddChild( (new Label(10,   0, "Scroll Down")) )
				->AddChild( (new Frame( 150, 50, 200, 300 ))
					->AddChild( (new Label(10,   0, "Scroll Down")) )
					->AddChild( (new Button(10, 300, 100, 30, "Dummy",    NULL    )) )
					->AddChild( (new Label(10, 600, "Scroll Up")) )
				)
				->AddChild( (new Label(10, 600, "Scroll Up")) )
			)
			->AddChild( (new Tab("A Picture"))
				->AddChild( (new Picture(10, 0, 400, 400, "data/art/menu2.png")) )
			)
			->AddChild( (new Tab("Inputs"))
				->AddChild( (new Textbox(30, 30, 100, 1, "Some Text Goes Here", "A Textbox")) )
				->AddChild( (new Checkbox(30, 100, 0, "A Checkbox")) )
				->AddChild( (new Slider(30, 200, 200, 100, "A Slider", 0.4f )) )
				->AddChild( (new Button(10, 300, 100, 30, "Dummy", NULL )) )
				->AddChild( (new Dropdown(200, 200, 100, 30))
					->AddOption("Lorem")
					->AddOption("Ipsum")
					->AddOption("Dolar")
					->AddOption("Sit")
				)
				->AddChild( (new Dropdown(300, 200, 100, 20))
					->AddOption("One Fish")
					->AddOption("Two Fish")
					->AddOption("Red Fish")
					->AddOption("Blue Fish")
				)
				->AddChild( (new Paragraph(300, 250, 100, 20, LOREM)) )
				->AddChild( (new Textarea(10, 300, 250, 500, LOREM, "A Textarea")) )
			)
			->AddChild( (new Tab("Dialogs"))
				->AddChild( (new Button(10, 10, 100, 30, "Modality Test", ModalityTest )) )
				->AddChild( (new Button(10, 50, 100, 30, "Confirm Test", TestConfirm )) )
				->AddChild( (new Button(10, 90, 100, 30, "Alert Test", TestAlert )) )
			)
		)
	);

	Tab* clickTestTab = new Tab("Click Test");
	clickTestTab->RegisterAction( Action_MouseLUp, new PositionalAction( AddImage, clickTestTab, Image::Get("data/graphics/shuttle.png") ) );
	((Container*)UI::Search("/'A Window'/'TEST TABS'/"))->AddChild( clickTestTab );

	// Check that the UI Searching is working
	assert( NULL != UI::Search("/[0]/") );
	assert( NULL != UI::Search("/Window/") );
	assert( NULL != UI::Search("/Window/Tabs/") );
	assert( NULL != UI::Search("/(100,100)/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/Tab/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/[0]/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/Tab[1]/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/[0]/Frame/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/[0]/(60,60)/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/Tab/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/Tab/Frame/Button/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/Tab/Frame/Frame/Button/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/Tab/Frame/Frame/Frame/Button/") );

	// Check Odd but acceptable Queries
	// Slashes at the beginning and end are optional but expected
	assert( NULL != UI::Search("Window") );
	assert( NULL != UI::Search("/Window") );
	assert( NULL != UI::Search("Window/") );
	// Extra Slashes just grab the first child
	assert( NULL != UI::Search("/Window//") );
	assert( NULL != UI::Search("/Window//Tab/") );

	// Check that Bad Inputs find nothing
	assert( NULL == UI::Search("/[-1]/") );
	//assert( NULL == UI::Search("/[fdsa]/") ); // TODO: Should fail, but Asserts in string convert
	//assert( NULL == UI::Search("/(foo,bar)/") ); // TODO: Should fail, but Asserts in string convert
	assert( NULL == UI::Search("/WindowWindow/") );
	assert( NULL == UI::Search("/'foobar'/") );
	//assert( NULL == UI::Search("/[]/") ); // TODO: This should fail but doesn't. The empty string is converted to an Int
	//assert( NULL == UI::Search("/(,)/") ); // TODO: This should fail but doesn't. The empty string is converted to an Int

	// Check that Malformed Queries Fail and return nothing.
	// Malformed Indexes
	//assert( NULL == UI::Search("/[/") ); // TODO: Should fail, but Asserts in string convert
	//assert( NULL == UI::Search("/]/") ); // TODO: Should fail, but Asserts in string convert
	assert( NULL == UI::Search("/[1)/") );
	assert( NULL == UI::Search("/[100,100]/") );
	// Malformed Coordinates
	//assert( NULL == UI::Search("/(/") ); // TODO: Should fail, but Asserts in string convert
	//assert( NULL == UI::Search("/)/") ); // TODO: Should fail, but Asserts in string convert
	assert( NULL == UI::Search("/,/") ); 
	//assert( NULL == UI::Search("/(100 100)/") ); // TODO: Should fail, but Asserts in string convert
	assert( NULL == UI::Search("/(100,100]/") );
	// Malformed Strings
	assert( NULL == UI::Search("/Window'/") );
	assert( NULL == UI::Search("/Window\"/") );
	assert( NULL == UI::Search("/'Window/") );
	assert( NULL == UI::Search("/\"Window/") );

	// Set a test Form button
	Tab* inputTab = (Tab*)UI::Search("/'A Window'/'TEST TABS'/Tab'Inputs'/");
	assert( NULL != inputTab );
	assert( inputTab->GetMask() & WIDGET_TAB );
	inputTab->SetFormButton(
		(Button*) UI::Search("/'A Window'/'TEST TABS'/Tab'Some Inputs'/Button'Dummy'/")
	);
}

#endif // EPIAR_UI_TESTS
