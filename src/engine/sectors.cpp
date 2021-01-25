/**\file			sectors.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Thursday, October 8, 2015
 * \date			Modified: Sunday, August 21, 2016
 * \brief
 * \details
 */

#include "menu.h"
#include "includes.h"
#include "engine/sectors.h"
#include "utilities/log.h"
#include "utilities/components.h"
#include "utilities/lua.h"
#include "engine/alliances.h"
#include "engine/scenario_lua.h"
#include "sprites/npc.h"

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
	xmlNodePtr attr;
	string value;
	Coordinate pos;

	if( (attr = FirstChildNamed(node, "alliance")) ){
		value = NodeToString(doc,attr);
		alliance = Menu::GetCurrentScenario()->GetAlliances()->GetAlliance(value);
		if(alliance == NULL) {
			LogMsg(ERR, "Could not create Sector '%s'. Unknown Alliance '%s'.", this->GetName().c_str(), value.c_str());
			return false;
		}
	} else return false;

	if( (attr = FirstChildNamed(node, "x")) ){
		value = NodeToString(doc, attr);
		x = atof( value.c_str() );
	} else return false;

	if( (attr = FirstChildNamed(node, "y")) ){
		value = NodeToString(doc, attr);
		y = atof( value.c_str() );
	} else return false;

	if( (attr = FirstChildNamed(node, "traffic")) ){
		value = NodeToString(doc, attr);
		traffic = atoi( value.c_str() );
	} else return false;

	for( attr = FirstChildNamed(node, "neighbor"); attr != NULL; attr = NextSiblingNamed(attr, "neighbor") ) {
		value = NodeToString(doc, attr);
		neighbors.push_back(value);
	}
	neighbors.unique();

	for( attr = FirstChildNamed(node, "planet"); attr != NULL; attr = NextSiblingNamed(attr, "planet") ) {
		value = NodeToString(doc, attr);
		planets.push_back(value);
	}
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

/* Creates 'count' AI ships in spriteManager based on sector characteristics */
void Sector::GenerateTraffic( int count ) {
	Scenario *currentScenario = Menu::GetCurrentScenario();

	if(currentScenario == NULL) {
		LogMsg(ERR, "Could not get current scenario while generating traffic.");
		return;
	}

	//cout << "Generating traffic, count of " << count << endl;

	//local X = X + about(Range)
	//local Y = Y + about(Range)
	//local model = choose(models)
	//local engine = choose(engines)
	//if (PLAYER:GetCredits() > 10000) then
	//	statemachines = {"Hunter", "Trader", "Patrol", "Bully" }
	//else
	//	statemachines = {"Trader", "Patrol", "Bully" }
	//end
	//local pirateModels = { "Fleet Guard", "Kartanal", "Terran Assist", "Patitu", "Terran Corvert Mark I", "Large Vesper", "Raven", "Hammer Freighter"  }
	//local escortModels = { "Fleet Guard", "Terran XV", "Kartanal", "Patitu", "Terran Corvert Mark I"  }

	//local p = choose(plans)

	// Turn some Hunters into anti-player Pirates if the player is far enough along
	//if (PLAYER:GetCredits() > 10000)
	//	and (p == "Hunter")
	//	and (math.random(20) == 1)
	//then
	//	p = "Pirate"
	//	model = pirateModels[math.random(#pirateModels)]
	//	engine = "Ion Engines"
	//end

	//local s = Ship.new(name, X, Y, model, engine, p, alliance)
	
	// Get the boundaries of the current sector to use when randomizing new sprite traffic coordinates
	float north, south, east, west;
	currentScenario->GetSpriteManager()->GetBoundaries(&north, &south, &east, &west);

	// If the sector only has a single planet, its boundaries may be very small, so expand them a bit
	if(north - south < 2500) {
		north -= 1000;
		south += 1000;
	}
	if(east - west < 2500) {
		east += 1000;
		west -= 1000;
	}

	// Expand the boundaries a bit
	north *= 1.33;
	south *= 1.33;
	east *= 1.33;
	west *= 1.33;

	int x_range = east - west;
	int y_range = south - north;

	for(int i = 0; i < count; i++) {
		NPC *s = new NPC("Violet", "Trader");
		Coordinate c;

		c.SetX( (rand() % x_range) + west );
		c.SetY( (rand() % y_range) + north );
	
		s->SetWorldPosition( c );
		s->SetModel( currentScenario->GetModels()->GetModel("Large Vesper") );
		s->SetEngine( currentScenario->GetEngines()->GetEngine("Ion Engines") );
		s->SetAlliance( currentScenario->GetAlliances()->GetAlliance("United Earth Alliance") );
	
		// Add this ship to the SpriteManager
		currentScenario->GetSpriteManager()->Add((Sprite*)(s));
	
		//if p == "Pirate" then
		//	setHuntHostile(s:GetID(), PLAYER:GetID() )
		//	local escort = Ship.new( "An Escort", X-150, Y-150, choose(escortModels), "Ion Engines", "Escort", alliance)
		//	setAccompany(escort:GetID(), s:GetID())
		//end
	
		s->SetRadarColor(LIGHTBLUE);
	
		// Give every AI the standard weapons of their ship's class
		//attachStandardWeapons(s, weapons)
	
		//local creditsMax = math.random(40, 90) * math.sqrt( s:GetTotalCost() )
		//local randCredits = math.random( creditsMax )
		//s:SetCredits(randCredits)
		s->SetCredits(0);
	}
}

