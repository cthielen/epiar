/**\file			starfield.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, January 3, 2016
 * \brief
 * \details
 */

#include "engine/camera.h"

#ifndef __h_starfield__
#define __h_starfield__

class Starfield {
	public:
		Starfield( int numStars );
		~Starfield( void );

		void Draw( void );
		void Update( Camera *camera );

	private:
		inline void drawStar( float ox, float oy, float x, float y, float brightness);

		struct _star {
			float x, y, ox, oy;
			float clr;
		} *stars;

		int numStars; // number of stars
};

#endif // __h_starfield__
