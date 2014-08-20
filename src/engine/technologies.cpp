/**\file			technologies.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Saturday, February 13, 2010
 * \date			Modified: Wednesday, August 20, 2014
 * \brief
 * \details
 */

#include "includes.h"
#include "engine/scenario.h"
#include "engine/technologies.h"
#include "engine/models.h"
#include "engine/engines.h"
#include "engine/weapons.h"
#include "utilities/components.h"

/**\class Technology
 * \brief Technology groups
 */

/**\brief Empty constructor
 */
Technology::Technology(){}

/**\brief Assignment operator (copy fields)
 */
Technology& Technology::operator= (const Technology& other)
{
	name = other.name;
	models = other.models;
	engines = other.engines;
	weapons = other.weapons;
	outfits = other.outfits;
	return *this;
}

/**\brief Creates a new Technology given parameters.
 * \param _name Name of the Technology
 * \param _models std::list of Model objects
 * \param _engines std::list of Engine objects
 * \param _weapons std::list of Weapon objects
 * \param _outfits std::list of outfit objects
 */
Technology::Technology( string _name,
		list<Model*> _models,
		list<Engine*>_engines,
		list<Weapon*>_weapons,
		list<Outfit*>_outfits) :
	models (_models ),
	engines(_engines),
	weapons(_weapons),
	outfits(_outfits)
{
	SetName(_name);
}

/**\brief Parses the XML file for Technology fields
 */
bool Technology::FromXMLNode( void *scenario, xmlDocPtr doc, xmlNodePtr node ) {
	xmlNodePtr  tech;
	string value;

	for( tech=node->xmlChildrenNode; tech!=NULL; tech = tech->next )
	{
		if( NodeNameIs( tech, "model" )) {
			value = NodeToString(doc,tech);
			Model* model = ((Scenario *)scenario)->GetModels()->GetModel( value );
			if(model==NULL) {
				LogMsg(ERR, "Could Not find the technology '%s'.", value.c_str() );
			} else {
				models.push_back( model );
			}
		} else if( NodeNameIs( tech, "engine" )) {
			value = NodeToString(doc,tech);
			Engine* engine = ((Scenario *)scenario)->GetEngines()->GetEngine( value );
			if(engine==NULL) {
				LogMsg(ERR, "Could Not find the technology '%s'.", value.c_str() );
			} else {
				engines.push_back( engine );
			}
		} else if( NodeNameIs( tech, "weapon" )) {
			value = NodeToString(doc,tech);
			Weapon* weapon = ((Scenario *)scenario)->GetWeapons()->GetWeapon( value );
			if(weapon==NULL) {
				LogMsg(ERR, "Could Not find the technology '%s'.", value.c_str() );
			} else {
				weapons.push_back( weapon );
			}
		} else if( NodeNameIs( tech, "outfit" )) {
			value = NodeToString(doc,tech);
			Outfit* outfit = ((Scenario *)scenario)->GetOutfits()->GetOutfit( value );
			if(outfit==NULL) {
				LogMsg(ERR, "Could Not find the technology '%s'.", value.c_str() );
			} else {
				outfits.push_back( outfit );
			}
		} else {

		}
	}
	return true;
}

/**\brief Converts the Technology object to an XML node
 */
xmlNodePtr Technology::ToXMLNode(string componentName) {
		xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str() );

		xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );
		
		list<Model*> models = this->GetModels();
		for( list<Model*>::iterator it_m = models.begin(); it_m!=models.end(); ++it_m ){
			xmlNewChild(section, NULL, BAD_CAST "model", BAD_CAST (*it_m)->GetName().c_str() );
		}

		list<Weapon*> weapons = this->GetWeapons();
		for( list<Weapon*>::iterator it_w = weapons.begin(); it_w!=weapons.end(); ++it_w ){
			xmlNewChild(section, NULL, BAD_CAST "weapon", BAD_CAST (*it_w)->GetName().c_str() );
		}

		list<Engine*> engines = this->GetEngines();
		for( list<Engine*>::iterator it_e = engines.begin(); it_e!=engines.end(); ++it_e ){
			xmlNewChild(section, NULL, BAD_CAST "engine", BAD_CAST (*it_e)->GetName().c_str() );
		}

		list<Outfit*> outfits = this->GetOutfits();
		for( list<Outfit*>::iterator it_w = outfits.begin(); it_w!=outfits.end(); ++it_w ){
			xmlNewChild(section, NULL, BAD_CAST "outfit", BAD_CAST (*it_w)->GetName().c_str() );
		}

	return section;
}

/**\fn Technology::GetModels()
 *  \brief Returns the list of Model objects
 * \fn Technology::GetEngines()
 *  \brief Returns the list of Engine objects
 * \fn Technology::GetWeapons()
 *  \brief Returns the list of Weapon objects
 * \fn Technology::GetOutfits()
 *  \brief Returns the list of Weapon objects
 */

/**\class Technologies
 * \brief Collection of Technology objects
 */

/**\fn Technologies::GetTechnology( string& TechnologyName )
 *  \brief Retrieves a Technology by name.
 * \fn Technologies::newComponent()
 *  \brief Creates a new Technology object
 * \fn Technologies::Technologies( const Technologies& )
 *  \brief Empty constructor.
 * \fn Technologies::operator = (const Technologies&)
 *  \brief Empty assignment operator.
 */
