/**\file			image.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Saturday, January 31, 2009
 * \date			Modified: Sunday, January 3, 2016
 * \brief			Image loading and display
 * \details
 * See this note section in image.h for an important clarification about the handling
 * of non-power of two image sizes and the difference between virtual/effective dimensions
 * and real dimensions.
 */

#include "includes.h"
#include "graphics/image.h"
#include "graphics/video.h"
#include "utilities/file.h"
#include "utilities/log.h"
#include "utilities/trig.h"

/**\class Image
 * \brief Image handling. */

/**\brief Constructor, initialize default values
 */
Image::Image() {
	w = h = real_w = real_h = 0;
	image = NULL;
	scale_w = scale_h = 1.;
	filepath = "";
}

/**\brief Create instance by loading image from file
 */
Image::Image( const string& filename ) {
	w = h = real_w = real_h = 0;
	image = NULL;
	scale_w = scale_h = 1.;
	filepath = "";

	Load(filename);
}

/**\brief Create instance from existing OpenGL texture
 */
Image::Image( SDL_Texture* texture, int w, int h ) {
	this->w = real_w = w;
	this->h = real_h = h;
	scale_w = scale_h = 1.;
	filepath = "";

	image = texture;
}

/**\brief Deallocate allocations
 */
Image::~Image() {
	if ( image ) {
		SDL_DestroyTexture( image );
		image = NULL;
	}
}

/**\brief Lazy fetch an Image
 */
Image* Image::Get( string filename ) {
	Image* value = NULL;
	value = static_cast<Image*>(Resource::Get(filename));

	if( value == NULL ) {
		value = new Image();

		if(value->Load(filename)) {
			Resource::Store(filename, (Resource*)value);
		} else {
			LogMsg(DEBUG, "Couldn't Find Image '%s'", filename.c_str());
			delete value;
			return NULL;
		}
	}

	return value;
}

/**\brief Load image from file
 */
bool Image::Load( const string& filename ) {
	File file = File();

	if( filename == "" ) {
		return false; // No File to load.
	}

	if( !file.OpenRead(filename ) ) {
		return false; // File could not be opened or found.
	}

	char* buffer = file.Read();
	int bytesread = file.GetLength();

	if ( buffer == NULL ) {
		return false; // File could not be Read.
	}

	int retval = Load( buffer, bytesread );
	delete [] buffer;

	if ( retval ) {
		filepath = filename;
		return true;
	}

	return false; // Image could not be loaded.
}

/**\brief Load image from buffer
 */
bool Image::Load( char *buf, int bufSize ) {
	SDL_RWops *rw;

	rw = SDL_RWFromMem( buf, bufSize );
	if( rw == NULL ) {
		LogMsg(WARN, "Image loading failed. Could not create RWops" );
		return( false );
	}

	image = IMG_LoadTexture_RW( Video::GetRenderer(), rw, 0 );
	SDL_FreeRW(rw);
	if( image == NULL ) {
		LogMsg(WARN, "Failed to load image from buffer" );
		return( false );
	}

	SDL_QueryTexture(image, NULL, NULL, &w, &h);

	return( true );
}

/**\brief Draw the image (angle is in degrees)
 */
void Image::Draw( int x, int y, float angle ) {
	_Draw( x, y, 1.f, 1.f, 1.f, 1.f, angle );
}

/**\brief Draw the image (angle is in degrees, alpha is between 0.0 and 1.0)
 */
void Image::DrawAlpha( int x, int y, float alpha ) {
	_Draw( x, y, 1.f, 1.f, 1.f, alpha );
}

/**\brief Draw the image (angle is in degrees)
 */
void Image::_Draw( int x, int y, float r, float g, float b, float alpha, float angle, float resize_ratio_w, float resize_ratio_h) {
	if( image == NULL ) {
		LogMsg(WARN, "Trying to draw without loading an image first." );
		return;
	}

	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	dest.w = w * resize_ratio_w;
	dest.h = h * resize_ratio_h;

	SDL_SetTextureAlphaMod(image, alpha * 255.);
	SDL_RenderCopyEx(Video::GetRenderer(), image, NULL, &dest, angle, NULL, SDL_FLIP_NONE );
}

/**\brief Draw the image centered on (x,y)
 */
void Image::DrawCentered( int x, int y, float angle ) {
	Draw( x - (w / 2), y - (h / 2), angle );
}

/**\brief Draw the image stretched within to a box
 */
void Image::DrawStretch( int x, int y, int box_w, int box_h, float angle ) {
	assert(this);
	assert(this->w);
	assert(this->h);

	float wf = static_cast<float>(this->w);
	float hf = static_cast<float>(this->h);

	float resize_ratio_w = static_cast<float>(box_w) / wf;
	float resize_ratio_h = static_cast<float>(box_h) / hf;

	_Draw(x, y, 1.f, 1.f, 1.f, 1.f, angle, resize_ratio_w, resize_ratio_h);
}

/**\brief Draw the image within a box but not stretched
 */
void Image::DrawFit( int x, int y, int box_w, int box_h, float angle ) {
	float resize_ratio_w = (float)box_w / (float)this->w;
	float resize_ratio_h = (float)box_h / (float)this->h;
	// Use Minimum of the two ratios
	float resize_ratio = resize_ratio_w<resize_ratio_h ? resize_ratio_w : resize_ratio_h;

	_Draw(x, y, 1.f, 1.f, 1.f, 1.f, angle, resize_ratio, resize_ratio);
}

/**\brief Draw the image tiled to fill a rectangle of w/h - will crop to meet w/h and won't overflow
 */
void Image::DrawTiled( int x, int y, int fill_w, int fill_h, float alpha ) {
	if( image == NULL ) {
		LogMsg(WARN, "Trying to draw without loading an image first." );
		return;
	}

	Video::SetCropRect(x, y, fill_w, fill_h);

	for( int j = 0; j < fill_h; j += h) {
		for( int i = 0; i < fill_w; i += w) {
			SDL_Rect src, dest;

			src.x = 0;
			src.y = 0;
			src.w = fill_w < w ? fill_w : w;
			src.h = fill_h < h ? fill_h : h;
			dest.x = x + i;
			dest.y = y + j;
			dest.w = fill_w < w ? fill_w : w;
			dest.h = fill_h < h ? fill_h : h;

			SDL_SetTextureAlphaMod(image, alpha * 255.);
			SDL_RenderCopyEx(Video::GetRenderer(), image, &src, &dest, 0., NULL, SDL_FLIP_NONE );
		}
	}

	Video::UnsetCropRect();
}


/**\fn Image::GetWidth()
 *  \brief Returns width of image.
 * \fn Image::GetHeight()
 *  \brief Returns height of image.
 * \fn Image::GetHalfWidth()
 *  \brief Returns half the width of image.
 * \fn Image::GetHalfHeight()
 *  \brief Returns half the height of image.
 * \fn Image::GetPath()
 *  \brief Returns the path to the image.
 */
