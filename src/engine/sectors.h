/**\file			sectors.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Thursday, October 8, 2015
 * \date			Modified: Thursday, October 8, 2015
 * \brief
 * \details
 */

#ifndef __h_sectors__
#define __h_sectors__

#include "includes.h"
#include "utilities/coordinate.h"
#include "utilities/components.h"
#include "engine/alliances.h"

// Abstraction of a single sector
class Sector : public Component {
	public:
		Sector();
		Sector& operator=(const Sector& other);
		Sector( string _name,
				float _x,
				float _y,
				Alliance* _alliance,
				list<string> _planets,
				list<string> _neighbors,
				int _traffic
		);
		
		void Update( lua_State *L );

		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);
		
		~Sector();

		list<string> GetPlanets();
		bool HasPlanet(string planetName);
		int GetTraffic() { return traffic; }

		float GetX() { return x; }
		float GetY() { return y; }

		Alliance* GetAlliance() const { return alliance; }

	private:
		Alliance* alliance;
		list<string> planets;
		list<string> neighbors;
		int traffic;
		float x, y; // nav map coords, not sprite/world/screen coords
};

// Class that holds list of all sectors; manages them
class Sectors : public Components {
	public:
		static Sectors *Instance();
		Sector *GetSector( string& SectorName ) { return (Sector*)this->Get(SectorName); }
		Sector *GetSectorByPlanet( string& PlanetName );
		Component* newComponent() { return new Sector(); }
		
	protected:
		Sectors() {};
		Sectors( const Sectors & );
		Sectors& operator= (const Sectors&);

	private:
		static Sectors *pInstance;
};


#endif // __h_sectors__
