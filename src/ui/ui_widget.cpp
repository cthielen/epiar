/**\file			ui_widget.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, December 27, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "ui.h"
#include "utilities/log.h"
#include "graphics/video.h"

/** \addtogroup UI
 * @{
 */

/**\class Widget
 * \brief A user interface widget, widgets do not have children.
 * \todo actions could be an array of lists of Actions.
 *       This would allow multiple Actions to be registered to the same Event.
 * \fn Widget::Draw()
 *  \brief Empty function that should be overloaded for drawing the widget.
 * \fn Widget::Update()
 *  \brief Empty function that should be overloaded for drawing the widget.
 */

/**\brief Constructor.
 */
Widget::Widget( void ):
	name( "UnspecifiedWidget" ),
	hovering( false ),
	hidden( false ),
	disabled( false ),
	keyactivated( false ),
	x( 0 ), y( 0 ),
	w( 0 ), h( 0 ),
	dragX( 0 ), dragY( 0 ),
	parent( NULL )
{
	for( int i = 0; i < (int)Action_Last; i++ ){
		actions[i] = NULL;
	}
}

/**\brief Destructor.
 */
Widget::~Widget( void )
{
	Activate(Action_Close, 0, 0);

	// Delete all alocated Actions
	for( int i = 0; i < (int)Action_Last; i++ ){
		if( actions[i] == NULL ) {
			delete actions[i];
		}
	}
}

/**\brief Draw
 * \details This does nothing in most cases, but will draw some extra debug
 * information when the "debug-ui" option is enabled.
 */
void Widget::Draw( int relx, int rely ) {
	if( hovering && OPTION(int,"options/development/debug-ui") ) {
		int absx, absy;
		char xbuff[6];
		char ybuff[6];
		absx = x + relx;
		absy = y + rely;
		Video::DrawBox( absx, absy, w, h, GREEN, 1 );
		Video::DrawLine( relx, absy, absx, absy, RED, 1 );
		Video::DrawLine( absx, rely, absx, absy, RED, 1 );

		snprintf( xbuff, sizeof(xbuff), "%d", x );
		snprintf( ybuff, sizeof(xbuff), "%d", y );

		Video::DrawRect( relx + x/2, y, UI::font->TextWidth(xbuff), UI::font->LineHeight(), BLACK, 1 );
		UI::font->RenderTight( relx + x/2, y, xbuff );

		Video::DrawRect( absx, rely + y/2, UI::font->TextWidth(ybuff), UI::font->LineHeight(), BLACK, 1 );
		UI::font->RenderTight( absx, rely + y/2, ybuff );
	}
}

/**\brief Tests if point is within a rectangle.
 */
int Widget::GetAbsX( void ) {
	int absx = GetX();
	Widget* theParent = parent;
	while( theParent != NULL ) {
		assert( theParent->GetMask() & WIDGET_CONTAINER );
		absx += theParent->GetX();
		theParent = theParent->parent;
	}
	return absx;
}

/**\brief Tests if point is within a rectangle.
 */
int Widget::GetAbsY( void ) {
	int absy = GetY();
	Widget* theParent = parent;
	while( theParent != NULL ) {
		assert( theParent->GetMask() & WIDGET_CONTAINER );
		absy += theParent->GetY();
		theParent = theParent->parent;
	}
	return absy;
}

/**\brief Tests if point is within a rectangle.
 */
bool Widget::Contains(int relx, int rely) {
	return WITHIN_BOUNDS(relx, rely, x, y, w, h);
}

/**\brief Register an Action to an action_type
 */
Widget* Widget::RegisterAction( action_type type, Action* action )
{
	assert( action != NULL );
	assert( type < Action_Last );
	if( actions[type] != NULL ) {
		delete actions[type];
	}
	actions[type] = action;

	return this;
}

/**\brief Activate an Action that has been registered to this Widget
 */
bool Widget::Activate( action_type type, int xi, int yi ) {
	assert( type < Action_Last );

	if( actions[type] != NULL ) {
		// There is a registered Action of this type, so call it
		(actions[type])->Activate( xi-x, yi-y );
		return true;
	}

	// There is no registered Action of this type
	return false;
}

/**\brief Widget is currently being dragged.
 */
bool Widget::MouseDrag( int xi,int yi ){
	Activate(Action_MouseDrag, xi, yi);
	return true;
}

/**\brief Mouse is currently moving over the widget, without button down.
 */
bool Widget::MouseMotion( int xi, int yi ){
	Activate(Action_MouseMotion, xi, yi);
	return true;
}

/**\brief Event is triggered on mouse enter.
 */
bool Widget::MouseEnter( int xi, int yi ){
	LogMsg(TRACE, "Mouse enter detect in %s named %s.", GetType().c_str(), GetName().c_str() );

	hovering = true;

	Activate(Action_MouseEnter, xi, yi);

	return true;
}

/**\brief Event is triggered on mouse leave.
 */
bool Widget::MouseLeave( void ){
	LogMsg(TRACE, "Mouse leave detect in %s named %s.", GetType().c_str(), GetName().c_str() );

	hovering = false;

	Activate(Action_MouseLeave, 0, 0);

	return true;
}

/**\brief Generic mouse up function.
 */
