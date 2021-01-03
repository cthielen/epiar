/**\file			spritemanager.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, August 21, 2016
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "sprites/npc.h"
#include "sprites/effects.h"
#include "sprites/spritemanager.h"
#include "utilities/log.h"
#include "utilities/quadtree.h"
#include "engine/camera.h"
#include "engine/scenario_lua.h"

/** \defgroup Sprites Sprite Objects and their Management
 * @{
 */

/**\class SpriteManager
 * \brief Mangers sprites.
 * \details
 * The SpriteManager is responsible for keeping track of all sprites.
 *
 * The Sprites themselves are stored on the Heap, but are recorded by the
 * SpriteManager in three different structures.
 * - The SpriteManager has a list of all sprites.
 *   - This list is ordered by sprite creations, but the order is not enforced.
 *   - This list can be requested as a whole, or filtered by requesting only a
 *     certain Sprite Type.
 *   \see GetSprites
 * - The SpriteManager has a set of QuadTrees.
 *   - The QuadTree is a recursive datastructure that stores the Sprites by their
 *     Universal location.  The Quadtree segments the Sprites based on how
 *     relativly close the Sprites are.
 *   - Each QuadTree is only a finite size, but can theoretically hold an infinte
 *     number of sprites.
 *   - The entire universe is broken up into a grid of QuadTrees.
 *     Only grid positions with Sprites in them are actually populated by QuadTrees.
 *   - The QuadTrees cannot be accessed directly, but are used implicitely when
 *     requesting sprites by a location.
 *   \see QuadTree
 *   \see GetSpritesNear
 *   \see GetNearestSprite
 * - The SpriteManager has a map of all Sprites by their unique ID.
 *   - Sprites can be queried by passing an ID.
 *   \see GetSpriteByID
 *
 * Sprites are never deleted immediately.  This is to prevent a Sprite from
 * being deleted during the middle of the Update Loop.  Instead, 'deleted'
 * Sprites are recorded in a list and deleted in a batch once per Update.
 *
 */

/**\brief Constructs a new sprite manager.
 */
//initialise the tick stuff - these should probably be set by an option somewhere, hardcode for now
SpriteManager::SpriteManager() :
	 tickCount (0)
	 , semiRegularPeriod (15)		//every 16 ticks we want to have updated the semi-regular distance quadrants
	 , fullUpdatePeriod (120)		//update the full quadrant map every 120 ticks
	 , numRegularBands (2)			//the regular (per-tick) updates are on this number of bands
	 , numSemiRegularBands (5)		//the semi-regular updates are on this number of bands - this SHOULD be easily divisible into semiRegularPeriod
{
	player = NULL;

	spritelist = new list<Sprite*>();
	spritelookup = new map<int,Sprite*>();

	//fill in the ticksToBandNum map based on the semiRegularPeriod and numSemiRegularBands
	int updateGap = semiRegularPeriod / numSemiRegularBands;

	for (int i = 0; i < numSemiRegularBands; i ++) {
		ticksToBandNum[(updateGap * i)] = numRegularBands + 1 + i;		//assign one of the semi-regular bands to a tick
	}
}

SpriteManager::~SpriteManager() {

}

/**\brief Adds a sprite to the manager.
 * \param sprite Pointer to the sprite
 */
void SpriteManager::Add( Sprite *sprite ) {
	spritelist->push_back(sprite);
	spritelookup->insert(make_pair(sprite->GetID(),sprite));
	GetQuadrant( sprite->GetWorldPosition() )->Insert( sprite );
}

/**\brief Adds player sprite to the manager.
 * \param sprite Pointer to the player sprite
 */
void SpriteManager::AddPlayer( Sprite *sprite ) {
	player = sprite;
	Add( sprite );
}

/**\brief Deletes a sprite from the manager (Internal use).
 * \param sprite Pointer to the sprite
 * \details
 * This performs the actual deletion.
 */
