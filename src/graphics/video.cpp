/**\file			video.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Wednesday, July 15, 2020
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "graphics/video.h"
#include "utilities/file.h"
#include "utilities/log.h"
#include "utilities/xmlfile.h"
#include "utilities/trig.h"

/**\class Rect
 * \brief Wrapper for a rectangle.
 * \fn Rect::Rect()
 *  \brief Creates a rectangle with 0 for x,y, width, and height.
 * \fn Rect::Rect( float x, float y, float w, float h )
 *  \brief Creates a rectangle with specified dimensions.
 * \fn Rect::Rect( int x, int y, int w, int h )
 *  \brief Creates a rectangle with specified dimensions. (float version)
 * \var Rect::x
 *  \brief x coordinate
 */
/**\var Rect::y
 *  \brief y coordinate
 */
/**\var Rect::w
 *  \brief width
 */
/**\var Rect::h
 *  \brief height
 */

/**\class Video
 * \brief Video handling. */

int Video::w = 0;
int Video::h = 0;
int Video::w2 = 0;
int Video::h2 = 0;
stack<Rect> Video::cropRects;
SDL_Window *Video::window = NULL;
SDL_Renderer *Video::renderer = NULL;

/**\brief Initializes the Video display.
 */
bool Video::Initialize( void ) {
	// Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) != 0 ) {
		LogMsg(ERR, "Could not initialize SDL: %s", SDL_GetError() );
		return( false );
	} else {
		LogMsg(DEBUG, "SDL video initialized using FIXME driver."); //, SDL_VideoDriverName( buf, 31 ) );
	}

	atexit( SDL_Quit );

	int w = OPTION( int, "options/video/w" );
	int h = OPTION( int, "options/video/h" );
	bool fullscreen = OPTION( bool, "options/video/fullscreen" );

	LogMsg(DEBUG, "Creating window of size (%d, %d)\n", w, h);

	Video::SetWindow( w, h, OPTION( int, "options/video/bpp"), fullscreen );

	/* Initialize SDL_image */
	if( IMG_Init( IMG_INIT_PNG ) != IMG_INIT_PNG ) {
		LogMsg(ERR, "Could not initialize SDL_image: %s", SDL_GetError() );
		return( false );
	}

	atexit( IMG_Quit );


	/* Initialize SDL_ttf */
	if( TTF_Init( ) < 0 ) {
		LogMsg(ERR, "Could not initialize SDL_ttf: %s", SDL_GetError() );
		return( false );
	}

	atexit( TTF_Quit );

	return( true );
}

/**\brief Shuts down the Video display.
 */
bool Video::Shutdown( void ) {
	EnableMouse();

	SDL_DestroyRenderer( renderer ); renderer = NULL;
	SDL_DestroyWindow( window ); window = NULL;

	return( true );
}

/**\brief Sets the window properties.
 */
