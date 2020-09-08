/**\file			font.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, February 13, 2016
 * \brief
 * \details
 */

#ifndef H_FONT
#define H_FONT

#include "includes.h"
#include "graphics/video.h"
#include "utilities/resource.h"

class Font : public Resource {
		public:
			enum XPos {
				LEFT,   /**< Renders left aligned (default).*/
				CENTER, /**< Renders centered on the point.*/
				RIGHT   /**< Renders right aligned.*/
			};
			enum YPos {
				TOP,    /**< Renders top aligned (default).*/
				MIDDLE, /**< Renders centered on the point.*/
				BOTTOM  /**< Renders bottom aligned.*/
			};

			Font();
			Font( string filename, unsigned int size );
			~Font();

			static Font* Get(string filename, unsigned int size);

			bool Load( string filename, unsigned int size );

			unsigned int GetSize( void );
			void SetColor( Color c, float a=1.0f );
			void SetColor( float r, float g, float b, float a=1.0f );

			int TextWidth(const string& text);
			int LineHeight( void );
			int TightHeight( void );

			int Render( int x, int y, const string& text, XPos xpos = LEFT, YPos ypos = TOP );
			int RenderTight( int x, int y, const string& text, XPos xpos = LEFT, YPos ypos = TOP );

		private:
			int _Render( int x, int y, const string& text, int h, XPos xpos, YPos ypos);

			string fontname; // filename of the loaded font
			float r, g, b, a; // color of text
			unsigned int size;

			TTF_Font* font;
			SDL_Texture* lastRenderedTexture; // cache of last rendered texture
			string lastRenderedText;
			int lastRenderedW, lastRenderedH;
};

#endif // H_FONT