bool SpriteManager::DeleteSprite( Sprite *sprite ) {
	if(sprite == player) LogMsg(WARN, "Deleting player sprite. Should we be doing this?");

	spritelist->remove( sprite );
	spritelookup->erase( sprite->GetID() );

	GetQuadrant( sprite->GetWorldPosition() )->Delete( sprite );

	// Delete the sprite itself unless it is a Planet or Player.
	// Planets and Players are special sprites since they are Components and get saved.
	if( !(sprite->GetDrawOrder() & (DRAW_ORDER_PLAYER | DRAW_ORDER_PLANET )) ) {
		delete sprite;
	}

	return true;
}

// Removes all sprites matching type 'type'
void SpriteManager::DeleteByType( int type ) {
	list<Sprite *>::iterator i;

	for( i = spritelist->begin(); i != spritelist->end(); ++i ) {
		Sprite *s = (*i);
		if( s->GetDrawOrder() == type ) {
			DeleteSprite( s );
			i = spritelist->begin(); // TODO: spritelist->remove() is called which ... makes this necessary? Right?
		}
	}
}

// Remove every sprite (planet, AI ship, projectile, effect, etc.) except the player's sprite
void SpriteManager::DeleteAllExceptPlayer( void ) {
	list<Sprite *>::iterator i;

	for( i = spritelist->begin(); i != spritelist->end(); ++i ) {
		Sprite *s = (*i);
		if( s->GetDrawOrder() != DRAW_ORDER_PLAYER ) {
			DeleteSprite( s );
			i = spritelist->begin(); // TODO: spritelist->remove() is called which ... makes this necessary? Right?
		}
	}
}

/**\brief Deletes a sprite.
 * \param sprite Pointer to the sprite object
 * \details
 * This just queues the sprite up to be deleted.
 */
bool SpriteManager::Delete( Sprite *sprite ) {
	spritesToDelete.push_back(sprite);

	return true;
}

void SpriteManager::UpdateScreenCoordinates( void ) {
	list<Sprite *>::iterator i;

	for( i = spritelist->begin(); i != spritelist->end(); ++i ) {
		(*i)->UpdateScreenCoordinates();
	}
}

/**\brief SpriteManager update function.
 * \details Update the sprites inside each quadrant
 * \param lowFps If true, forces the wave-update method to be used rather than the full-update
 */
void SpriteManager::Update( lua_State *L, bool lowFps) {
	//this will contain every quadrant that we will potentially want to update
	list<QuadTree*> quadList;

	//if update-all is given then we update every quadrant
	//we do the same if tickCount == 0 even if update-all is not given
	// (in wave update mode, tickCount == 0 is when we want to update all quadrants)
	if( ! lowFps || tickCount == 0) {
		//need to get all of the quadrants in our map
		GetAllQuadrants(&quadList);
	} else {
		//wave update mode with tickCount != 0 -- update some quadrants
		Camera* camera = Scenario_Lua::GetScenario(L)->GetCamera();
		Coordinate currentPoint(camera->GetFocusCoordinate());	//always update centered on where we're at

		quadList.push_back(GetQuadrant (currentPoint)); //we ALWAYS update the current quadrant

		//we also ALWAYS update the 'regular' bands
		//	the first band is at index 1 - index 0 would be the single quadrant in the middle
		//	when we get the list of quadrants back we splice them onto the end of our overall list
		for (int i = 1; i <= numRegularBands; i ++) {
			list<QuadTree*> tempBandList = GetQuadrantsInBand(currentPoint, i);
			quadList.splice(quadList.end(), tempBandList);
		}

		//now - we SOMETIMES update the semi-regular bands
		//   the ticks that each band is updated in is stored in the map
		//   so we get our semiRegular update modulus of the ticks and then check the map
		//    - the map has the tick index as the key and the band to update as the value
		int semiRegularTick = tickCount % semiRegularPeriod;
		map<int,int>::iterator findBand = ticksToBandNum.find (semiRegularTick);
		if (findBand != ticksToBandNum.end()) {		//found the key
			//cout << "tick = " << tickCount << ", semiRegularTick = " << semiRegularTick << ", band = " << findBand->second << endl;
			list<QuadTree*> tempBandList = GetQuadrantsInBand (currentPoint, findBand->second);
			quadList.splice(quadList.end(), tempBandList);
		}
		else {
			//no semi-regular bands to update at this tick, do nothing
		}
	}

	// Find and Fix any Sprites that have moved out of bounds.
	list<Sprite *> all_oob;
	list<QuadTree*>::iterator iter;
	for ( iter = quadList.begin(); iter != quadList.end(); ++iter ) {
		(*iter)->Update(L);
		list<Sprite *>* oob = (*iter)->FixOutOfBounds();
		all_oob.splice(all_oob.end(), *oob );
		delete oob;
	}

	// Move sprites to adjacent Quadrants as they cross boundaries
	list<Sprite *>::iterator i;
	for( i = all_oob.begin(); i != all_oob.end(); ++i ) {
		GetQuadrant( (*i)->GetWorldPosition() )->Insert( *i );
	}

	// Delete all sprites queued to be deleted
	if (!spritesToDelete.empty()) {
		spritesToDelete.sort(); // The list has to be sorted or unique doesn't work correctly.
		spritesToDelete.unique();

		// Tell the AI that they've been killed
		for( i = spritesToDelete.begin(); i != spritesToDelete.end(); ++i ) {
			if( (*i)->GetDrawOrder() == DRAW_ORDER_SHIP ) {
				((NPC*)(*i))->Killed(L);
			}
		}

		for( i = spritesToDelete.begin(); i != spritesToDelete.end(); ++i ) {
			DeleteSprite(*i);
		}
		spritesToDelete.clear();
	}

	for ( iter = quadList.begin(); iter != quadList.end(); ++iter ) {
		(*iter)->ReBallance();
	}

	DeleteEmptyQuadrants();

	// Update the tick count after all updates for this tick are done
	UpdateTickCount();
}

