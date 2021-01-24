/**\file			starfield.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, January 3, 2016
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "engine/starfield.h"
#include "graphics/video.h"
#include "engine/camera.h"
#include "utilities/timer.h"

/**\class Starfield
 * \brief Controls the starfield. */

/**\struct Starfield::_stars
 * \brief Contains x,y coordinate as well as the color
 */

/**\brief Initializes the starfield.
 * \param num Number of stars to initialize
 */
Starfield::Starfield( int numStars ) {
	int i;

	// seed the random number generator
	srand(static_cast<unsigned int>( time(NULL) ));

	// allocate space for stars
	stars = (struct _star *)malloc( sizeof(struct _star) * numStars );

	// randomly assign position and color
	for( i = 0; i < numStars; i++ ) {
		stars[i].ox = stars[i].x = (float)(rand() % (int)(1.3 * Video::GetWidth()));
		stars[i].oy = stars[i].y = (float)(rand() % (int)(1.4 * Video::GetHeight()));

		// rand() % 225 generates greys between 0 and 225
		stars[i].clr = static_cast<float>( (rand() % 225) / 256. );
	}

	this->numStars = numStars;
}

/**\brief Destroys Starfield
 */
Starfield::~Starfield( void ) {
	assert( stars != NULL );
	free( stars );
	stars = NULL;
}

/**\brief Draws the Starfield
 */
void Starfield::Draw( void ) {
	int i;

	for( i = 0; i < numStars; i++ ) {
		drawStar(stars[i].ox, stars[i].oy, stars[i].x, stars[i].y, stars[i].clr);
	}
}

/**\brief Updates the Starfield
 */
void Starfield::Update( Camera *camera ) {
	int i;
	double dx, dy;
	float w, h;

	assert(camera != NULL);
	camera->GetDelta( &dx, &dy );

	w = static_cast<float>(1.3 * Video::GetWidth());
	h = static_cast<float>(1.4 * Video::GetHeight());

	for( i = 0; i < numStars; i++ ) {
		stars[i].ox = stars[i].x;
		stars[i].oy = stars[i].y;

		stars[i].x -= (float)dx * stars[i].clr;
		stars[i].y += (float)dy * stars[i].clr;

		// handle wrapping the stars around if they go offscreen top/left
		while( stars[i].x < 0.0f ) {
			stars[i].ox += w;
			stars[i].x += w;
		}
		while( stars[i].y < 0.0f ) {
			stars[i].oy += h;
			stars[i].y += h;
		}

		// handle wrapping the stars around if they go offscreen bottom/right
		while( stars[i].x > w ) {
			stars[i].ox -= w;
			stars[i].x -= w;
		}
		while( stars[i].y > h ) {
			stars[i].oy -= h;
			stars[i].y -= h;
		}
	}
}

/**\brief Draws the stars
 */
inline void Starfield::drawStar(float ox, float oy, float x, float y, float  brightness) {
	float drawBrightness;
	float xcomponent;
	float ycomponent;
	float ix, iy;
	double fframe = Timer::GetFFrame();

	if(interpolateOn) {
		ix = ox * (1.0f - fframe) + x * fframe;
		iy = oy * (1.0f - fframe) + y * fframe;
	} else {
		ix = x;
		iy = y;
	}

	// Loop between both X columns
	for (int xcnt = 0; xcnt <= 1; xcnt++) {
		// Check for clipping in the X direction (make sure it fits horizontally)
		if (((int)ix - xcnt)>= 0 && ((int)iy - xcnt) < Video::GetWidth()) {
			// Loop between both Y rows
			for (int ycnt = 0; ycnt <= 1; ycnt++) {
				// Check for clipping in the Y direction (make sure it fits vertically)
				if ((int)iy - ycnt >= 0 && (int)iy - ycnt < Video::GetHeight()) {
					// Get the x component of the brightness and divide it by 2
					xcomponent = (((float)(xcnt - (ix - (int)ix)) * .5f));

					// Get the y component of the brightness and divide it by 2
					ycomponent = (((float)(ycnt - (iy - (int)iy)) * .5f));

					if(ycomponent < 0) ycomponent = ycomponent * -1;
					if(xcomponent < 0) xcomponent = xcomponent * -1;

					// Add the x anid y components of the brightness to get the total brightness at that particular location
					drawBrightness = ((xcomponent + ycomponent) * brightness);

					Video::DrawPoint((int)ix - xcnt, (int)iy - ycnt, drawBrightness, drawBrightness, drawBrightness);
				}
			}
		}
	}
}
