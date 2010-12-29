/**\file			player.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Wednesday, July 5, 2006
 * \date			Modified: Saturday, January 5, 2008
 * \brief			Main player-specific functions and handle
 * \details
 */


#ifndef __H_PLAYER__
#define __H_PLAYER__

#include "includes.h"
#include "Sprites/ship.h"
#include "Engine/mission.h"

class Player : public Ship , public Component {
	public:
		static Player *Instance();

		static bool IsLoaded() { return pInstance!=NULL; }
		void setLastPlanet( string planetName);

		// Generic Getters
		string GetLastPlanet() { return lastPlanet; }
		string GetName() { return name; }

		// Autopilot Related Functions
		void SetLuaControlFunc( string _luaControlFunc );
		void RemoveLuaControlFunc();

		// Mission Related Functions
		void AcceptMission( Mission *mission );
		void RejectMission( string missionName );
		list<Mission*>* GetMissions() { return &missions; }

		// Saving and Loading this Player to XML
		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);

		friend class Players;

	protected:
		Player();
		Player( const Player & );
		Player& operator= (const Player&);
		~Player();

		void Update( void );

		virtual int GetDrawOrder( void ) {
			return( DRAW_ORDER_PLAYER );
		}
		Color GetRadarColor( void ) { return GOLD; }

		bool ConfigureWeaponSlots(xmlDocPtr, xmlNodePtr);

	private:
		static Player *pInstance;
		time_t lastLoadTime; // TODO This may need to be renamed
		string lastPlanet;
		list<Mission*> missions;
		string luaControlFunc;
};

class Players : public Components {
	public:
		static Players *Instance();
		Player* GetPlayer(string name) { return (Player*) this->Get(name); }
		Component* newComponent() { return new Player(); }

		Player* CreateNew(string playerName);
		Player* LoadLast();
		Player* LoadPlayer(string playerName);

		void SetDefaults(
			Model *_defaultModel,
			Engine *_defaultEngine,
			int _defaultCredits,
			Coordinate _defaultLocation);
		Model* GetDefaultModel() { return defaultModel; }
		Engine* GetDefaultEngine() { return defaultEngine; }
		int GetDefaultCredits() { return defaultCredits; }
		Coordinate GetDefaultLocation() { return defaultLocation; }

	protected:
		Players() {};
		Players( const Players & );
		Players& operator= (const Players&);

	private:
		static Players *pInstance;
		Model *defaultModel;
		Engine *defaultEngine;
		int defaultCredits;
		Coordinate defaultLocation;
};

#endif // __H_PLAYER__