/**\brief Save this Sector to an xml node
 */
xmlNodePtr Sector::ToXMLNode(string componentName) {
	char buff[256] = {0};
	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str() );

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST GetName().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "alliance", BAD_CAST GetAlliance()->GetName().c_str() );
	snprintf(buff, sizeof(buff), "%d", (int)GetX() );
	xmlNewChild(section, NULL, BAD_CAST "x", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", (int)GetY() );
	xmlNewChild(section, NULL, BAD_CAST "y", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", (int)GetTraffic() );
	xmlNewChild(section, NULL, BAD_CAST "traffic", BAD_CAST buff );

	return section;
}

ostream& operator<<(ostream &out, const Sector &s) {
	out << "(" << s.name << ": " << s.x << ", " << s.y << ")";
	return out;
}

/**\class Sectors
 * \brief Collection of all Sectors
 *
 *\see Sector
 */

Sectors::Sectors() {
	rootName = "sectors";
	componentName = "sector";
}

Sector *Sectors::GetSectorByPlanet( string& PlanetName ) {
	for( map<string,Component*>::iterator i = components.begin(); i != components.end(); ++i ) {
		Sector *s = (Sector *)i->second;
		assert(s != NULL);
		if(s->HasPlanet(PlanetName)) return s;
	}

	return NULL;
}

/* Returns a list<Sector*>* with all sectors filled in. Must be freed by client. */
list<Sector*>* Sectors::GetAllSectors() {
	list<Sector *>* sectorList = new list<Sector *>();

	for( map<string,Component*>::iterator i = components.begin(); i != components.end(); ++i ) {
		Sector *s = (Sector *)i->second;
		assert(s != NULL);
		sectorList->push_back(s);
	}

	return sectorList;
}

/* Return a min/max X/Y value that covers all sector coordinates */
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

/* Returns true if 'possibleNeighbor' is a neighbor of 'origin' */
bool Sectors::SectorHasNeighbor(Sector *origin, Sector *possibleNeighbor) {
	list<string> neighbors = origin->GetNeighbors();

	// Is 'possibleNeighbor' a neighbor of 'origin'?
	if(std::find(neighbors.begin(), neighbors.end(), possibleNeighbor->GetName()) != neighbors.end()) {
		return true;
	}

	// If not, it's possible 'origin' is a neighbor of 'possibleNeighbor' ...
	neighbors = possibleNeighbor->GetNeighbors();

	// Is 'possibleNeighbor' a neighbor of 'origin'?
	if(std::find(neighbors.begin(), neighbors.end(), origin->GetName()) != neighbors.end()) {
		return true;
	}

	return false;
}

bool Sectors::Load(string filename, bool fileoptional, bool skipcorrupt) {
	if(Components::Load(filename, fileoptional, skipcorrupt) == false) { return false; }

	// Ensure sector neighbor references are symmetric, e.g. A references B so B must reference A
	for( map<string,Component*>::iterator i = components.begin(); i != components.end(); ++i ) {
		Sector *s = (Sector *)i->second;
		assert(s != NULL);

		list<string> neighbors = s->GetNeighbors();

		// Check this sector's neighbors to ensure they refer to this sector
		for( list<string>::iterator ii = neighbors.begin(); ii != neighbors.end(); ++ii ) {
			Sector *ss = GetSector(*ii);
			assert(ss != NULL);

			list<string> neighborsNeighbors = ss->GetNeighbors();
			if(std::find(neighborsNeighbors.begin(), neighborsNeighbors.end(), s->GetName()) == std::end(neighborsNeighbors)) {
				ss->AddNeighbor(s->GetName());
			}
		}
	}

	return true;
}

/** @} */

