/**\file			ui_widget.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, December 27, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "ui.h"

/**\class Widget
 * \brief Widgets. */

bool Widget::AddChild( Widget *widget ) {
	if( !widget) return false;
	
	children.push_back( widget );
	
	return true;
}

void Widget::Draw( int relx, int rely ) {
	// Draw any children
	list<Widget *>::iterator i;
	
	for( i = children.begin(); i != children.end(); ++i ) {
		(*i)->Draw( x + relx, y + rely );
	}
}

void Widget::Update( void ) {

}

bool Widget::Contains(int relx, int rely) {
	bool insideLeftBorder = ( x < relx );
	bool insideRightBorder = ( relx < x + this->GetWidth() );
	bool insideTopBorder = 	( y < rely );  // (0,0) is the upper left, not lower left
	bool insideBottomBorder = ( rely < y + this->GetHeight() );
	
	return insideLeftBorder && insideRightBorder && insideTopBorder && insideBottomBorder;
}

// returns a widget if there is a child widget of this widget that was clicked on
Widget *Widget::DetermineMouseFocus( int relx, int rely ) {
	list<Widget *>::iterator i;

	for( i = children.begin(); i != children.end(); ++i ) {
		if( (*i)->Contains(relx, rely) ) {
			return (*i);
		}
	}

	return( NULL );
}

void Widget::FocusMouse( int x, int y ) {
	// Relative coordinate - to current widget
	int xr = x - GetX();
	int yr = y - GetY();

	// update drag coordinates in case this is draggable
	dragX = xr;
	dragY = yr;

	Widget *mouseFocus = DetermineMouseFocus( xr, yr );

	if( mouseFocus )
		mouseFocus->FocusMouse( xr, yr );

	if( keyboardFocus ) keyboardFocus->UnfocusKeyboard();
	keyboardFocus = mouseFocus;
	if( keyboardFocus ) keyboardFocus->FocusKeyboard();
}

void Widget::MouseDown( int x, int y ) {
	// Relative coordinate - to current widget
	int xr = x - GetX();
	int yr = y - GetY();

	Widget *down_on = DetermineMouseFocus( xr, yr );
	if( down_on ) down_on->MouseDown( xr,yr );
}

void Widget::MouseMotion( int x, int y, int dx, int dy ){
	// Relative coordinate - to current widget
	int xr = x - GetX();
	int yr = y - GetY();

	Widget *motion = DetermineMouseFocus( xr, yr );
	if( motion ) motion->MouseMotion( xr,yr,dx,dy);
}

// when a widget loses focus, so do all of its children
void Widget::UnfocusMouse( void ) {
	list<Widget *>::iterator i;

	for( i = children.begin(); i != children.end(); ++i ) {
		(*i)->UnfocusMouse();
	}
}

void Widget::UnfocusKeyboard( void ) {
	if( keyboardFocus ) {
		keyboardFocus->UnfocusKeyboard();
		keyboardFocus = NULL;
	}
}

bool Widget::KeyPress( SDLKey key ) {
	if( keyboardFocus ) return keyboardFocus->KeyPress( key );
	
	return false;
}
