/**\file			navigation.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created  : Sunday, October 18, 2015
 * \date			Modified: Sunday, October 18, 2015
 * \brief			Handles the navigation route
 * \details
 */

#ifndef __h_navigation__
#define __h_navigation__

#include "includes.h"

class Navigation {
	public:
		static void Init( void );
		static void Close( void );

		static bool AddSector( string sectorName );
		static bool RemoveFinalSector( void );

		static string GetNextSector( void );
		static bool RemoveNextSector( void );

	private:
		static list<string> Route;
};

#endif // __h_navigation__
