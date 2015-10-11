/**\file			sectors.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Thursday, October 8, 2015
 * \date			Modified: Thursday, October 8, 2015
 * \brief
 * \details
 */

#include "includes.h"
#include "engine/sectors.h"
#include "utilities/log.h"
#include "utilities/components.h"
#include "utilities/lua.h"
#include "engine/alliances.h"
#include "engine/scenario_lua.h"

/**\class Sector
 * \brief A Sector.
 *
 * Sectors are where planets live. Alliances own them.
 */

/**\brief Blank Constructor
 */
Sector::Sector() {
	Component::SetName("");
	alliance = NULL;
	traffic = 0;
}

/**\brief Copy Constructor
 */
Sector& Sector::operator=(const Sector& other) {
	// Check the other Sprite
	assert( other.GetAlliance() );

	name = other.name;
	alliance = other.alliance;
	traffic = other.traffic;
	neighbors = other.neighbors;
	planets = other.planets;

	// Set the Sprite Stuff
	Coordinate pos;
	x = other.x;
	y = other.y;

	return *this;
}

/**\brief Constructor using a full Full Description
 */
Sector::Sector( string _name,
				float _x,
				float _y,
				Alliance* _alliance,
				list<string> _planets,
				list<string> _neighbors,
				int _traffic
	):
	alliance(_alliance),
	planets(_planets),
	neighbors(_neighbors),
	traffic(_traffic)
{
	// Check the inputs
	assert(_alliance);

	Coordinate pos;
	pos.SetX(_x);
	pos.SetY(_y);

	SetName(_name);
}

/**\brief Destructor
 */
Sector::~Sector() {
}

/**\brief Parse one sector out of an xml node.
 */
bool Sector::FromXMLNode( xmlDocPtr doc, xmlNodePtr node ) {
	xmlNodePtr  attr;
	string value;
	Coordinate pos;

	if( (attr = FirstChildNamed(node, "alliance")) ){
		value = NodeToString(doc,attr);
		alliance = Alliances::Instance()->GetAlliance(value);
		if(alliance == NULL) {
			LogMsg(ERR, "Could not create Sector '%s'. Unknown Alliance '%s'.", this->GetName().c_str(), value.c_str());
			return false;
		}
	} else return false;

	if( (attr = FirstChildNamed(node, "x")) ){
		value = NodeToString(doc, attr);
		pos.SetX( atof( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node, "y")) ){
		value = NodeToString(doc, attr);
		pos.SetY( atof( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node, "traffic")) ){
		value = NodeToString(doc, attr);
		traffic = ( atoi( value.c_str() ) != 0);
	} else return false;

	for( attr = FirstChildNamed(node, "neighbor"); attr != NULL; attr = NextSiblingNamed(attr, "neighbor") ) {
		value = NodeToString(doc, attr);
		neighbors.push_back(value);
	}
	//neighbors.sort();
	neighbors.unique();

	for( attr = FirstChildNamed(node, "planet"); attr != NULL; attr = NextSiblingNamed(attr, "planet") ) {
		value = NodeToString(doc, attr);
		planets.push_back(value);
	}
	//planets.sort();
	planets.unique();

	return true;
}

/**\brief List of the Planets in this Sector
 */
list<string> Sector::GetPlanets() {
	list<string> planets;
	list<string>::iterator planetiter;

	for(planetiter = this->planets.begin(); planetiter != this->planets.end(); ++planetiter) {
		planets.push_back( *planetiter );
	}

	//planets.sort();
	planets.unique();

	return planets;
}

/* Returns true if sector has planet with name 'planetName', else false. */
bool Sector::HasPlanet(string planetName) {
	list<string> planets;
	list<string>::iterator iter;

	for(iter = this->planets.begin(); iter != this->planets.end(); ++iter) {
		if(*iter == planetName) return true;
	}

	return false;
}

/**\brief Save this Sector to an xml node
 */
xmlNodePtr Sector::ToXMLNode(string componentName) {
	char buff[256];
	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str() );

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST GetName().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "alliance", BAD_CAST GetAlliance()->GetName().c_str() );
	snprintf(buff, sizeof(buff), "%d", (int)GetX() );
	xmlNewChild(section, NULL, BAD_CAST "x", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", (int)GetY() );
	xmlNewChild(section, NULL, BAD_CAST "y", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", (int)GetTraffic() );
	xmlNewChild(section, NULL, BAD_CAST "traffic", BAD_CAST buff );

	//list<Technology*> techs = GetTechnologies();
	//for( list<Technology*>::iterator it = techs.begin(); it!=techs.end(); ++it ){
	//	xmlNewChild(section, NULL, BAD_CAST "technology", BAD_CAST (*it)->GetName().c_str() );
	//}

	return section;
}

/**\class Sectors
 * \brief Collection of all Sectors
 *
 *\see Sector
 */

Sectors *Sectors::pInstance = 0; // initialize pointer

/**\brief Returns or creates the Sectors instance.
 * \return Pointer to the Sectors instance
 */
Sectors *Sectors::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Sectors; // create the solid instance
		pInstance->rootName = "sectors";
		pInstance->componentName = "sector";
	}
	return( pInstance );
}

Sector *Sectors::GetSectorByPlanet( string& PlanetName ) {
	for( map<string,Component*>::iterator i = components.begin(); i != components.end(); ++i ) {
		Sector *s = (Sector *)i->second;
		assert(s != NULL);
		if(s->HasPlanet(PlanetName)) return s;
	}

	return NULL;
}

list<Sector*>* Sectors::GetAllSectors() {
	list<Sector *>* sectorList = new list<Sector *>();

	for( map<string,Component*>::iterator i = components.begin(); i != components.end(); ++i ) {
		Sector *s = (Sector *)i->second;
		assert(s != NULL);
		sectorList->push_back(s);
	}

	return sectorList;
}

/* Sets pointer values such that a box containing all sector coordinates is given */
void Sectors::GetBoundaries(float *north, float *south, float *east, float *west) {
	*north = *south = *east = *west = 0;

	for( map<string,Component*>::iterator i = components.begin(); i != components.end(); ++i ) {
		Sector *s = (Sector *)i->second;

		if(s->GetY() < *north) *north = s->GetY();
		if(s->GetY() > *south) *south = s->GetY();
		if(s->GetX() < *west) *west = s->GetX();
		if(s->GetX() > *east) *east = s->GetX();
	}
}

/** @} */

