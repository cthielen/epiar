/**\file			scenario.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Sunday, August 17, 2014
 * \date			Modified: Sunday, August 17, 2014
 * \brief			
 * \details
 */

#include "includes.h"
#include "common.h"
#include "audio/music.h"
#include "audio/audio_lua.h"
#include "engine/hud.h"
#include "engine/scenario.h"
#include "engine/commodities.h"
#include "engine/alliances.h"
#include "engine/technologies.h"
#include "engine/starfield.h"
#include "engine/console.h"
#include "graphics/video.h"
#include "sprites/effects.h"
#include "sprites/player.h"
#include "sprites/planets.h"
#include "sprites/planets_lua.h"
#include "utilities/file.h"
#include "utilities/log.h"
#include "utilities/lua.h"

/**\class Scenario
 * \brief */

/**\brief Loads an empty Scenario.
 */
Scenario::Scenario( void ) {
	commodities = new Commodities();
	engines = new Engines();
	map = new Map();
	models = new Models(this);
	weapons = new Weapons();
	alliances = new Alliances();
	technologies = new Technologies();
	outfits = new Outfits();

	folderpath = "";
}

/**\brief Loads the XML file.
 * \param filename Name of the file
 * \return true if success
 */
bool Scenario::Load( string scenarioName ) {
	folderpath = "resources/scenarios/" + scenarioName + "/";

	if( !Open( folderpath + string("scenario.xml") ) ) {
		return false;
	}

	loaded = Parse();

	return loaded;
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
	if( map->Load( (folderpath + Get("map")) ) != true ) {
	    LogMsg(WARN, "There was an error loading the map from '%s'.", (folderpath + Get("map")).c_str() );
	    return false;
    }

	// Check the Music
	bgmusic = Song::Get( Get("music") );
	if( bgmusic == NULL ) {
		LogMsg(WARN, "There was an error loading music from '%s'.", Get("music").c_str() );
	}

	// Check the Player Defaults
	if( planets->Get( Get("defaultPlayer/start")) == NULL) {
		LogMsg(ERR, "Bad Default Player Start Location '%s'.", Get("defaultPlayer/start").c_str() );
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

	return true;
}

