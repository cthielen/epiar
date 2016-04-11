/**\file			navigation.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created  : Sunday, October 18, 2015
 * \date			Modified: Sunday, April 10, 2016
 * \brief			Handles the navigation route
 * \details
 */

#ifndef __h_navigation__
#define __h_navigation__

#include "engine/sectors.h"
#include "includes.h"

class Navigation {
	public:
		static void Init( void );
		static void Close( void );

		static bool AddSector( string sectorName );
		static bool RemoveFinalSector( void );

		static string GetNextSector( void );
		static bool RemoveNextSector( void );

		static bool IsEnroute(Sector *a, Sector *b);

		static void ClearRoute( void );

		static bool HasDestination( void );

	private:
		static list<string> Route;

		static void PrintRoute();
};

#endif // __h_navigation__
