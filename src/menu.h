/**\file		menu.h
 * \author		Christopher Thielen (chris@epiar.net)
 * \author		and others.
 * \date		Created: Tuesday, April 26, 2011
 * \date		Modified: Friday, January 1, 2016
 * \brief		Export Main_Menu function.
 * \details
 */

#ifndef __H_MENU
#define __H_MENU

#include "audio/music.h"
#include "engine/scenario.h"
#include "graphics/image.h"
#include "ui/ui.h"
#include "ui/ui_picture.h"

class Menu {
	public:
		static void Run( void );
		static Scenario* GetCurrentScenario( void );

	private:
		static bool quit;
		// The Scenario to be played
		static Scenario* scenario;
		static PlayerInfo* playerToLoad;

		// Images
		static Image* menuSplash;
		static Image* gameSplash;
		static Image* editSplash;

		// Menu Buttons
		// These pointers are only necessary because UI::Search does not support Picture paths.
		static Picture* play;
		static Picture* load;
		static Picture* edit;
		static Picture* options;
		static Picture* exit;
	
		// Music
		static Song* bgMusic;
	
		// Skip straight to the Game
		static bool AutoLoad( void );
	
		// GUI Setup and Actions
		static void SetupUI();
		static void CreateNewWindow();
		static void CreateLoadWindow();
		static void StartGame( void* playerInfo );
		static void QuitMenu();
	
		// Player Management
		static void CreateNewPlayer();
		static void ErasePlayer( void *playerInfo );

		// GUI Helpers
		static Picture* PictureButton( int x, int y, void (*callback)(), Image* activeImage, Image* inactiveImage);
		static void ChangePicture( void* picture, void* image);
};

#endif // __H_MENU
