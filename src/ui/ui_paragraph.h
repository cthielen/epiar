/**\file			ui_paragraph.h
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Friday, April 25, 2008
 * \date			Modified: Friday, April 25, 2008
 * \brief
 * \details
 */

#ifndef __H_PARAGRAPH__
#define __H_PARAGRAPH__

#include "ui/ui.h"
#include "ui/ui_text.h"

#define UI_PARAGRAPH_BOTTOM_PADDING 15

class Paragraph : public Widget {
	public:
		Paragraph( int x, int y, int w, int h, string text );
		~Paragraph() { };
		
		void Draw(  int relx = 0, int rely = 0 );

		void SetText(string text);
		void AppendText(string text);
		string GetText(){return name;}
	
		string GetType( void ) {return string("Paragraph");}
		virtual int GetMask( void ) { return WIDGET_PARAGRAPH; }

		void SetH( int _h ){ h = _h + UI_PARAGRAPH_BOTTOM_PADDING; }
	
	private:
		bool centered;
		Text lines;
};

#endif // __H_PARAGRAPH__
