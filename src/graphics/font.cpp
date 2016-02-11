/**\file			font.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, January 3, 2016
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include <FTGL/ftgl.h>
#include "graphics/font.h"
#include "graphics/video.h"
#include "utilities/log.h"
#include "utilities/file.h"

/**\class Font
 * \brief Font class takes care of initializing fonts. */

/**\brief Constructs new font (default color white).
 */
Font::Font():r(1.f),g(1.f),b(1.f),a(1.f),font(NULL),lastRenderedTexture(NULL) {}

/**\brief Construct new font based on file.
 * \param filename String containing file.
 */
Font::Font( string filename ):r(1.f),g(1.f),b(1.f),a(1.f),font(NULL),lastRenderedTexture(NULL) {
	bool success;
	success = Load( filename );
	assert( success );
}

/**\brief Lazy fetch an Font
 */
Font* Font::Get( string filename ) {
	Font* value = NULL;

	value = static_cast<Font*>(Resource::Get(filename));

	if( value == NULL ) {
		value = new Font();

		if(value->Load(filename)){
			Resource::Store(filename,(Resource*)value);
		} else {
			LogMsg(WARN, "Couldn't Find Font '%s'", filename.c_str());
			delete value;
			return NULL;
		}
	}

	return value;
}

/**\brief Destroys the font.*/
Font::~Font() {
	TTF_CloseFont(font);
	font = NULL;
	if(lastRenderedTexture) { SDL_DestroyTexture(lastRenderedTexture); }
	LogMsg(DEBUG, "Font '%s' freed.", fontname.c_str() );
}

/**\brief Sets the new color and alpha value.
 */
void Font::SetColor( Color c, float a ) {
	this->r = c.r;
	this->g = c.g;
	this->b = c.b;
	this->a = a;
}

/**\brief Sets the new color and alpha value.
 */
void Font::SetColor( float r, float g, float b, float a ) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

/**\brief Loads the font (uses FTGL Texture Fonts).
 * \param filename Path to font file.
 */
bool Font::Load( string filename ) {
	File fontFile;

	if( fontFile.OpenRead( filename.c_str() ) == false) {
		LogMsg(ERR, "Font '%s' could not be loaded.", fontname.c_str() );
		return( false );
	}

	if( this->font != NULL) {
		LogMsg(ERR, "Deleting the old font '%s'.\n", fontname.c_str() );
		TTF_CloseFont(font);
		font = NULL;
	}

	fontname = fontFile.GetAbsolutePath();
	font = TTF_OpenFont( fontname.c_str(), 12 );

	if( font == NULL ) {
		LogMsg(ERR, "Failed to load font '%s'.\n", fontname.c_str() );
		return( false );
	}

	LogMsg(DEBUG, "Font '%s' loaded.\n", fontname.c_str() );

	return( true );
}

/**\brief Set's the size of the font (default is 12).*/
void Font::SetSize( int size ){
	cout << "Font::SetSize() needs to be worked around" << endl;
	//this->font->FaceSize( size );
}

/**\brief Retrieves the size of the font.*/
unsigned int Font::GetSize( void ){
	cout << "Font::GetSize() needs to be worked around" << endl;
	return 12;
	//return this->font->FaceSize();
}

/**\brief Returns the width of the text (no padding).*/
int Font::TextWidth( const string& text ) {
	int w, h;

	TTF_SizeText(font, text.c_str(), &w, &h);

	return w;
}

/**\brief Returns the recommended line height of the font.
 * \details
 * It is recommended that you use the line height for rendering lines
 * of text as it inserts a natural padding between lines.
 */
int Font::LineHeight( void ){
	return TTF_FontLineSkip(font);
}

/**\brief Returns the complete height of the font, including Ascend and Descend.
 * \details
 * Use Outer height to get the tight fitting height of the font.
 */
int Font::TightHeight( void ){
	float asc = TTF_FontAscent(font);
	float desc = TTF_FontDescent(font);
	int height = TO_INT(ceil(asc - desc));

	return height;
}

