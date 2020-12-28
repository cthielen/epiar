/**\file			ui_button.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Friday, April 25, 2008
 * \date			Modified: Friday, April 25, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "graphics/font.h"
#include "graphics/video.h"
#include "ui/ui.h"
#include "ui/ui_button.h"
#include "utilities/log.h"
#include "utilities/lua.h"

/** \addtogroup UI
 * @{
 */

/**\class Button
 * \brief UI button. */

/**\brief Convenience function to initialize the button, used to prevent code repetition.
 */
void Button::Initialize( int x, int y, int w, int h, string label ) {
	// This is the main Button Constructor
	// This cuts down on code duplication so it can be called by multiple constructors.
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	
	this->name = label;
	
	// Load the bitmaps needed for drawing
	bitmap_normal = Image::Get( "data/skin/ui_button.png" );
	bitmap_mouseover = Image::Get( "data/skin/ui_button_mouseover.png" );
	bitmap_pressed = Image::Get( "data/skin/ui_button_pressed.png" );

	bitmap_current = bitmap_normal;

	assert(bitmap_normal);
	assert(bitmap_mouseover);
	assert(bitmap_pressed);
	assert(bitmap_current);
}

/**\brief Constructs a button with a C++ callback.*/
Button::Button( int x, int y, int w, int h, string label, void (*function)(void*), void* value) {
	Initialize( x, y, w, h, label );
	RegisterAction( Action_MouseLUp, new ObjectAction(function, value) );
}

/**\brief Constructs a button with a C++ callback.*/
Button::Button( int x, int y, int w, int h, string label, void (*function)()) {
	Initialize( x, y, w, h, label );
	RegisterAction( Action_MouseLUp, new VoidAction(function) );
}

/**\brief Constructs a button with a Lua callback.*/
Button::Button( int x, int y, int w, int h, string label, string lua_code) {
	Initialize( x, y, w, h, label );
	RegisterAction( Action_MouseLUp, new LuaAction(lua_code) );
}

Button::~Button() {
//	DO NOT DO THIS - it's just a pointer, never allocated - delete bitmap_current;
	//delete bitmap_normal;
	//delete bitmap_pressed;

	bitmap_normal = bitmap_pressed = NULL;
}

/**\brief Draws the button.*/
void Button::Draw( int relx, int rely ) {
	int x, y;
	
	x = this->x + relx;
	y = this->y + rely;
	
	Video::DrawRect( x, y, this->w, this->h, WHITE );

	assert(bitmap_current);

	// Draw the button (loaded image is simply scaled)
	bitmap_current->DrawStretch( x, y, this->w, this->h );

	// Draw the label
	Video::SetCropRect(x + 1, y + 1, this->w - 2, this->h - 2); // constants adjust for the 1px border
	UI::font->RenderTight( x + (w / 2), y + (h / 2), this->name, Font::CENTER, Font::MIDDLE );
	Video::UnsetCropRect();

	Widget::Draw(relx, rely);
}

/**\brief When Left mouse is down on the button.*/
bool Button::MouseLDown( int xi, int yi ) {
	Widget::MouseLDown( xi, yi );

	if(OPTION(int, "options/sound/buttons")) { UI::beep->Play(); }

	bitmap_current = bitmap_pressed;

	return true;
}

/**\brief When left mouse is back up on the button.*/
bool Button::MouseLUp( int xi, int yi ) {
	bitmap_current = bitmap_mouseover;

	Widget::MouseLUp( xi, yi );

	return true;
}

bool Button::MouseLRelease( void ){
	Widget::MouseLRelease();
	bitmap_current = bitmap_normal;
	return true;
}

/**\brief Event is triggered on mouse enter.
 */
bool Button::MouseEnter( int xi, int yi ){
	Widget::MouseEnter( xi, yi );

	bitmap_current = bitmap_mouseover;
	hovering = true;

	LogMsg(TRACE, "Mouse enter detect in %s named %s.", GetType().c_str(), GetName().c_str() );

	return true;
}

/**\brief Event is triggered on mouse leave.
 */
bool Button::MouseLeave( void ){
	Widget::MouseLeave();

	bitmap_current = bitmap_normal;
	hovering = false;

	LogMsg(TRACE, "Mouse leave detect in %s named %s.", GetType().c_str(), GetName().c_str() );

	return true;
}

/** @} */
