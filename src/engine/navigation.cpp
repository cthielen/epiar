/**\file			navigation.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Sunday, October 18, 2015
 * \date			Modified: Sunday, October 18, 2015
 * \brief			Handles the navigation route
 * \details
 */

#include "includes.h"
#include "engine/navigation.h"

list<string> Navigation::Route;

/**\class Navigation
 * \brief Handles the navigation route. */

void Navigation::Init( void ) {
	Route.empty();
}

void Navigation::Close( void ) {
	Route.empty();
}

bool Navigation::AddSector( string sectorName ) {
	Route.push_back( sectorName );
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

