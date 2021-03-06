/**\file			video.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Friday, January 1, 2016
 * \brief
 * \details
 */

#ifndef __H_VIDEO__
#define __H_VIDEO__

// Fix for certain OpenGL implementations
#ifndef GL_BGRA
#define GL_BGRA  GL_RGBA
#endif

#include "graphics/color.h"
#include "graphics/image.h"
#include "includes.h"
#include "utilities/coordinate.h"
#include "utilities/lua.h"

#define EPIAR_VIDEO "Video"

class Rect {
	public:
		float x, y, w, h;

		Rect() { x = y = w = h = 0.0f; }
		Rect( float x, float y, float w, float h ) { this->x = x; this->y = y; this->w = w; this->h = h; }
		Rect( int x, int y, int w, int h ) { this->x = TO_FLOAT(x); this->y = TO_FLOAT(y); this->w = TO_FLOAT(w); this->h = TO_FLOAT(h); }
};

class Video {
 	public:
		static bool Initialize( void );
		static bool Shutdown( void );

  		static bool SetWindow( int w, int h, int bpp, bool fullscreen );

  		static void RegisterVideo(lua_State *L);

  		static void Update( void );
  		static void Erase( void );

		static SDL_Renderer* GetRenderer( void ) { return renderer; }

  		static void EnableMouse( void );
  		static void DisableMouse( void );

		static int GetWidth( void );
		static int GetHalfWidth( void );
		static int GetHeight( void );
		static int GetHalfHeight( void );

		static void DrawPoint( int x, int y, float r, float g, float b );
		static void DrawPoint( Coordinate c, Color col );
		static void DrawLine( int x1, int y1, int x2, int y2, Color c, float a = 1.0f);
		static void DrawLine( int x1, int y1, int x2, int y2, float r, float g, float b, float a = 1.0f);
		static void DrawLine( Coordinate p1, Coordinate p2, Color c, float a = 1.0f);
		static void DrawRect( int x, int y, int w, int h, Color c, float a = 1.0f);
		static void DrawRect( int x, int y, int w, int h, float r, float g, float b, float a = 1.0f);
		static void DrawRect( Coordinate p, int w, int h, Color c, float a = 1.0f);
		static void DrawBox( int x, int y, int w, int h, Color c, float a = 1.0f);
		static void DrawBox( int x, int y, int w, int h, float r, float g, float b, float a );
		static void DrawBox( Coordinate p, int w, int h, Color c, float a = 1.0f);
		static void DrawCircle( int x, int y, int radius, float line_width, float r, float g, float b, float a = 1.0f);
		static void DrawCircle( Coordinate c, int radius, float line_width, float r, float g, float b, float a = 1.0f);
		static void DrawCircle( Coordinate c, int radius, float line_width, Color col, float a = 1.0f);
		static void DrawFilledCircle( int x, int y, int radius, Color c, float a = 1.0f);
		static void DrawFilledCircle( int x, int y, int radius, float r, float g, float b, float a = 1.0f);
		static void DrawFilledCircle( Coordinate, int radius, Color c, float a = 1.0f);
		static void DrawTarget( int x, int y, int w, int h, int d, float r, float g, float b, float a = 1.0f );
		static void DrawEllipse( int x, int y, int rx, int ry, float r, float g, float b, float a);

		static void SetCropRect( int x, int y, int w, int h );
		static void UnsetCropRect( void );

		static Image *CaptureScreen( void );
		static void SaveScreenshot( string filename = "" );

		// Lua functions
		static int lua_getWidth(lua_State *L);
		static int lua_getHeight(lua_State *L);

 	private:
  		static int w, h; // width/height of screen
		static int w2, h2; // width/height divided by 2
		static stack<Rect> cropRects;
		static SDL_Window *window;
		static SDL_Renderer *renderer;
};

#endif // __H_VIDEO__