/**\brief Returns the number of non-player (AI) ships.
 */
int SpriteManager::GetAIShipCount( void ) {
	list<Sprite *>::iterator i;
	int count = 0;

	for( i = spritelist->begin(); i != spritelist->end(); ++i ) {
		Sprite *s = (*i);
		if(( s->GetDrawOrder() == DRAW_ORDER_SHIP ) && (s != player)) {
			count++;
		}
	}

	return count;
}

/**\brief Deletes empty QuadTrees (Internal use)
 */
void SpriteManager::DeleteEmptyQuadrants() {
	map<Coordinate,QuadTree*>::iterator iter;
	// Delete QuadTrees that are empty
	// TODO: Delete QuadTrees that are far away from
	list<QuadTree*> emptyTrees;
	// Collect empty trees
	for ( iter = trees.begin(); iter != trees.end(); ++iter ) {
		if ( iter->second->Count() == 0 ) {
			emptyTrees.push_back(iter->second);
		}
	}
	// Delete empty trees
	list<QuadTree*>::iterator emptyIter;
	for ( emptyIter = emptyTrees.begin(); emptyIter != emptyTrees.end(); ++emptyIter) {
		//cout<<"Deleting the empty tree at "<<(*emptyIter)->GetCenter()<<endl;
		trees.erase((*emptyIter)->GetCenter());
		delete (*emptyIter);
	}
	if( emptyTrees.size() ) {
		AdjustBoundaries();
	}
}

/** \brief Comparator function for ordering Sprites
 *
 * \details The goal here is to order the sprites in a deterministic way.
 *          We also need the Sprites to be ordered by their DRAW_ORDER.
 *          Since the Sprite ID is unique, and monotonically increasing, this
 *          will sort older sprites below newer sprites.
 *
 * \param a A pointer to a Sprite.
 * \param b A pointer to another Sprite.
 * \returns True if Sprite a should be below Sprite b.
 *
 * \relates Sprite
 */

bool compareSpritePtrs(Sprite* a, Sprite* b){
	if(a->GetDrawOrder() != b->GetDrawOrder()) {
		return a->GetDrawOrder() < b->GetDrawOrder();
	} else {
		return a->GetID() < b->GetID();
	}
}

/**\brief Draws the current sprites
 */
