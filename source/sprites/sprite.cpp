/**\file			sprite.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "engine/camera.h"
#include "sprites/sprite.h"
#include "utilities/log.h"
#include "utilities/timer.h"

/** \addtogroup Sprites
 * @{
 */

// Sprite ID 0 is only used as a NULL
long int Sprite::sprite_ids = 1;

/**\class Sprite
 * \brief Supertype for all drawable objects existing at a point in the universe with an angle and momentum.
 * \details Sprites are the objects that move around the universe.
 *          They may be created and destroyed.
 *          Each Sprite has a Unique ID.
 *
 *          Only the SpriteManager should ever store pointers to Sprite
 *          objects.  This is because only the SpriteManager is informed when a
 *          Sprite is removed, so a Sprite object may or may not be valid from
 *          one point to the next.
 *
 *          Sprites are drawn based on their Draw Order and their id.  This
 *          means that all Planets are drawn below all ships, which are drawn
 *          below all Effects.  The Draw Order should also be used to detect
 *          the kind of Sprite given just a Sprite pointer.
 *
 *          Sprites share Image objects to save on memory usage.
 *
 * \TODO Move function implementations to the .cpp file.
 * \warn NEVER STORE SPRITE POINTERS (unless you are the SpriteManager)!
 *       Instead store the sprite's unique ID and query the SpriteManager for
 *       the Sprite object every time it is referenced.  This prevents memory
 *       corruption by accessing Sprites that have been deleted.
 * \sa SpriteManager
 */


/**\brief Default Sprite Constructor
 * \details Gets a unique ID.
 *          Sets the radarColor as Grey.
 */
Sprite::Sprite() {
	id = sprite_ids++;

	// Momentum caps

	angle = 0.;

	image = NULL;

	radarSize = 1;
	radarColor = WHITE * 0.7f;

  interpolationUpdateCheck = 0;
}

Coordinate Sprite::GetWorldPosition( void ) const {
  return worldPosition;
}

void Sprite::SetWorldPosition( Coordinate coord ) {
	worldPosition = coord;
}

Coordinate Sprite::GetScreenPosition( void ) const {
	return screenPosition;
}

/**\brief Move this Sprite in the direction of their current momentum.
 * \details Since this is a space simulation, there is no Friction; momentum does not decrease over time.
 */
void Sprite::Update( lua_State *L ) {
	worldPosition += momentum;
	acceleration = lastMomentum - momentum;
	lastMomentum = momentum;
}

void Sprite::UpdateScreenCoordinates( void ) {
  Camera *camera = Camera::Instance();

  oldScreenPosition = screenPosition;
  camera->TranslateWorldToScreen( worldPosition, screenPosition );

  if(interpolationUpdateCheck < 2) interpolationUpdateCheck++;
}

/**\brief Draw
 * \details The Sprite is drawn centered on wx,wy.
 *          This will attempt to Draw the sprite even if wx,wy are completely off the Screen.
 *          Avoid drawing sprites that are too far off the Screen.
 * \sa SpriteManager::Draw
 */
void Sprite::Draw( void ) {
  double fframe = Timer::GetFFrame();

	if( image ) {
    if(interpolateOn && (interpolationUpdateCheck >= 2)) {
      Coordinate interpolatedScreenPosition;

      interpolatedScreenPosition.SetX(oldScreenPosition.GetX() * (1.0f - fframe) + screenPosition.GetX() * fframe);
      interpolatedScreenPosition.SetY(oldScreenPosition.GetY() * (1.0f - fframe) + screenPosition.GetY() * fframe);

      image->DrawCentered( interpolatedScreenPosition.GetX(), interpolatedScreenPosition.GetY(), angle );
    } else {
      image->DrawCentered( screenPosition.GetX(), screenPosition.GetY(), angle );
    }
	} else {
		LogMsg(WARN, "Attempt to draw a sprite before an image was assigned." );
	}
}

/** @} */
