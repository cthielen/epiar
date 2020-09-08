/**\file			ui_paragraph.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Sunday, June 26, 2011
 * \date			Modified: Saturday, April 2, 2016
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "graphics/video.h"
#include "graphics/font.h"
#include "ui/ui.h"
#include "ui/ui_paragraph.h"
#include "utilities/log.h"

/** \addtogroup UI
 * @{
 */

/**\class Paragraph
 * \brief UI Paragraph
 */

/**\brief Constructor
 */
Paragraph::Paragraph( int x, int y, int maxw, int h, string text ) 
	:lines(UI::font, text, maxw ) {

	this->x = x;
	this->y = y;

	centered = false;

	SetW( lines.GetWidth() );
	SetH( lines.GetHeight() );
}

/**\brief Draw the Paragraph
 */
void Paragraph::Draw(  int relx, int rely ) {
	int x, y;
	
	x = this->x + relx;
	y = this->y + rely;
	
	// draw the text 
	Font::XPos xpositioning = (centered) ? (Font::CENTER) : (Font::LEFT);
	Font::YPos ypositioning = (centered) ? (Font::MIDDLE) : (Font::TOP);

	lines.Render( x, y, xpositioning, ypositioning );

	Widget::Draw(relx, rely + UI::font->TightHeight() / 2 );
}

/**\brief Set the text string of this Widget
 */
void Paragraph::SetText(string text) {
	lines.SetText( text );
	SetW( lines.GetWidth() );
	SetH( lines.GetHeight() );
}

/**\brief Append some text to the current text
 */
void Paragraph::AppendText(string text) {
	lines.AppendText( text );
	SetW( lines.GetWidth() );
	SetH( lines.GetHeight() );
}

/** @} */