void SpriteManager::Draw( Coordinate focus ) {
	list<Sprite *>::iterator i;
	list<Sprite*> *onscreen = NULL;
	float r = (Video::GetHalfHeight() < Video::GetHalfWidth() ? Video::GetHalfWidth() : Video::GetHalfHeight()) * V_SQRT2;
	onscreen = GetSpritesNear( focus, r, DRAW_ORDER_ALL);

	onscreen->sort(compareSpritePtrs);

	for( i = onscreen->begin(); i != onscreen->end(); ++i ) {
		(*i)->Draw();
	}

	delete onscreen;
}

/**\brief Draws the current sprites
 */
void SpriteManager::DrawQuadrantMap( Coordinate focus ) {
	GetQuadrant( focus )->Draw( GetQuadrantCenter( focus ) );
}

/**\brief Retrieves a list of the current sprites.
 * \return std::list of Sprite pointers.
 */
list<Sprite *> *SpriteManager::GetSprites(int type) {
	list<Sprite *>::iterator i;
	list<Sprite *> *filtered;
	if( type==DRAW_ORDER_ALL ){
		filtered = new list<Sprite*>(*spritelist);
	} else {
		filtered = new list<Sprite*>();
		// Collect only the Sprites of this type
		for( i = spritelist->begin(); i != spritelist->end(); ++i ) {
			if( (*i)->GetDrawOrder() & type){
				filtered->push_back( (*i) );
			}
		}
	}
	return( filtered);
}

/**\brief Queries for sprite by the ID
 * \param id Identification of the sprite.
 */
Sprite *SpriteManager::GetSpriteByID(int id) {
	map<int,Sprite*>::iterator val = spritelookup->find( id );
	if( val != spritelookup->end() ){
		return val->second;
	}
	return NULL;
}

/**\brief Retrieves nearby QuadTrees in a square band at <bandIndex> quadrants distant from the coordinate
 * \param c Coordinate
 * \param bandIndex number of quadrants distant from c
 * \return std::list of QuadTree pointers.
 */
list<QuadTree*> SpriteManager::GetQuadrantsInBand ( Coordinate c, int bandIndex) {
	// The possibleQuadrants here are the quadrants that are in the square band
	//  at distance bandIndex from the coordinate
	// After we get the possible quadrants we prune them by making sure they exist
	//  (ie that something is in them)

	list<QuadTree*> nearbyQuadrants;
	set<Coordinate> possibleQuadrants;

	//note that the QUADRANTSIZE define is the
	//	distance from the middle to the edge of a quadrant
	//to get the square band of co-ordinates we have to
	//		- start at bottom left
	//			loop over increasing Y (to get 'west' line)
	//			loop over increasing X (to get 'south' line)
	//		- start at top right
	//			loop over decreasing Y (to get 'east' line)
	//			loop over decreasing X (to get 'north' line)

	int edgeDistance = bandIndex * QUADRANTSIZE * 2;		//number of pixels from middle to the band
	Coordinate bottomLeft (c - Coordinate (edgeDistance, edgeDistance));
	Coordinate topLeft (c + Coordinate (-edgeDistance, edgeDistance));
	Coordinate topRight (c + Coordinate (edgeDistance, edgeDistance));
	Coordinate bottomRight (c + Coordinate (edgeDistance, -edgeDistance));

	//the 'full' length of one of the lines is (bandindex * 2) + 1
	//we don't need the +1 as we deal with the corners individually, separately
	int bandLength = (bandIndex * 2);

	//deal with the un-included corners first
	//we're using bottomLeft and topRight as the anchors,
	// so topLeft and bottomRight are added here
	possibleQuadrants.insert (GetQuadrantCenter (topLeft));
	possibleQuadrants.insert (GetQuadrantCenter (bottomRight));
	for (int i = 0; i < bandLength; i ++) {
		int offset = ((QUADRANTSIZE * 2) * i);
		Coordinate west, south, north, east;

		west = GetQuadrantCenter (bottomLeft + Coordinate (0, offset));
		south = GetQuadrantCenter (bottomLeft + Coordinate (offset, 0));
		north = GetQuadrantCenter (topRight - Coordinate (offset, 0));
		east = GetQuadrantCenter (topRight - Coordinate (0, offset));

		possibleQuadrants.insert (west);		//west
		possibleQuadrants.insert (south);		//south
		possibleQuadrants.insert (north);		//north
		possibleQuadrants.insert (east);		//east
	}

	//here we're checking to see if this possible quadrant is one of the existing quadrants
	// and if it is then we add its QuadTree to the vector we're returning
	// if it's not then there's nothing in it anyway so we don't care about it
	set<Coordinate>::iterator it;
	map<Coordinate,QuadTree*>::iterator iter;
	for(it = possibleQuadrants.begin(); it != possibleQuadrants.end(); ++it) {
		iter = trees.find(*it);
		if(iter != trees.end()) {
			nearbyQuadrants.push_back(iter->second);
		}

	}
	return nearbyQuadrants;
}


