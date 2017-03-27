/**\file			spritemanager.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, August 21, 2016
 * \brief
 * \details
 */

#ifndef __H_SPRITEMANAGER__
#define __H_SPRITEMANAGER__

#include "sprites/sprite.h"
#include "utilities/quadtree.h"

class SpriteManager {
	public:
		SpriteManager();
		~SpriteManager();

		void Add( Sprite *sprite );
		void AddPlayer( Sprite *sprite );
		bool Delete( Sprite *sprite );

		void DeleteByType( int type );
		void DeleteAllExceptPlayer( void );

		void Update( lua_State *L, bool lowFps);
		void UpdateScreenCoordinates( void );
		void Draw( Coordinate focus );
		void DrawQuadrantMap( Coordinate focus );

		int GetAIShipCount( void );

		Sprite *GetSpriteByID(int id);
		list<Sprite*> *GetSprites(int type = DRAW_ORDER_ALL);
		list<Sprite*> *GetSpritesNear(Coordinate c, float r, int type = DRAW_ORDER_ALL);
		Sprite* GetNearestSprite(Sprite *obj, float r, int type = DRAW_ORDER_ALL);
		Sprite* GetNearestSprite(Coordinate c, float r, int type = DRAW_ORDER_ALL);

		Coordinate GetQuadrantCenter( Coordinate point );
		int GetNumQuadrants() { return trees.size(); }
		int GetNumSprites();
		//void GetBoundaries(float *northEdge, float *southEdge, float *eastEdge, float *westEdge);

		void Save();

	private:
		// These structures each contain a complete list of all Sprites.
		// Each one is useful for a different purpose, depending on the way that the sprites need to be accessed.
		map<Coordinate,QuadTree*> trees;    ///< Collection of all Sprites. Use this tree when referring to the sprites at a location.
		list<Sprite*> *spritelist;          ///< Collection of all Sprites. Use this list when referring to all sprites.
		map<int,Sprite*> *spritelookup;     ///< Collection of all Sprites. Use the map when referring to sprites by their unique ID.

		Sprite *player;                     ///< The Player Sprite.

		list<Sprite *> spritesToDelete;     ///< The list of Sprites that should be deleted at the end of this Update.

		int tickCount;                      ///< Counts number of ticks to track updates to quadrants.  Max value is the number of ticks to update all quadrants
		const int semiRegularPeriod;        ///< The period at which every semi-regular quadrant is updated
		const int fullUpdatePeriod;         ///< The period at which every quadrant is updated regardless of distance

		const int numRegularBands;          ///< The number of bands surrounding the centre point that are updated every tick
		const int numSemiRegularBands;      ///< The number of bands surrounding the centre point that are updated semi-regularly
		map<int, int> ticksToBandNum;       ///< The key is the tick# that the value band# will be updated at

		float northEdge, southEdge, eastEdge, westEdge; ///< The Edges of the universe

		bool DeleteSprite( Sprite *sprite );
		void DeleteEmptyQuadrants( void );
		QuadTree* GetQuadrant( Coordinate point );
		list<QuadTree*> GetQuadrantsNear( Coordinate c, float r);
		list<QuadTree*> GetQuadrantsInBand ( Coordinate c, int bandIndex);
		void AdjustBoundaries();
		void UpdateTickCount();

		void GetAllQuadrants( list<QuadTree*> *newTree);
};

#endif // __H_SPRITEMANAGER__
