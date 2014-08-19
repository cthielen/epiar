/**\file			sector.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Monday, August 18, 2014
 * \date			Modified: Monday, August 18, 2014
 * \brief
 * \details
 */

#ifndef __H_SECTOR__
#define __H_SECTOR__

#include "audio/sound.h"
#include "graphics/animation.h"
#include "utilities/components.h"
#include "engine/outfit.h"
#include "includes.h"

class Sector {

};

// Abstraction of a single engine
/*class Engine : public Outfit {
	public:
		Engine();
		Engine& operator= (const Engine&);
		Engine( string _name, Image* _pic, string _description, Sound* _sound, float _forceOutput, short int _msrp, bool _foldDrive, string _flareAnimation);

		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);

		string GetFlareAnimation( void ) { return flareAnimation; }
		short int GetFoldDrive( void ) { return foldDrive; }
		Sound* GetSound() { return thrustsound; }

	private:
		Sound *thrustsound;
		bool foldDrive;
		string flareAnimation;
};*/

// Class that holds list of all planets; manages them
/*class Engines : public Components {
	public:
		Engines();
		Engine* GetEngine(string name) { return (Engine*) this->Get(name); }
		Component* newComponent() { return new Engine(); }
};*/

#endif // __H_SECTOR__
