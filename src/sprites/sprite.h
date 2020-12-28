/**\file			sprite.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#ifndef __h_sprite__
#define __h_sprite__

#include "graphics/image.h"
#include "graphics/video.h"
#include "utilities/lua.h"
#include "utilities/coordinate.h"

// With the draw order, higher numbers are drawn later (on top)
// By using non-overlapping bits we can bit mask during searches
#define DRAW_ORDER_PLANET              0x0001 ///< Draw order for Planet Sprites
#define DRAW_ORDER_PROJECTILE          0x0002 ///< Draw order for Projectile Sprites
#define DRAW_ORDER_SHIP                0x0004 ///< Draw order for Ship Sprites
#define DRAW_ORDER_PLAYER              0x0008 ///< Draw order for Player Sprites
#define DRAW_ORDER_EFFECT              0x0010 ///< Draw order for Effect Sprites (Explosions)
#define DRAW_ORDER_ALL                 0xFFFF ///< Default DRAW_ORDER for searches that filter.

class Sprite {
	public:
		Sprite();
		virtual ~Sprite() {};

		Coordinate GetWorldPosition( void ) const;
		void SetWorldPosition( Coordinate coord );

		Coordinate GetScreenPosition( void ) const;

		virtual void Update( lua_State *L );
		void UpdateScreenCoordinates( void );
		virtual void Draw( void );

		int GetID( void ) { return id; }

		float GetAngle( void ) const {
			return( angle );
		}
		void SetAngle( float angle ) {
			this->angle = angle;
		}
		Coordinate GetMomentum( void ) const {
			return momentum;
		}
		void SetMomentum( Coordinate momentum ) {
			this->momentum = momentum;
		}
		Coordinate GetAcceleration( void ) const {
			return acceleration;
		}
		void SetImage( Image *image ) {
			assert(image);
			this->image = image;
			this->radarSize = ( image->GetWidth() + image->GetHeight() ) / 1.7;
		}
		void SetRadarColor( Color col ){
			this->radarColor = col;
		}
		Image *GetImage( void ) const {
			return image;
		}
		int GetRadarSize( void ) { return radarSize; }
		virtual Color GetRadarColor( void ) { return radarColor; }
		virtual int GetDrawOrder( void ) = 0;

	private:
		static long int sprite_ids; ///< The ID for the next Sprite.

		int id; ///< The unique ID of this Sprite.
		Coordinate oldScreenPosition, screenPosition; ///< The Current position of this Sprite.
		Coordinate worldPosition; ///< The Current position of this Sprite.
		Coordinate momentum; ///< The current Speed and Direction that this Sprite is moving (not pointing).
		Coordinate acceleration; ///< The ammount that the Sprite accelerated during the previous Update.
		Coordinate lastMomentum; ///< The momentum that this Sprite had after the previous Update.
		Image *image; ///< The current Image that this Sprite is using.
		float angle; ///< The current direction that this Sprite is pointing (not moving).
		int radarSize; ///< A Rough appoximation of this Sprite's size.
		Color radarColor; ///< The color of this Sprite.
		int interpolationUpdateCheck; // we need two logical loops before interpolated coordinates can be used

    protected:
        bool isPlayerFlag;              ///< Flag for player Sprite, true if the Sprite is an instance of Player class

        bool isPlayer() {
            return isPlayerFlag;
        }
};

#endif // __h_sprite__
