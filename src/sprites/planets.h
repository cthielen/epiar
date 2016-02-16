/**\file			planets.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Thursday, October 8, 2015
 * \brief
 * \details
 */

#ifndef __h_planets__
#define __h_planets__

#include "includes.h"
#include "sprites/sprite.h"
#include "utilities/coordinate.h"
#include "utilities/components.h"
#include "engine/models.h"
#include "engine/engines.h"
#include "engine/weapons.h"
#include "engine/technologies.h"
#include "engine/alliances.h"

// Abstraction of a single planet
class Planet : public Sprite, public Component {
	public:
		Planet();
		Planet& operator=(const Planet& other);
		Planet( string _name,
				float _x,
				float _y,
				Image* _image,
				Alliance* _alliance,
				bool _landable,
				Image* _surface,
				string _summary,
				list<Technology*> _technologies
		);
		
		void Update( lua_State *L );
		void GenerateTraffic( lua_State *L );

		virtual int GetDrawOrder( void ) { return( DRAW_ORDER_PLANET ); }
		
		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);
		
		~Planet();

		list<Model*> GetModels();
		list<Engine*> GetEngines();
		list<Weapon*> GetWeapons();
		list<Outfit*> GetOutfits();

		Alliance* GetAlliance() const { return alliance; }
		bool GetLandable() const { return landable; }
		string GetSummary() const { return summary; }
		Image* GetSurfaceImage() const { return surface; }
		list<Technology*> GetTechnologies() const { return technologies; }

		bool GetForbidden() { return forbidden; }
		void SetForbidden(bool f) { forbidden = f; }

	private:
		Alliance* alliance;
		bool landable;
		bool forbidden;
		Image* surface;
		string summary;
		list<Technology*> technologies;

		Uint32 lastTrafficTime;
};

// Class that holds list of all planets; manages them
class Planets : public Components {
	public:
		static Planets *Instance();
		Planet *GetPlanet( string& PlanetName ) { return (Planet*) this->Get(PlanetName); }
		Planet *GetPlanetByID( int spriteID );
		Component* newComponent() { return new Planet(); }
		
	protected:
		Planets() {};
		Planets( const Planets & );
		Planets& operator= (const Planets&);

	private:
		static Planets *pInstance;
};


#endif // __h_planets__