/**\brief Retrieves nearby QuadTrees
 * \param c Coordinate
 * \param r Radius
 * \return std::list of QuadTree pointers.
 */
list<QuadTree*> SpriteManager::GetQuadrantsNear( Coordinate c, float r) {
	// The possibleQuadrants are those trees adjacent and within a radius r
	// Gather more trees when r is greater than the size of a quadrant
	map<Coordinate,QuadTree*>::iterator iter;
	list<QuadTree*> nearbyQuadrants;
	set<Coordinate> possibleQuadrants;

	Coordinate center = GetQuadrantCenter(c);

	possibleQuadrants.insert( center );
	float R = r;
	do{
		possibleQuadrants.insert( GetQuadrantCenter(c + Coordinate(-R,-0)));
		possibleQuadrants.insert( GetQuadrantCenter(c + Coordinate(-0,+R)));
		possibleQuadrants.insert( GetQuadrantCenter(c + Coordinate(+0,-R)));
		possibleQuadrants.insert( GetQuadrantCenter(c + Coordinate(+R,+0)));
		possibleQuadrants.insert( GetQuadrantCenter(c + Coordinate(-R,-R)));
		possibleQuadrants.insert( GetQuadrantCenter(c + Coordinate(-R,+R)));
		possibleQuadrants.insert( GetQuadrantCenter(c + Coordinate(+R,-R)));
		possibleQuadrants.insert( GetQuadrantCenter(c + Coordinate(+R,+R)));
		R/=2;
	} while(R>QUADRANTSIZE);
	set<Coordinate>::iterator it;
	for(it = possibleQuadrants.begin(); it != possibleQuadrants.end(); ++it) {
		//here we're checking to see if this possible quadrant is one of the existing quadrants
		// and if it is then we add its QuadTree to the vector we're returning
		//how about we try creating the quadrant if it does not already exist
		iter = trees.find(*it);
		if(iter != trees.end() && iter->second->PossiblyNear(c,r)){
			nearbyQuadrants.push_back(iter->second);
		}
	}
	return nearbyQuadrants;
}


/**\brief Creates a binary comparison object that can be passed to stl sort.
 * Sprites will be sorted by distance from the point in ascending order.
 * \relates Sprite
 */
struct compareSpriteDistFromPoint
	: public std::binary_function<Sprite*, Sprite*, bool>
{
	compareSpriteDistFromPoint(const Coordinate& c) : point(c) {} ///< Default Constructor(?)

    /**\brief Compare two sprites by their distance from a Coordinate (c).
     * \details This uses GetMagnitudeSquared to calculate the distance from a
     *          Sprite so that we don't have to run a costly sqrt.
     * \param a A pointer to a Sprite.
     * \param b A pointer to another Sprite.
     * \returns True if Sprite a is closer than Sprite b to point c.
     *
     */
	bool operator() (Sprite* a, Sprite* b) {
		return (point - a->GetWorldPosition()).GetMagnitudeSquared()
		     < (point - b->GetWorldPosition()).GetMagnitudeSquared() ;
	}

	Coordinate point;
};

/**\brief Returns a list of sprites that are near coordinate.
 * \param c Coordinate
 * \param r Radius
 * \return std::list of Sprite pointers.
 */
