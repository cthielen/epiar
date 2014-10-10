/**\file			technologies.h
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Saturday, February 13, 2010
 * \date			Modified: Saturday, February 13, 2010
 * \brief
 * \details
 */

#ifndef __h_technologies
#define __h_technologies

#include "includes.h"
#include "engine/models.h"
#include "engine/engines.h"
#include "engine/weapons.h"
#include "utilities/components.h"

class Technology : public Component {
	public:
		Technology();
  		Technology& operator= (const Technology&);
		Technology( string _name, list<Model*> _models, list<Engine*>_engines, list<Weapon*>_weapons, list<Outfit*>_outfits);
		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);

		list<Model*> GetModels() { return models; }
		list<Engine*> GetEngines() { return engines; }
		list<Weapon*> GetWeapons() { return weapons; }
		list<Outfit*> GetOutfits() { return outfits; }

	private:
		list<Model*> models;
		list<Engine*> engines;
		list<Weapon*> weapons;
		list<Outfit*> outfits;
};

// Class that holds list of all technologies; manages them
class Technologies : public Components {
	public:
		Technologies() {};
		Technology *GetTechnology( string& TechnologyName ) { return (Technology*) this->Get(TechnologyName); }
		Component* newComponent(){ return new Technology; }
};

#endif // __h_technologies
