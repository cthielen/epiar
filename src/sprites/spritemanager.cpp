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
	spritelookup->insert(make_pair(sprite->GetID(), sprite));
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

	//GetQuadrant( sprite->GetWorldPosition() )->Delete( sprite );

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
	list<Sprite*>::iterator i;

	for( i = spritelist->begin(); i != spritelist->end(); ++i ) {
		(*i)->Update( L );
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
	float r = (Video::GetHalfHeight() < Video::GetHalfWidth() ? Video::GetHalfWidth() : Video::GetHalfHeight()) * V_SQRT2;
	list<Sprite*> *onScreen = GetSpritesNear(focus, r, DRAW_ORDER_ALL);

	onScreen->sort(compareSpritePtrs);

	for( i = onScreen->begin(); i != onScreen->end(); ++i ) {
		(*i)->Draw();
	}

	delete onScreen;
}

/**\brief Retrieves a list of the current sprites.
 * \return std::list of Sprite pointers.
 */
list<Sprite *> *SpriteManager::GetSprites(int type) {
	list<Sprite *>::iterator i;
	list<Sprite *> *filtered;

	if( type == DRAW_ORDER_ALL ){
		filtered = new list<Sprite*>(*spritelist);
	} else {
		filtered = new list<Sprite*>();
		// Collect only the Sprites of this type
		for( i = spritelist->begin(); i != spritelist->end(); ++i ) {
			if( (*i)->GetDrawOrder() & type) {
				filtered->push_back( (*i) );
			}
		}
	}

	return filtered;
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
 * \param r Radius - zero indicates infinite radius
 * \return std::list of Sprite pointers.
 */
list<Sprite*> *SpriteManager::GetSpritesNear(Coordinate c, float r, int type, bool sorted) {
	list<Sprite*> *sprites = new list<Sprite*>();

	for( list<Sprite*>::iterator i = spritelist->begin(); i != spritelist->end(); ++i ) {
		Sprite* s = *i;
		if( (s->GetDrawOrder() & type) == 0) continue;

		Image *spriteImage = s->GetImage();
		int half_w, half_h;

		if(spriteImage == NULL) {
			// Effect sprites have animations, not images
			half_w = half_h = 0;
		} else {
			half_w = s->GetImage()->GetHalfWidth();
			half_h = s->GetImage()->GetHalfHeight();
		}
		float rr = r + (half_w > half_h ? half_w : half_h);
		rr *= rr;

		if( (r == 0) || ((c - s->GetWorldPosition()).GetMagnitudeSquared() <= rr) ) {
			sprites->push_back(s);
		}
	}

	if(sorted) {
		// Sort sprites by their distance from the coordinate c
		sprites->sort(compareSpriteDistFromPoint(c));
	}

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
	Coordinate worldPosition = obj->GetWorldPosition();
	list<Sprite*> *nearbySprites = GetSpritesNear(worldPosition, r, type);

	nearbySprites->sort(compareSpriteDistFromPoint(worldPosition));

	Sprite* nearestSprite = nearbySprites->front();

	delete nearbySprites;

	return nearestSprite;
}

Sprite* SpriteManager::GetNearestSprite(Coordinate c, float r, int type) {
	// This dummy variable is a local variable and will be deleted immediately after.
	Effect dummy( c, "data/animations/shield.ani", 0);
	return GetNearestSprite( &dummy, r, type );
}

/**\brief Gets the number of Sprites in the SpriteManager
 */
int SpriteManager::GetNumSprites() {
	return spritelist->size();
}

/**\brief Get the min/max planet positions. Useful when generating traffic.
 * \note Returns the values through the pointer arguments.
 */
void SpriteManager::GetBoundaries(float *north, float *south, float *east, float *west) {
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

/**\brief Save an XML file of all of the Sprites.
 * \details
 * Traverse each Quadtree looking for sprites.
 * Each Sprite will be an XML node.
 * Each Quadtee Leaf or Node will be an XML Node.
 *
 * The point of this is to create a file that could be useful for debugging quadtree problems.
 */
void SpriteManager::Save() {
	xmlDocPtr doc = NULL;       /* document pointer */
	xmlNodePtr root_node = NULL;/* node pointers */

	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "Sprites" );
	xmlDocSetRootElement(doc, root_node);

	for( list<Sprite *>::iterator i = spritelist->begin(); i != spritelist->end(); ++i ) {
		char buff[256] = {0};
		xmlNodePtr objNode;

		switch((*i)->GetDrawOrder()) {
			case DRAW_ORDER_PLANET:
				snprintf(buff, sizeof(buff), "%s", "Planet" );
				break;
			case DRAW_ORDER_PROJECTILE:
				snprintf(buff, sizeof(buff), "%s", "Weapon" );
				break;
			case DRAW_ORDER_SHIP:
				snprintf(buff, sizeof(buff), "%s", "Ship" );
				break;
			case DRAW_ORDER_PLAYER:
				snprintf(buff, sizeof(buff), "%s", "Player" );
				break;
			case DRAW_ORDER_EFFECT:
				snprintf(buff, sizeof(buff), "%s", "Effect" );
				break;
			default:
				LogMsg(ERR,"Unknown Sprite Type: %d",(*i)->GetDrawOrder());
				assert(0);
				break;
		}

		objNode = xmlNewNode(NULL, BAD_CAST buff);
		snprintf(buff, sizeof(buff), "%d", (int) (*i)->GetWorldPosition().GetX() );
		xmlSetProp( objNode, BAD_CAST "x", BAD_CAST buff );
		snprintf(buff, sizeof(buff), "%d", (int) (*i)->GetWorldPosition().GetY() );
		xmlSetProp( objNode, BAD_CAST "y", BAD_CAST buff );
		snprintf(buff, sizeof(buff), "%d", (int) (*i)->GetAngle() );
		xmlSetProp( objNode, BAD_CAST "angle", BAD_CAST buff );
		xmlAddChild(root_node, objNode);
	}

	xmlSaveFormatFileEnc( "Sprites.xml" , doc, "ISO-8859-1", 1);
	xmlFreeDoc( doc );
}

/**\brief Count up to fullUpdatePeriod
 */
void SpriteManager::UpdateTickCount () {
	tickCount ++;
			//we could do a modulus here but I think this will average out more efficient
	if (tickCount >= fullUpdatePeriod)
		tickCount -= fullUpdatePeriod;
}

/** @} */
