/**\file			color.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Thursday, December 24, 2015
 * \date			Modified: Thursday, December 24, 2015
 * \brief
 * \details         Moved out of video.cpp on 12/24/2015
 */

#include "graphics/color.h"

/**\class Color
 * \brief RGB coloring
 * \var Color::r
 *  \brief Red component
 * \var Color::g
 *  \brief Green component
 * \var Color::b
 *  \brief Blue component
 */

/** \brief Default Color Constructor
 *  \details Defaults to White
 */
Color::Color() {
	r = g = b = 1.0f;
}

/** \brief Color Assignment Opererator
 */
Color& Color::operator=(Color other) {
	r = other.r;
	g = other.g;
	b = other.b;

	return *this;
}

/** \brief Color Constructor from Integers
 *  \param[in] red Red percentage as int between 0x00 and 0xFF.
 *  \param[in] green Green percentage as int between 0x00 and 0xFF.
 *  \param[in] blue Blue percentage as int between 0x00 and 0xFF.
 */
Color::Color( int red, int green, int blue ) {
	r = static_cast<float> (red   / 255.0f);
	g = static_cast<float> (green / 255.0f);
	b = static_cast<float> (blue  / 255.0f);
}

/** \brief Color Constructor from Floats
 *  \param[in] red Red percentage as int between 0.0f and 1.0f.
 *  \param[in] green Green percentage as int between 0.0f and 1.0f.
 *  \param[in] blue Blue percentage as int between 0.0f and 1.0f.
 */
Color::Color( float red, float green, float blue ) {
	r = red;
	g = green;
	b = blue;
}

/** \brief Color Constructor from Hex String
 *  \param[in] str A string of the form RRGGBB where RR, GG, and BB are the
 *  hex values of the Red, Green and Blue components of the color.
 *  This can be optionally preceeded by "0x" or "0X".
 *
 *  \example "0xFF0000" is Red
 */
Color::Color( string str ) {
	// string must be in hex format.
	int n;

	stringstream ss;
	ss << std::hex << str;
	ss >> n;

	r = ((n >> 16) & 0xFF ) / 255.0f;
	g = ((n >>  8) & 0xFF ) / 255.0f;
	b = ((n      ) & 0xFF ) / 255.0f;
}

/** \brief Enforce that a float is between 0.0 and 1.0
 *  \todo This should be moved to a math utilities file.
 */
inline float enforce_ratio(float x) {
	if( x < 0.0f )
		return 0.0f;
	if( x > 1.0f )
		return 1.0f;
	return x;
}

/** \brief Color Multiplier
 *	\note Colors can't be darker than black or brighter than white.  This is enforced.
 */
Color Color::operator*(float ratio) {
	Color out = *this;

	out.r *= ratio;
	out.g *= ratio;
	out.b *= ratio;

	out.r = enforce_ratio( out.r );
	out.g = enforce_ratio( out.g );
	out.b = enforce_ratio( out.b );

	return out;
}
