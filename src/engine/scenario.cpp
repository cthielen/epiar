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
#include "engine/sector.h"
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
#include "utilities/xmlfile.h"

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
	scenarioXML = new XMLFile();
	bgmusic = NULL;

	folderpath = "";
}

/**\brief Loads the XML file.
 * \param filename Name of the file
 * \return true if success
 */
bool Scenario::Load( string scenarioName ) {
	folderpath = "resources/scenarios/" + scenarioName + "/";

	if( !scenarioXML->Open( folderpath + string("scenario.xml") ) ) {
		return false;
	}

	return Parse();
}

/**\brief Parses an XML scenario file
 * \return true if successful
 */
bool Scenario::Parse( void ) {
	LogMsg(INFO, "Scenario version %s.%s.%s.", scenarioXML->Get("version-major").c_str(), scenarioXML->Get("version-minor").c_str(), scenarioXML->Get("version-macro").c_str());

	// Now load the various subsystems
	if( commodities->Load( (folderpath + scenarioXML->Get("commodities")) ) != true ) {
		LogMsg(ERR, "There was an error loading the commodities from '%s'.", (folderpath + scenarioXML->Get("commodities")).c_str() );
		return false;
	}
	if( engines->Load( (folderpath + scenarioXML->Get("engines")) ) != true ) {
		LogMsg(ERR, "There was an error loading the engines from '%s'.", (folderpath + scenarioXML->Get("engines")).c_str() );
		return false;
	}
	if( weapons->Load( (folderpath + scenarioXML->Get("weapons")) ) != true ) {
		LogMsg(ERR, "There was an error loading the weapons from '%s'.", (folderpath + scenarioXML->Get("weapons")).c_str() );
		return false;
	}
	if( models->Load( (folderpath + scenarioXML->Get("models")) ) != true ) {
		LogMsg(ERR, "There was an error loading the models from '%s'.", (folderpath + scenarioXML->Get("models")).c_str() );
		return false;
	}
	if( outfits->Load( (folderpath + scenarioXML->Get("outfits")) ) != true ) {
		LogMsg(ERR, "There was an error loading the outfits from '%s'.", (folderpath + scenarioXML->Get("outfits")).c_str() );
		return false;
	}
	if( technologies->Load( (folderpath + scenarioXML->Get("technologies")) ) != true ) {
		LogMsg(ERR, "There was an error loading the technologies from '%s'.", (folderpath + scenarioXML->Get("technologies")).c_str() );
		return false;
	}
	if( alliances->Load( (folderpath + scenarioXML->Get("alliances")) ) != true ) {
		LogMsg(ERR, "There was an error loading the alliances from '%s'.", (folderpath + scenarioXML->Get("alliances")).c_str() );
		return false;
	}
	if( map->Load( (folderpath + scenarioXML->Get("map")) ) != true ) {
	    LogMsg(WARN, "There was an error loading the map from '%s'.", (folderpath + scenarioXML->Get("map")).c_str() );
	    return false;
    }

	// Check the Music
	bgmusic = Song::Get( scenarioXML->Get("music") );
	if( bgmusic == NULL ) {
		LogMsg(WARN, "There was an error loading music from '%s'.", scenarioXML->Get("music").c_str() );
	}

	// Check the Player Defaults
	if( map->GetSector( scenarioXML->Get("defaultPlayer/sector")) == NULL) {
		LogMsg(ERR, "Bad Default Player Start Location '%s'.", scenarioXML->Get("defaultPlayer/start").c_str() );
		return false;
	}
	if( models->Get( scenarioXML->Get("defaultPlayer/model")) == NULL) {
		LogMsg(ERR, "Bad Default Player Start Model '%s'.", scenarioXML->Get("defaultPlayer/model").c_str() );
		return false;
	}
	if( engines->Get( scenarioXML->Get("defaultPlayer/engine")) == NULL) {
		LogMsg(WARN, "Bad Default Player Start Engine '%s'.", scenarioXML->Get("defaultPlayer/engine").c_str() );
		return false;
	}

	return true;
}