bool Video::SetWindow( int w, int h, int bpp, bool fullscreen ) {
	// Set the application icon (must be done before SDL_SetVideoMode)
	SDL_Surface *icon = NULL;

	File icon_file( "data/graphics/icon.bmp" ); // File is used to calculate path

	icon = SDL_LoadBMP( icon_file.GetAbsolutePath().c_str() );

	if(icon != NULL) {
		SDL_SetWindowIcon(window, icon);
		SDL_FreeSurface(icon); icon = NULL;
	} else {
		LogMsg(WARN, "Unable to load window icon data/graphics/icon.bmp." );
	}

	window = SDL_CreateWindow("Epiar", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
	if(window == NULL) {
		LogMsg(ERR, "Could not create window: %s", SDL_GetError() );
		return( false );
	}

	if(fullscreen) {
		if( SDL_SetWindowFullscreen( window, SDL_WINDOW_FULLSCREEN ) < 0 ) {
			LogMsg(ERR, "Window created but unable to set fullscreen: %s", SDL_GetError() );
		}
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(renderer == NULL) {
		SDL_DestroyWindow(window); window = NULL;
		LogMsg(ERR, "Could not create renderer: %s", SDL_GetError() );
		return( false );
	}

	//SDL_SetRenderDrawBlendMode( renderer, SDL_BLENDMODE_BLEND );

	SDL_GetWindowSize(window, &Video::w, &Video::h);

	// compute the half dimensions
	w2 = Video::w / 2;
	h2 = Video::h / 2;

	LogMsg(DEBUG, "Video mode initialized at %dx%dxbpp fixme\n", Video::w, Video::h );

	return( true );
}

/**\brief Register Lua functions for Video related operations.
 */
void Video::RegisterVideo(lua_State *L) {
	static const luaL_Reg videoFunctions[] = {
		{"getWidth", &Video::lua_getWidth},
		{"getHeight", &Video::lua_getHeight},
		{NULL, NULL}
	};

	luaL_openlib(L, EPIAR_VIDEO, videoFunctions, 0);

	lua_pop(L,1);
}

/**\brief Same as Video::GetWidth (Lua callable)
 */
int Video::lua_getWidth(lua_State *L) {
	lua_pushinteger(L, GetWidth() );
	return 1;
}

/**\brief Same as Video::GetHeight (Lua callable)
 */
int Video::lua_getHeight(lua_State *L) {
	lua_pushinteger(L, GetHeight() );
	return 1;
}

/**\brief Video updates.
 */
void Video::Update( void ) {
	SDL_RenderPresent(renderer);
}

/**\brief Clears screen.
 */
void Video::Erase( void ) {
	SDL_SetRenderDrawColor(renderer, 0., 0., 0., 255.);
	SDL_RenderClear(renderer);
}

/**\brief Returns the width of the screen.
 */
int Video::GetWidth( void ) {
	return( w );
}

/**\brief Returns the height of the screen.
 */
int Video::GetHeight( void ) {
	return( h );
}

/**\brief draws a point, a single pixel, on the screen
 */
void Video::DrawPoint( int x, int y, float r, float g, float b ) {
	SDL_SetRenderDrawColor(renderer, r * 255., g * 255., b * 255., 255.);
	SDL_RenderDrawPoint(renderer, x, y);
}

/**\brief Draw a point using Coordinate and Color.
 */
void Video::DrawPoint( Coordinate c, Color col ) {
	DrawPoint( (int)c.GetX(), (int)c.GetY(), col.r, col.g, col.b );
}

/**\brief Draw a Line.
 */
void Video::DrawLine( Coordinate p1, Coordinate p2, Color c, float a ) {
	DrawLine( p1.GetX(), p1.GetY(), p2.GetX(), p2.GetY(), c.r, c.g, c.b, a );
}

/**\brief Draw a Line.
 */
void Video::DrawLine( int x1, int y1, int x2, int y2, Color c, float a ) {
	DrawLine( x1, y1, x2, y2, c.r, c.g, c.b, a );
}

/**\brief Draw a Line.
 */
void Video::DrawLine( int x1, int y1, int x2, int y2, float r, float g, float b, float a ) {
	SDL_SetRenderDrawColor( renderer, r * 255., g * 255., b * 255., a * 255. );
  
	SDL_RenderDrawLine( renderer, x1, y1, x2, y2 );
}

/**\brief Draws a filled rectangle
 */
void Video::DrawRect( int x, int y, int w, int h, float r, float g, float b, float a ) {
	SDL_Rect rect;

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
 
	SDL_SetRenderDrawColor( renderer, r * 255., g * 255., b * 255., a * 255. );
  
	SDL_RenderFillRect( renderer, &rect );
}

void Video::DrawRect( int x, int y, int w, int h, Color c, float a ) {
	DrawRect( x, y, w, h, c.r, c.g, c.b, a );
}

void Video::DrawRect( Coordinate p, int w, int h, Color c, float a ) {
	DrawRect( p.GetX(), p.GetY(), w, h, c.r, c.g, c.b, a );
}

void Video::DrawBox( int x, int y, int w, int h, Color c, float a ) {
	DrawBox( x, y, w, h, c.r, c.g, c.b, a );
}

/**\brief Draws an unfilled rectangle
 */
void Video::DrawBox( int x, int y, int w, int h, float r, float g, float b, float a ) {
	SDL_Rect rect;

	a = 0.5;

	SDL_SetRenderDrawColor( renderer, r * 255., g * 255., b * 255., a * 255. );

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	SDL_RenderDrawRect( renderer, &rect );
}

/**\brief Draws a circle.
 */
void Video::DrawCircle( Coordinate c, int radius, float line_width, float r, float g, float b, float a) {
	DrawCircle( (int)c.GetX(), (int)c.GetY(), radius, line_width, r, g, b, a );
}

/**\brief Draws a circle with Color
 */
void Video::DrawCircle( Coordinate c, int radius, float line_width, Color col, float a) {
	DrawCircle( (int)c.GetX(), (int)c.GetY(), radius, line_width, col.r, col.g, col.b, a);
}

/**\brief Draws a circle
 */
void Video::DrawCircle( int x, int y, int radius, float line_width, float r, float g, float b, float a) {
	DrawEllipse( x, y, radius, radius, r, g, b, a );

	/*double prev_x, prev_y, cur_x, cur_y;

	SDL_SetRenderDrawColor( renderer, r * 255., g * 255., b * 255., a * 255. );

	Trig* t = Trig::Instance();

	prev_x = radius * t->GetCos(0) + x;
	prev_y = radius * t->GetSin(0) + y;

	for(int angle = 5; angle < 360; angle += 5) {
		cur_x = radius * t->GetCos(angle) + x;
		cur_y = radius * t->GetSin(angle) + y;

		SDL_RenderDrawLine(renderer, prev_x, prev_y, cur_x, cur_y);

		prev_x = cur_x;
		prev_y = cur_y;
	}

	cur_x = radius * t->GetCos(0) + x;
	cur_y = radius * t->GetSin(0) + y;

	SDL_RenderDrawLine(renderer, prev_x, prev_y, cur_x, cur_y);*/
}

void Video::DrawFilledCircle( Coordinate p, int radius, Color c, float a) {
	DrawFilledCircle( p.GetX(), p.GetY(), radius, c.r, c.g, c.b, a );
}

void Video::DrawFilledCircle( int x, int y, int radius, Color c, float a) {
	DrawFilledCircle( x, y, radius, c.r, c.g, c.b, a );
}

/**\brief Draw a filled circle.
 *
 *        Adapted from: SDL2_gfx (zlib licensed, aschiffler at ferzkopp dot net) http://www.ferzkopp.net/Software/SDL2_gfx/Docs/html/_s_d_l2__gfx_primitives_8c_source.html#l01457
 */
void Video::DrawFilledCircle( int x, int y, int rad, float r, float g, float b, float a) {
	Sint16 cx = 0;
	Sint16 cy = rad;
	Sint16 ocx = (Sint16) 0xffff;
	Sint16 ocy = (Sint16) 0xffff;
	Sint16 df = 1 - rad;
	Sint16 d_e = 3;
	Sint16 d_se = -2 * rad + 5;
	Sint16 xpcx, xmcx, xpcy, xmcy;
	Sint16 ypcy, ymcy, ypcx, ymcx;

	// Convert from 'float'
	r *= 255.;
	g *= 255.;
	b *= 255.;
	a *= 255.;

	// Sanity check radius 
	if (rad <= 0) {
		return;
	}

	// Set color
	SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
 
	// Draw 
	do {
		xpcx = x + cx;
		xmcx = x - cx;
		xpcy = x + cy;
		xmcy = x - cy;
		
		if (ocy != cy) {
			if (cy > 0) {
				ypcy = y + cy;
				ymcy = y - cy;

				SDL_RenderDrawLine(renderer, xmcx, ypcy, xpcx, ypcy);
				SDL_RenderDrawLine(renderer, xmcx, ymcy, xpcx, ymcy);
			} else {
				SDL_RenderDrawLine(renderer, xmcx, y, xpcx, y);
			}

			ocy = cy;
		}
		if (ocx != cx) {
			if (cx != cy) {
				if (cx > 0) {
					ypcx = y + cx;
					ymcx = y - cx;

					SDL_RenderDrawLine(renderer, xmcy, ymcx, xpcy, ymcx);
					SDL_RenderDrawLine(renderer, xmcy, ypcx, xpcy, ypcx);
				} else {
					SDL_RenderDrawLine(renderer, xmcy, y, xpcy, y);
				}
			}

			ocx = cx;
		}
 
		// Update 
		if (df < 0) {
			df += d_e;
			d_e += 2;
			d_se += 2;
		} else {
			df += d_se;
			d_e += 2;
			d_se += 4;
			cy--;
		}

		cx++;
	} while (cx <= cy);
}

/**\brief Draw an ellipse.
 *
 *        Adapted from: SDL2_gfx (zlib licensed, aschiffler at ferzkopp dot net) http://www.ferzkopp.net/Software/SDL2_gfx/Docs/html/_s_d_l2__gfx_primitives_8c_source.html#l01598
 */
void Video::DrawEllipse( int x, int y, int rx, int ry, float r, float g, float b, float a) {
	int ix, iy;
	int h, i, j, k;
	int oh, oi, oj, ok;
	int xmh, xph, ypk, ymk;
	int xmi, xpi, ymj, ypj;
	int xmj, xpj, ymi, ypi;
	int xmk, xpk, ymh, yph;

	// Convert from 'float'
	r *= 255.;
	g *= 255.;
	b *= 255.;
	a *= 255.;
 
         // Sanity check radii 
         if ((rx <= 0) || (ry <= 0)) {
                 return;
         }
 
         // Set color
         SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
         SDL_SetRenderDrawColor(renderer, r, g, b, a);
 
         // Init vars 
         oh = oi = oj = ok = 0xFFFF;
 
         // Draw 
         if (rx > ry) {
                 ix = 0;
                 iy = rx * 64;
 
                 do {
                         h = (ix + 32) >> 6;
                         i = (iy + 32) >> 6;
                         j = (h * ry) / rx;
                         k = (i * ry) / rx;
 
                         if (((ok != k) && (oj != k)) || ((oj != j) && (ok != j)) || (k != j)) {
                                 xph = x + h;
                                 xmh = x - h;
                                 if (k > 0) {
                                         ypk = y + k;
                                         ymk = y - k;
                                         SDL_RenderDrawPoint(renderer, xmh, ypk);
                                         SDL_RenderDrawPoint(renderer, xph, ypk);
                                         SDL_RenderDrawPoint(renderer, xmh, ymk);
                                         SDL_RenderDrawPoint(renderer, xph, ymk);
                                 } else {
                                         SDL_RenderDrawPoint(renderer, xmh, y);
                                         SDL_RenderDrawPoint(renderer, xph, y);
                                 }
                                 ok = k;
                                 xpi = x + i;
                                 xmi = x - i;
                                 if (j > 0) {
                                         ypj = y + j;
                                         ymj = y - j;
                                         SDL_RenderDrawPoint(renderer, xmi, ypj);
                                         SDL_RenderDrawPoint(renderer, xpi, ypj);
                                         SDL_RenderDrawPoint(renderer, xmi, ymj);
                                         SDL_RenderDrawPoint(renderer, xpi, ymj);
                                 } else {
                                         SDL_RenderDrawPoint(renderer, xmi, y);
                                         SDL_RenderDrawPoint(renderer, xpi, y);
                                 }
                                 oj = j;
                         }
 
                         ix = ix + iy / rx;
                         iy = iy - ix / rx;
 
                 } while (i > h);
         } else {
                 ix = 0;
                 iy = ry * 64;
 
                 do {
                         h = (ix + 32) >> 6;
                         i = (iy + 32) >> 6;
                         j = (h * rx) / ry;
                         k = (i * rx) / ry;
 
                         if (((oi != i) && (oh != i)) || ((oh != h) && (oi != h) && (i != h))) {
                                 xmj = x - j;
                                 xpj = x + j;
                                 if (i > 0) {
                                         ypi = y + i;
                                         ymi = y - i;
                                         SDL_RenderDrawPoint(renderer, xmj, ypi);
                                         SDL_RenderDrawPoint(renderer, xpj, ypi);
                                         SDL_RenderDrawPoint(renderer, xmj, ymi);
                                         SDL_RenderDrawPoint(renderer, xpj, ymi);
                                 } else {
                                         SDL_RenderDrawPoint(renderer, xmj, y);
                                         SDL_RenderDrawPoint(renderer, xpj, y);
                                 }
                                 oi = i;
                                 xmk = x - k;
                                 xpk = x + k;
                                 if (h > 0) {
                                         yph = y + h;
                                         ymh = y - h;
                                         SDL_RenderDrawPoint(renderer, xmk, yph);
                                         SDL_RenderDrawPoint(renderer, xpk, yph);
                                         SDL_RenderDrawPoint(renderer, xmk, ymh);
                                         SDL_RenderDrawPoint(renderer, xpk, ymh);
                                 } else {
                                         SDL_RenderDrawPoint(renderer, xmk, y);
                                         SDL_RenderDrawPoint(renderer, xpk, y);
                                 }
                                 oh = h;
                         }
 
                         ix = ix + iy / ry;
                         iy = iy - ix / ry;
 
                 } while (i > h);
         }
}

/**\brief Draws a targeting overlay.
 */
void Video::DrawTarget( int x, int y, int w, int h, int d, float r, float g, float b, float a ) {
	float w2 = w / 2.;
	float h2 = h / 2.;

	SDL_SetRenderDrawColor( renderer, r * 255., g * 255., b * 255., a * 255. );

	// Upper Left Corner
	SDL_RenderDrawLine(renderer, x - w2, y - h2, x - w2, y - h2 + d);
	SDL_RenderDrawLine(renderer, x - w2, y - h2, x - w2 + d, y - h2);

	// Upper Right Corner
	SDL_RenderDrawLine(renderer, x + w2, y - h2, x + w2, y - h2 + d);
	SDL_RenderDrawLine(renderer, x + w2, y - h2, x + w2 - d, y - h2);

	// Lower Left Corner
	SDL_RenderDrawLine(renderer, x - w2, y + h2, x - w2, y + h2 - d);
	SDL_RenderDrawLine(renderer, x - w2, y + h2, x - w2 + d, y + h2);

	// Lower Right Corner
	SDL_RenderDrawLine(renderer, x + w2, y + h2, x + w2, y + h2 - d);
	SDL_RenderDrawLine(renderer, x + w2, y + h2, x + w2 - d, y + h2);
}

/**\brief Enables the mouse
 */
void Video::EnableMouse( void ) {
	SDL_ShowCursor( 1 );
}

/**\brief Disables the mouse
 */
void Video::DisableMouse( void ) {
	SDL_ShowCursor( 0 );
}

/**\brief Returns the half width.
 */
int Video::GetHalfWidth( void ) {
	return( w2 );
}

/**\brief Returns the half height.
 */
int Video::GetHalfHeight( void ) {
	return( h2 );
}

/**\brief Set crop rectangle.
 */
void Video::SetCropRect( int x, int y, int w, int h ) {
	int xn, yn, wn, hn;

	if (cropRects.empty()) {
		xn = x;
		yn = y;
		wn = w;
		hn = h;
	} else {
		// Need to detect which part of crop rectangle is within previous rectangle
		// So we don't miss things that needs to be cropped.
		Rect prevrect = cropRects.top();

		int rightprev = TO_INT(prevrect.x + prevrect.w);
		int right = x + w;
		int botprev = TO_INT(prevrect.y + prevrect.h);
		int bot = y + h;

		xn = prevrect.x > x ? TO_INT(prevrect.x) : x;	       // Left
		yn = prevrect.y > y ? TO_INT(prevrect.y) : y;	       // Top
		wn = rightprev > right ? right - xn : rightprev - xn;  // Right
		hn = botprev > bot ? bot - yn : botprev - yn;          // Bottom

		if(wn < 0) wn = 0; // crops don't overlap. we still record this as they're going to call unsetcrop
		if(hn < 0) hn = 0; // same reason as line above
	}

	cropRects.push(Rect( xn, yn, wn, hn ));

	SDL_Rect rect;

	rect.x = xn;
	rect.y = yn;
	rect.w = wn;
	rect.h = hn;

	if( (wn > 0) && (hn > 0) ) {
		SDL_RenderSetClipRect( renderer, &rect );
	}
}

/**\brief Unset the previous crop rectangle after use.
 */
void Video::UnsetCropRect( void ) {
	if(!cropRects.empty()) { // Shouldn't be empty
		cropRects.pop();
	} else {
		LogMsg(WARN, "You unset the crop rect too many times.");
	}

	if( cropRects.empty() ) {
		SDL_RenderSetClipRect( renderer, NULL );
	} else {
		// Set's the previous crop rectangle.
		Rect prevrect = cropRects.top();
		SDL_Rect rect;

		rect.x = TO_INT(prevrect.x);
		rect.y = TO_INT(prevrect.y);
		rect.w = TO_INT(prevrect.w);
		rect.h = TO_INT(prevrect.h);

		if( (rect.w > 0) && (rect.h > 0) ) {
			SDL_RenderSetClipRect( renderer, &rect );
		}
	}
}

/**\brief Takes a screenshot of the game and saves it to an Image.
 */
Image *Video::CaptureScreen( void ) {
	//GLuint screenCapture;

	//glGenTextures( 1, &screenCapture );

	//glBindTexture(GL_TEXTURE_2D, screenCapture);

	//glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 0, 0, w, h, 0);

	//Image *screenshot = new Image( screenCapture, w, h );

	//return( screenshot );
	return NULL;
}

/**\brief Takes a screenshot of the game and saves it to a file.
 */
void Video::SaveScreenshot( string filename ) {
	// unsigned int size = w * h * 4;
	// int *pixelData, *pixelDataOrig;
	//
	// pixelData = (int *)malloc( size );
	// pixelDataOrig = (int *)malloc( size );
	// memset( pixelData, 0, size );
	// memset( pixelDataOrig, 0, size );
	//
	// glPixelStorei( GL_PACK_ROW_LENGTH, 0 ) ;
	// glPixelStorei( GL_PACK_ALIGNMENT, 1 ) ;
	// glReadPixels( 0, 0, w, h, GL_BGRA, GL_UNSIGNED_BYTE, pixelDataOrig ) ;
	//
	// // image if flipped vertically - fix this
	// for(int x = 1; x < w; x++) {
	// 	for(int y = 1; y < h; y++) {
	// 		pixelData[x + (y * w)] = pixelDataOrig[x + ((h-y) * w)];
	// 	}
	// }
	//
	// SDL_Surface *s = SDL_CreateRGBSurfaceFrom( pixelData, w, h, screen->format->BitsPerPixel, screen->pitch, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
	//
	// if( filename == "" ) filename = string("Screenshot_") + Log::GetTimestamp() + string(".bmp");
	//
	// SDL_SaveBMP( s, filename.c_str() );
	//
	// SDL_FreeSurface( s );
	// free( pixelData );
	// free( pixelDataOrig );

	LogMsg(WARN, "Video::SaveScreenshot() is unimplemented." );
}
