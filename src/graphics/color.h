/**\file			color.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Thursday, December 24, 2015
 * \date			Modified: Thursday, December 24, 2015
 * \brief
 * \details
 */

#ifndef __H_COLOR__
#define __H_COLOR__

#include "includes.h"

#define BLACK     ( Color(0x00,0x00,0x00) )
#define WHITE     ( Color(0xFF,0xFF,0xFF) )
#define RED       ( Color(0xFF,0x00,0x00) )
#define ORANGE    ( Color(0xFF,0x7F,0x00) )
#define YELLOW    ( Color(0xFF,0xFF,0x00) )
#define GREEN     ( Color(0x00,0xFF,0x00) )
#define BLUE      ( Color(0x00,0x00,0xFF) )
#define LIGHTBLUE ( Color(0x00,0x88,0xFF) )
#define PURPLE    ( Color(0x80,0x00,0x80) )
#define GREY      ( Color(0x80,0x80,0x80) )
#define LIGHTGREY ( Color(0xC0,0xC0,0xC0) )
#define DARKGREY  ( Color(0x40,0x40,0x40) )
#define GRAY      GREY
#define GOLD      ( Color(0xFF,0xD7,0x80) )

class Color {
	public:
	float r, g, b;

	Color();
	Color& operator=(Color other);

	Color( int r, int g, int b );
	Color( float r, float g, float b );
	Color( string str );

	Color operator*(float delta);
};

#endif // __H_COLOR__
