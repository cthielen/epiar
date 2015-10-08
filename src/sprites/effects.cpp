/**\file			effect.cpp
 * \author			Matt Zweig
 * \date			Created: Tuesday, December 15, 2009
 * \date			Modified: Tuesday, December 15, 2009
 * \brief			Sprite SubClass for Animated backgrounds
 * \details
 */

#include "includes.h"
#include "graphics/animation.h"
#include "graphics/image.h"
#include "sprites/spritemanager.h"
#include "sprites/sprite.h"
#include "sprites/effects.h"
#include "engine/scenario_lua.h"

/** \addtogroup Sprites
 * @{
 */

/**\class Effect
 * \brief Various Animation effects.
 */

/**\brief Creates a new Effect at specified coordinate with Animation file
 */
Effect::Effect(Coordinate pos, string filename, float loopPercent) {
	SetWorldPosition(pos);
	visual = new Animation(filename);
	visual->SetLoopPercent( loopPercent );
}

/**\brief Destroy an Effect
 */
Effect::~Effect() {
	delete visual;
}

/**\brief Updates the Effect
 */
void Effect::Update( lua_State *L ) {
	Sprite::Update( L );
	if( visual->Update() == true ) {
		SpriteManager *sprites = Scenario_Lua::GetScenario(L)->GetSpriteManager();
		sprites->Delete( (Sprite*)this );
	}
}

/**\brief Draws the Effect
 */
void Effect::Draw( void ) {
	Coordinate pos = GetScreenPosition();
	visual->Draw( pos.GetX(), pos.GetY(), this->GetAngle());
}

/**\fn Effect::GetDrawOrder( )
 *  \brief Returns the Draw order of the Effect
 */

/** @} */

