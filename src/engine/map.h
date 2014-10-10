/**\file			map.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Sunday, August 17, 2014
 * \date			Modified: Sunday, August 17, 2014
 * \brief
 * \details
 */

#ifndef __h_map__
#define __h_map__

#include "includes.h"
#include "sprites/sprite.h"
#include "utilities/coordinate.h"
#include "utilities/components.h"
#include "engine/models.h"
#include "engine/engines.h"
#include "engine/weapons.h"
#include "engine/technologies.h"
#include "engine/alliances.h"

// Abstraction of the map
class Map : public Component {
	public:
		Map();
		Map& operator=(const Map& other);
		Map( string _name,
				float _x,
				float _y,
				Image* _image,
				Alliance* _alliance,
				bool _landable,
				int _traffic,
				int _militiaSize,
				int _sphereOfInfluence,
				Image* _surface,
				string _summary,
				list<Technology*> _technologies
		);
		
		void Update( lua_State *L );
		void GenerateTraffic( lua_State *L );

		virtual int GetDrawOrder( void ) { return( DRAW_ORDER_PLANET ); }
		
		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);
		
		~Map();

		list<Model*> GetModels();
		list<Engine*> GetEngines();
		list<Weapon*> GetWeapons();
		list<Outfit*> GetOutfits();
		Alliance* GetAlliance() const {return alliance;}
		short unsigned int GetTraffic() const {return traffic;}
		short unsigned int GetMilitiaSize() const {return militiaSize;}
		bool GetLandable() const {return landable;}
		string GetSummary() const {return summary;}
		int GetInfluence() const {return sphereOfInfluence;}
		Image* GetSurfaceImage() const {return surface;}
		list<Technology*> GetTechnologies() const { return technologies;}

		bool GetForbidden() {return forbidden;}
		void SetForbidden(bool f) {forbidden = f;}
		void SetInfluence(int influence) {sphereOfInfluence = influence;}

	private:
		Alliance* alliance;
		bool landable;
		bool forbidden;
		short unsigned int traffic;
		short unsigned int militiaSize;
		int sphereOfInfluence;
		Image* surface;
		string summary;
		list<Technology*> technologies;

		Uint32 lastTrafficTime;
};

// Class that holds list of all planets; manages them
/*
class Planets : public Components {
	public:
		static Planets *Instance();
		Planet *GetPlanet( string& PlanetName ) { return (Planet*) this->Get(PlanetName); }
		Component* newComponent() { return new Planet(); }
		
	protected:
		Planets() {};
		Planets( const Planets & );
		Planets& operator= (const Planets&);

	private:
		static Planets *pInstance;
};*/

#endif // __h_map__