bool Widget::MouseLUp( int xi, int yi ){
	LogMsg(TRACE, "Mouse Left up detect in %s named %s.", GetType().c_str(), GetName().c_str() );

	Activate(Action_MouseLUp, xi, yi);

	return true;
}

/**\brief Generic mouse down function.
 */
bool Widget::MouseLDown( int xi, int yi ) {
	LogMsg(TRACE, "Mouse Left up detect in %s named %s.", GetType().c_str(), GetName().c_str() );

	// update drag coordinates in case this is draggable
	dragX = xi-x;
	dragY = yi-y;

	Activate(Action_MouseLDown, xi, yi);

	return true;
}

/**\brief Generic mouse release function.
 */
bool Widget::MouseLRelease( void ){
	LogMsg(TRACE, "Left Mouse released in %s named %s.", GetType().c_str(), GetName().c_str() );

	Activate(Action_MouseLRelease, 0, 0);

	return true;
}

/**\brief Generic middle mouse up function.
 */
bool Widget::MouseMUp( int xi, int yi ){
	LogMsg(TRACE, "Mouse Middle up detect in %s named %s.", GetType().c_str(), GetName().c_str() );

	Activate(Action_MouseMUp, xi, yi);

	return true;
}

/**\brief Generic middle mouse down function.
 */
bool Widget::MouseMDown( int xi, int yi ){
	LogMsg(TRACE, "Mouse Middle down detect in %s named %s.", GetType().c_str(), GetName().c_str() );

	Activate(Action_MouseMDown, xi, yi);

	return true;
}

/**\brief Generic middle mouse release function.
 */
bool Widget::MouseMRelease( void ){
	LogMsg(TRACE, "Middle Mouse released in %s named %s.", GetType().c_str(), GetName().c_str() );

	Activate(Action_MouseMRelease, 0, 0);

	return true;
}

/**\brief Generic right mouse up function.
 */
bool Widget::MouseRUp( int xi, int yi ){
	LogMsg(TRACE, "Mouse Right up detect in %s named %s.", GetType().c_str(), GetName().c_str() );

	Activate(Action_MouseRUp, xi, yi);

	return true;
}

/**\brief Generic right mouse down function.
 */
bool Widget::MouseRDown( int xi, int yi ){
	LogMsg(TRACE, "Mouse Right down detect in %s named %s.", GetType().c_str(), GetName().c_str() );

	Activate(Action_MouseRDown, xi, yi);

	return true;
}

/**\brief Generic right mouse release function.
 */
bool Widget::MouseRRelease( void ){
	LogMsg(TRACE, "Right Mouse released in %s named %s.", GetType().c_str(), GetName().c_str() );

	Activate(Action_MouseRRelease, 0, 0);

	return true;
}

/**\brief Generic mouse wheel up function.
 */
bool Widget::MouseWUp( int xi, int yi ){
	LogMsg(TRACE, "Mouse Wheel up detect in %s named %s.", GetType().c_str(), GetName().c_str() );

	Activate(Action_MouseWUp, xi, yi);

	return false;
}

/**\brief Generic mouse wheel down function.
 */
bool Widget::MouseWDown( int xi, int yi ){
	LogMsg(TRACE, "Mouse Wheel down detect in %s named %s.", GetType().c_str(), GetName().c_str() );

	Activate(Action_MouseWDown, xi, yi);

	return false;
}

/**\brief Generic keyboard focus function.
 */
bool Widget::KeyboardEnter( void ){
	LogMsg(TRACE, "Keyboard enter detect in %s named %s.", GetType().c_str(), GetName().c_str() );

	Activate(Action_KeyboardEnter, 0, 0);

	keyactivated = true;

	return true;
}

/**\brief Generic keyboard unfocus function.
 */
bool Widget::KeyboardLeave( void ){
	LogMsg(TRACE, "Keyboard leave detect in %s named %s.", GetType().c_str(), GetName().c_str() );

	Activate(Action_KeyboardLeave, 0, 0);

	keyactivated = false;

	return true;
}

/**\brief Generic keyboard key press function.
 */
bool Widget::KeyPress( SDL_Keycode key ) {
	LogMsg(TRACE, "Key press detect in %s named %s.", GetType().c_str(), GetName().c_str() );

	return false;
}

/**\brief XML Node of a Widget.
 * \details Expandable by the Widget subclasses by simply modifying the Node.
 * \sa UI::Save
 */
xmlNodePtr Widget::ToNode() {
	xmlNodePtr thisNode;
	char buff[256] = {0};

	thisNode = xmlNewNode(NULL, BAD_CAST GetType().c_str() );

	xmlSetProp( thisNode, BAD_CAST "name", BAD_CAST GetName().c_str() );
	snprintf(buff, sizeof(buff), "%d", GetX() );
	xmlSetProp( thisNode, BAD_CAST "x", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", GetY() );
	xmlSetProp( thisNode, BAD_CAST "y", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", GetW() );
	xmlSetProp( thisNode, BAD_CAST "w", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", GetH() );
	xmlSetProp( thisNode, BAD_CAST "h", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%p", this );
	xmlSetProp( thisNode, BAD_CAST "address", BAD_CAST buff );

	if(hidden) xmlSetProp( thisNode, BAD_CAST "hidden", BAD_CAST "true" );
	if(disabled) xmlSetProp( thisNode, BAD_CAST "disabled", BAD_CAST "true" );

	return thisNode;
}

/** @} */