/**\brief Renders a string with natural padding.
 * \return The consumed width (This includes a small bit of padding on the right)
 */
int Font::Render( int x, int y, const string& text, XPos xpos, YPos ypos ){
	//int h = this->LineHeight();
	int h = TTF_FontAscent(font);

	return this->_Render(x, y, text, h, xpos, ypos);
}

/**\brief Renders a string with no padding.
 * \return The consumed width (This includes a small bit of padding on the right)
 */
int Font::RenderTight(int x, int y, const string& text, XPos xpos, YPos ypos ){
	int h = this->TightHeight();

	return this->_Render(x, y, text, h, xpos, ypos);
}

/**\brief Internal rendering function. Returns the consumed width. */
int Font::_Render( int x, int y, const string& text, int h, XPos xpos, YPos ypos) {
	int xn = 0;
	int yn = 0;

	if(text.length() == 0) {
		return 0; // Okay mister!
	}

	switch( xpos ) {
		case LEFT:
			xn = x;
			break;
		case CENTER:
			xn = x - this->TextWidth(text) / 2;
			break;
		case RIGHT:
			xn = x - this->TextWidth(text);
			break;
		default:
			LogMsg(ERR, "Invalid xpos");
			assert(0);
	}
	
	//cout << "---" << endl;

	// Y coordinates are flipped
	switch( ypos ) {
		case TOP:
			// Top of text will touch the given 'y'
			//cout << "render top (y, h, descent) = (" << y << ", " << h << ", " << TTF_FontDescent(this->font) << ")" << endl;
			yn = y; // - TO_INT(floor(TTF_FontDescent(this->font)));
			break;
		case MIDDLE:
			// Middle of text line will run through the given 'y'
			//cout << "render middle (" << y << ")" << endl;
			//cout << "descent is" << TO_INT(floor(TTF_FontDescent(this->font))) << endl;
			yn = y - (h / 2) + TO_INT(floor(TTF_FontDescent(this->font)));
			break;
		case BOTTOM:
			// Bottom of text will touch the given 'y'
			//cout << "render bottom" << endl;
			yn = y - h + TO_INT(floor(TTF_FontDescent(this->font)));
			break;
		default:
			LogMsg(ERR, "Invalid ypos");
			assert(0);
	}

	int ret_w = -1;
	SDL_Rect rect;
	SDL_Texture *t = NULL;

	if(lastRenderedText == text) {
		assert(lastRenderedTexture != NULL);

		rect.x = xn;
		rect.y = yn;
		rect.w = lastRenderedW;
		rect.h = lastRenderedH;

		t = lastRenderedTexture;

		ret_w = rect.w;
	} else {
		SDL_Surface *s = NULL;
		SDL_Color fg;

		fg.r = r * 255.;
		fg.g = g * 255.;
		fg.b = b * 255.;
		fg.a = a * 255.;

		s = TTF_RenderUTF8_Blended(font, text.c_str(), fg);
		if(s == NULL) {
			cout << "Could not render '" << text << "'!" << endl;

			if(text.length() == 0) {
				cout << "Why is text null?" << endl;
			}

			return 0;
		}

		rect.x = xn;
		rect.y = yn;
		rect.w = s->w;
		rect.h = s->h;

		t = SDL_CreateTextureFromSurface(Video::GetRenderer(), s);

		ret_w = s->w;
		lastRenderedW = s->w;
		lastRenderedH = s->h;

		SDL_FreeSurface(s);

		if(lastRenderedTexture) { SDL_DestroyTexture(lastRenderedTexture); }
		lastRenderedTexture = t;
		lastRenderedText = text;
	}

	//Video::DrawRect(x, y, rect.w, rect.h, 128, 128, 128, 1.0);
	SDL_RenderCopy(Video::GetRenderer(), t, NULL, &rect);

	//cout << "rendered '" << text << "' at " << xn << ", " << yn << " with color " << r << ", " << g << ", " << b << ", " << a << endl;

	return ret_w;
}

