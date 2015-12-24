/**\file			ui_textbox.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Monday, November 9, 2009
 * \date			Modified: Monday, November 9, 2009
 * \brief
 * \details
 */

#ifndef __H_TEXTBOX__
#define __H_TEXTBOX__

#include "graphics/video.h"
#include "graphics/font.h"
#include "graphics/image.h"
#include "ui/ui.h"

class Textbox : public Widget {
	public:
		Textbox( int x, int y, int w, int rows, string text = "", string label = "");

		void Draw( int relx, int rely = 0 );

		string GetType( void ) {return string("Textbox");}
		virtual int GetMask( void ) { return WIDGET_TEXTBOX; }

		string GetText() { return text; }
		void SetText(string s) { text = s; }

	protected:
		bool KeyPress( SDL_Keycode key );

	private:
		string text;
		int rowPad; ///< The padding around each row of text

		static Font* font;
		static Color foreground;
		static Color background;
		static Color edge;
};

#endif // __H_TEXTBOX__
