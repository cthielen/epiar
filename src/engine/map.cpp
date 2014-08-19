/**\file			map.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Sunday, August 17, 2014
 * \date			Modified: Sunday, August 17, 2014
 * \brief
 * \details
 */
#include "includes.h"
#include "engine/map.h"
#include "graphics/image.h"
#include "utilities/log.h"
#include "utilities/components.h"

/** \class Map
 * \brief 
 */

/** Default Constructor
 */
Map::Map() {
}

bool Map::Load(string filepath) {

	return false;
}

Sector *GetSector(string sectorName) {
	return NULL;
}

/** Default Copy Constructor
 */
Map& Map::operator= (const Map& other)
{
	/*msrp = other.msrp;
	picture = other.picture;
	description = other.description;
	rotPerSecond = other.rotPerSecond;
	maxSpeed = other.maxSpeed;
	forceOutput = other.forceOutput;
	mass = other.mass;
	cargoSpace = other.cargoSpace;
	surfaceArea = other.surfaceArea;
	hullStrength = other.hullStrength;
	shieldStrength = other.shieldStrength;*/
	return *this;
}

/**	Add different Maps together to combine their effectiveness
 */
Map Map::operator+ (const Map& other)
{
	Map total;
	/*total.msrp = msrp + other.msrp;
	total.picture = picture; // Keep the first picture
	total.rotPerSecond = rotPerSecond + other.rotPerSecond;
	total.maxSpeed = maxSpeed + other.maxSpeed;
	total.forceOutput = forceOutput + other.forceOutput;
	total.mass = mass + other.mass;
	total.cargoSpace = cargoSpace + other.cargoSpace;
	total.surfaceArea = surfaceArea + other.surfaceArea;
	total.hullStrength = hullStrength + other.hullStrength;
	total.shieldStrength = shieldStrength + other.shieldStrength;*/
	return total;
}

/**	Add different Maps together to combine their effectiveness
 */
Map& Map::operator+= (const Map& other)
{
	//msrp += other.msrp;
	// Don't change the picture
	/*rotPerSecond += other.rotPerSecond;
	maxSpeed += other.maxSpeed;
	forceOutput += other.forceOutput;
	mass += other.mass;
	cargoSpace += other.cargoSpace;
	surfaceArea += other.surfaceArea;
	hullStrength += other.hullStrength;
	shieldStrength += other.shieldStrength;*/
	return *this;
}

/**\brief Parses weapon information
 */
bool Map::FromXMLNode( xmlDocPtr doc, xmlNodePtr node ) {
	xmlNodePtr  attr;
	string value;

	/*if( (attr = FirstChildNamed(node,"msrp")) ){
		value = NodeToString(doc,attr);
		SetMSRP( atoi( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node,"picName")) ){
		Image* pic = Image::Get( NodeToString(doc,attr) );
		// This image can be accessed by either the path or the Engine Name
		Image::Store(name, pic);
		SetPicture(pic);
	} else return false;

	if( (attr = FirstChildNamed(node,"description")) ){
		value = NodeToString(doc,attr);
		SetDescription( value );
	} else {
		LogMsg( WARN, "%s does not have a description.", GetName().c_str() );
	}

	if( (attr = FirstChildNamed(node,"rotsPerSecond")) ){
		value = NodeToString(doc,attr);
		SetRotationsPerSecond( static_cast<float>(atof( value.c_str() )));
	}

	if( (attr = FirstChildNamed(node,"maxSpeed")) ){
		value = NodeToString(doc,attr);
		SetMaxSpeed( static_cast<float>(atof( value.c_str() )));
	}

	if( (attr = FirstChildNamed(node,"force")) ){
		value = NodeToString(doc,attr);
		SetForceOutput( static_cast<float> (atof( value.c_str() )));
	}

	if( (attr = FirstChildNamed(node,"mass")) ){
		value = NodeToString(doc,attr);
		SetMass( static_cast<float> (atof( value.c_str() )));
	}

	if( (attr = FirstChildNamed(node,"surfaceArea")) ){
		value = NodeToString(doc,attr);
		SetSurfaceArea( atoi( value.c_str() ));
	}

	if( (attr = FirstChildNamed(node,"cargoSpace")) ){
		value = NodeToString(doc,attr);
		SetCargoSpace( atoi( value.c_str() ));
	}

	if( (attr = FirstChildNamed(node,"hull")) ){
		value = NodeToString(doc,attr);
		SetHullStrength( atoi( value.c_str() ));
	}

	if( (attr = FirstChildNamed(node,"shield")) ){
		value = NodeToString(doc,attr);
		SetShieldStrength( (short)atoi( value.c_str() ));
	}*/

	return true;
}

/** \brief Converts the Map object to an XML node.
 */
xmlNodePtr Map::ToXMLNode(string componentName) {
	char buff[256];
	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str() );

	/*xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );

	snprintf(buff, sizeof(buff), "%d", this->GetMSRP() );
	xmlNewChild(section, NULL, BAD_CAST "msrp", BAD_CAST buff );

	xmlNewChild(section, NULL, BAD_CAST "picName", BAD_CAST this->GetPicture()->GetPath().c_str() );

	xmlNewChild(section, NULL, BAD_CAST "description", BAD_CAST this->GetDescription().c_str() );

	snprintf(buff, sizeof(buff), "%f", this->GetRotationsPerSecond() );
	xmlNewChild(section, NULL, BAD_CAST "rotsPerSecond", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%f", this->GetMaxSpeed() );
	xmlNewChild(section, NULL, BAD_CAST "maxSpeed", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%f", this->GetForceOutput() );
	xmlNewChild(section, NULL, BAD_CAST "force", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%f", this->GetMass() );
	xmlNewChild(section, NULL, BAD_CAST "mass", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%d", this->GetSurfaceArea() );
	xmlNewChild(section, NULL, BAD_CAST "surfaceArea", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%d", this->GetCargoSpace() );
	xmlNewChild(section, NULL, BAD_CAST "cargoSpace", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%d", this->GetHullStrength() );
	xmlNewChild(section, NULL, BAD_CAST "hull", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%d", this->GetShieldStrength() );
	xmlNewChild(section, NULL, BAD_CAST "shield", BAD_CAST buff );
*/
	return section;
}

