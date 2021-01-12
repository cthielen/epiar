/**\file			sectors.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Thursday, October 8, 2015
 * \date			Modified: Sunday, August 21, 2016
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
		bool HasPlanets() { return planets.size() > 0; }
		string GetName() { return name; }
		bool HasPlanet(string planetName);
		int GetTraffic() { return traffic; }
		list<string> GetNeighbors() const { return neighbors; }
		void AddNeighbor(string neighbor) { neighbors.push_back(neighbor); }

		float GetX() { return x; }
		float GetY() { return y; }

		Alliance* GetAlliance() const { return alliance; }

		friend ostream& operator<<(ostream & out, const Sector &s );

		void GenerateTraffic( int count );
		void GenerateDefaultTraffic() { GenerateTraffic( GetTraffic() ); }

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
		Sectors();
		Sector *GetSector( string& SectorName ) { return (Sector*)this->Get(SectorName); }
		Sector *GetSectorByPlanet( string& PlanetName );
		list<Sector*>* GetAllSectors();
		Component* newComponent() { return new Sector(); }
		void GetBoundaries(float *north, float *south, float *east, float *west);
		bool SectorHasNeighbor(Sector *origin, Sector *possibleNeighbor);

		bool Load(string filename, bool fileoptional = false, bool skipcorrupt = false);
};

#endif // __h_sectors__
