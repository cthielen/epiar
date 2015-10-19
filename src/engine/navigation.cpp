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

/**\class Hud
 * \brief Heads-Up-Display. */

void Navigation::Init( void ) {

}

void Navigation::Close( void ) {

}

bool Navigation::AddSector( string sectorName ) {
	return false;
}

bool Navigation::RemoveFinalSector( void ) {
	return false;
}

string Navigation::GetNextSector( void ) {
	return NULL;
}

bool Navigation::RemoveNextSector( void ) {
	return false;
}

