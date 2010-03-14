/**\file			ui_slider.h
 * \author			Maoserr
 * \date			Created: Saturday, March 13, 2010
 * \date			Modified: Saturday, March 13, 2010
 * \brief			Creates a slider widget
 */

#ifndef __H_SLIDER__
#define __H_SLIDER__

#include "UI/ui.h"

// Slider bg height
#define SLIDER_H 4
// Marker width
#define SLIDER_MW 4
// Padding above slider to display number
#define SLIDER_VALPAD 2

class Slider : public Widget {
	public:
		Slider( int x, int y, int w, int h, const string& label);
		
		// Utility functions to convert between pixel and values
		int ValToPixel( float value );
		float PixelToVal( int pixels);
		~Slider();

		void Draw( int relx=0, int rely = 0 );

		int GetWidth( void ) { return w; };
		int GetHeight( void ) { return h; };

		void MouseMotion( int x, int y, int dx, int dy );
		void MouseDown( int wx, int wy );
		string GetName( void ) {return string("Slider_"+label); }

	private:
		float minval,maxval,val;
		int w,h;
		string label;
};

#endif // __H_SLIDER__