list<Sprite*> *SpriteManager::GetSpritesNear(Coordinate c, float r, int type) {
	list<Sprite*> *sprites = new list<Sprite*>();

	// Search the possible quadrants
	list<QuadTree*> nearbyQuadrants = GetQuadrantsNear(c,r);
	list<QuadTree*>::iterator it;
	for(it = nearbyQuadrants.begin(); it != nearbyQuadrants.end(); ++it) {
			list<Sprite *>* nearby = new list<Sprite*>;
			(*it)->GetSpritesNear(c,r,nearby,type);
			sprites->splice(sprites->end(), *nearby);
			delete nearby;
	}

	// Sort sprites by their distance from the coordinate c
	sprites->sort(compareSpriteDistFromPoint(c));
	return( sprites );
}

/**\brief Get a Sprite nearest to another Sprite.
 * \details Rather than just accept a Coordinate, this requires another Sprite
 *          because the common usage is to look for a nearby enemy or
 *          collision.  Since a Sprite is by definition the closest thing to
 *          its own position, this function needs to know what sprite to ignore.
 *
 *          The Usual use for this is to find the nearest sprite of a certain type.
\verbatim
 	Sprite* found = GetNearestSprite(mySprite, 1000, DRAW_ORDER_SHIP);
\endverbatim
 *
 *          Rather than re-implement this code for when the caller truely
 *          doesn't care about a Sprite, the caller can supply a dummy Sprite like:
\verbatim
 	Sprite* found = GetNearestSprite(Effect( Coordinate(0,0),"",0), 1000);
\endverbatim
 *
 * \todo Build the Dummy example above into the SpriteManager.
 *
 */
Sprite* SpriteManager::GetNearestSprite(Sprite* obj, float r, int type) {
	float tmpdist;
	Sprite* closest=NULL;
	Sprite* possible=NULL;
	if(obj==NULL)
		return (Sprite*)NULL;
	list<QuadTree*> nearbyQuadrants = GetQuadrantsNear(obj->GetWorldPosition(),r);
	list<QuadTree*>::iterator it;
	for(it = nearbyQuadrants.begin(); it != nearbyQuadrants.end(); ++it) {
		possible = (*it)->GetNearestSprite(obj,r, type);
		if(possible!=NULL) {
			tmpdist = (obj->GetWorldPosition()-possible->GetWorldPosition()).GetMagnitude();
			if(tmpdist<r) {
				r = tmpdist;
				closest = possible;
			}
		}
	}
	return closest;
}

Sprite* SpriteManager::GetNearestSprite(Coordinate c, float r, int type) {
	// This dummy variable is a local variable and will be deleted immediately after.
	Effect dummy( c, "data/animations/shield.ani", 0);
	return GetNearestSprite( &dummy, r, type );
}

/**\brief Returns QuadTree center.
 * \param point Coordinate
 * \return Coordinate of centerpointer
 */
Coordinate SpriteManager::GetQuadrantCenter(Coordinate point){
	// Figure out where the new Tree should go.
	// Quadrants are tiled adjacent to the central Quadrant centered at (0,0).
	double cx, cy;
	cx = float(floor( (point.GetX()+QUADRANTSIZE)/(QUADRANTSIZE*2.0f)) * QUADRANTSIZE*2.f);
	cy = float(floor( (point.GetY()+QUADRANTSIZE)/(QUADRANTSIZE*2.0f)) * QUADRANTSIZE*2.f);
	return Coordinate(cx,cy);
}

/**\brief Gets the number of Sprites in the SpriteManager
 */
int SpriteManager::GetNumSprites() {
	unsigned int total = 0;
	map<Coordinate,QuadTree*>::iterator iter;

	for ( iter = trees.begin(); iter != trees.end(); ++iter ) {
		total += iter->second->Count();
	}

	assert( total == spritelist->size() );
	assert( total == spritelookup->size() );

	return total;
}

/**\brief Returns QuadTree at Coordinate
 * \param point Coordinate
 */
