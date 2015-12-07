/**\file			navigation.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Sunday, October 18, 2015
 * \date			Modified: Sunday, October 18, 2015
 * \brief			Handles the navigation route
 * \details
 */

#include "includes.h"
#include "engine/navigation.h"
#include "engine/sectors.h"
#include "menu.h"

list<string> Navigation::Route;

/**\class Navigation
 * \brief Handles the navigation route. */

void Navigation::Init( void ) {
	Route.empty();
}

void Navigation::Close( void ) {
	Route.empty();
}

/* Adds a sector to the navigation route iff its logical, i.e. sectors connect */
bool Navigation::AddSector( string sectorName ) {
	Sectors *sectors = Menu::GetCurrentScenario()->GetSectors();
	Sector *lastAdded = NULL;
	Sector *attemptingToAdd = (Sector *)sectors->Get(sectorName);

	if(Menu::GetCurrentScenario()->GetCurrentSector()->GetName() == sectorName) {
		LogMsg(WARN, "Trying to add sector to navigation map but it is the current sector '%s'", sectorName.c_str());
		return false;
	}

	if(Navigation::Route.empty() == false) {
		lastAdded = (Sector *)sectors->Get(Navigation::Route.back());

		if(lastAdded == NULL) {
			LogMsg(WARN, "Trying to add sector to navigation map but cannot find last sector, Navigation::Route.back() returned NULL." );
			return false;
		}
		if(attemptingToAdd == NULL) {
			LogMsg(WARN, "Trying to add sector to navigation map but cannot find new sector '%s'", sectorName.c_str());
			return false;
		}

		if(std::find(Navigation::Route.begin(), Navigation::Route.end(), sectorName) != Navigation::Route.end()) {
			LogMsg(WARN, "Trying to add sector to navigation map but navigation route already contains sector '%s'", sectorName.c_str());
			return false;
		}

		if(sectors->SectorHasNeighbor(lastAdded, attemptingToAdd) == false) {
			LogMsg(WARN, "Trying to add sector to navigation map but new sector '%s' is not a neighbor of last sector '%s's'", lastAdded->GetName().c_str(), attemptingToAdd->GetName().c_str());
			return false;
		}
	}

	Route.push_back( sectorName );

	LogMsg(INFO, "Added '%s' to navigation map. Total route length now %d", sectorName.c_str(), Navigation::Route.size());

	return true;
}

bool Navigation::RemoveFinalSector( void ) {
	Route.pop_back();
	return true;
}

string Navigation::GetNextSector( void ) {
	return Route.front();
}

bool Navigation::RemoveNextSector( void ) {
	Route.pop_front();
	return true;
}

