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
#include "utilities/components.h"

// Abstraction of the map
class Map : public Components {
	public:
		Map();

		Map& operator= (const Map& other);
		Map operator+ (const Map& other);
		Map& operator+= (const Map& other);

		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);
};

#endif // __h_map__