QuadTree* SpriteManager::GetQuadrant( Coordinate point ) {
	Coordinate treeCenter = GetQuadrantCenter(point);

	// Check in the known Quadrant
	map<Coordinate,QuadTree*>::iterator iter;
	iter = trees.find( treeCenter );
	if( iter != trees.end() ) {
		return iter->second;
	}

	// Create the new Tree and attach it to the universe
	QuadTree *newTree = new QuadTree(treeCenter, QUADRANTSIZE);
	assert(treeCenter == newTree->GetCenter() );
	assert(newTree->Contains(point));
	trees.insert(make_pair(treeCenter, newTree));
	AdjustBoundaries();

	// Debug
	//cout<<"A Tree at "<<treeCenter<<" was created to contain "<<point<<". "<<trees.size()<<" Quadrants exist now."<<endl;

	return newTree;
}

/**\brief Get the min/max planet positions. Useful when generating traffic.
 * \note Returns the values through the pointer arguments.
 */
void SpriteManager::GetBoundaries(float *north, float *south, float *east, float *west)
{
	*north = *south = *east = *west = 0;

	list<Sprite *>::iterator i;

	for( i = spritelist->begin(); i != spritelist->end(); ++i ) {
		Sprite *s = (*i);

		if( s->GetDrawOrder() == DRAW_ORDER_PLANET ) {
			Coordinate c = s->GetWorldPosition();

			if(c.GetY() < *north) *north = c.GetY();
			if(c.GetY() > *south) *south = c.GetY();
			if(c.GetX() < *west) *west = c.GetX();
			if(c.GetX() > *east) *east = c.GetX();
		}
	}
}

/** Adjust the Edges based on the locations of the populated QuadTrees
 */
void SpriteManager::AdjustBoundaries()
{
	Coordinate c;
	map<Coordinate,QuadTree*>::iterator iter;

	northEdge = southEdge = eastEdge = westEdge = 0;
	for ( iter = trees.begin(); iter != trees.end(); ++iter ) {
		c = iter->first;
		if( c.GetY() > northEdge) northEdge = c.GetY();
		if( c.GetY() < southEdge) southEdge = c.GetY();
		if( c.GetX() > eastEdge)  eastEdge  = c.GetX();
		if( c.GetX() < westEdge)  westEdge  = c.GetX();
	}
}

/**\brief Save an XML file of all of the Sprites.
 * \details
 * Traverse each Quadtree looking for sprites.
 * Each Sprite will be an XML node.
 * Each Quadtee Leaf or Node will be an XML Node.
 *
 * The point of this is to create a file that could be useful for debugging quadtree problems.
 */
void SpriteManager::Save() {
	map<Coordinate,QuadTree*>::iterator iter;
	xmlDocPtr doc = NULL;       /* document pointer */
	xmlNodePtr root_node = NULL;/* node pointers */

	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "Sprites" );
	xmlDocSetRootElement(doc, root_node);

	for ( iter = trees.begin(); iter != trees.end(); ++iter ) {
		xmlAddChild( root_node, iter->second->ToNode() );
	}

	xmlSaveFormatFileEnc( "Sprites.xml" , doc, "ISO-8859-1", 1);
	xmlFreeDoc( doc );
}

/**\brief Count up to fullUpdatePeriod
 */
void SpriteManager::UpdateTickCount ()
{
	tickCount ++;
			//we could do a modulus here but I think this will average out more efficient
	if (tickCount >= fullUpdatePeriod)
		tickCount -= fullUpdatePeriod;
}

/**\brief Populate a list of all Quadtrees.
 * \details Used for looping between all Quadrants.
 * \warn
 * This is not very efficient, but std::transform doesn't work...
 * (if for some reason we got transform working, the idea would be to have
 *   a helper method to get map->second to pass as the 4th argument of transform
 *   with the third argument being a back_inserter into the list we want)
 */
void SpriteManager::GetAllQuadrants (list<QuadTree*> *newList)
{
	map<Coordinate,QuadTree*>::iterator mapIter = trees.begin();
	while (mapIter != trees.end())
	{
		newList->push_back (mapIter->second);
		++ mapIter;
	}
}

/** @} */
