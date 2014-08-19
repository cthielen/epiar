/**\filename		scenario.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Sunday, August 17, 2014
 * \date			Modified: Sunday, August 17, 2014
 * \brief			
 * \details
 */

#ifndef __H_SCENARIO__
#define __H_SCENARIO__

#include "audio/music.h"
#include "engine/commodities.h"
#include "engine/alliances.h"
#include "engine/engines.h"
#include "engine/models.h"
#include "engine/calendar.h"
#include "engine/map.h"
#include "engine/weapons.h"
#include "engine/technologies.h"
#include "sprites/player.h"
#include "audio/music.h"
#include "engine/camera.h"
#include "input/input.h"
#include "engine/console.h"

class Scenario {
	public:
		Scenario();

		bool New( string _folderpath );
		bool Load( string _folderpath );
		bool Parse(void);

		Song *GetBgMusic(void) { return bgmusic; }

		Commodities *GetCommodities() { return commodities; }
		Engines *GetEngines() { return engines; }
		Models *GetModels() { return models; }
		Weapons *GetWeapons() { return weapons; }
		Alliances *GetAlliances() { return alliances; }
		Technologies *GetTechnologies() { return technologies; }
		Outfits *GetOutfits() { return outfits; }
		Map *GetMap() { return map; }

	private:
		Commodities *commodities;
		Engines *engines;
		Models *models;
		Weapons *weapons;
		Map *map;
		Alliances *alliances;
		Technologies *technologies;
		Outfits *outfits;

		Song* bgmusic;

		XMLFile *scenarioXML;

		// Description of this Simulation
		string folderpath;
};

#endif // __H_SCENARIO__
