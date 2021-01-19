/**\file			animation.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */

#ifndef __h_animation__
#define __h_animation__

#include "graphics/image.h"
#include "utilities/resource.h"
#include "includes.h"

class Ani : public Resource {
	public:
		Ani();
		Ani( string& filename );
		bool Load( string& filename );
		static Ani* Get(string filename);

		Image* GetFrame(int frameNum);
		int GetNumFrames() { return numFrames; }
		int GetDelay() { return delay; }
		int GetWidth() { return w; }
		int GetHeight() { return h; }

	private:
		Image *frames;
		int numFrames;
		Uint32 delay;
		int w, h;
};

class Animation {
	public:
		Animation();
		Animation( string filename );
		bool Update( void );
		void Draw( int x, int y, float ang, float alpha );
		void SetLoopPercent( float loopPercent );
		float GetLoopPercent( void ) { return loopPercent; };
		void Reset( void );
		int GetHalfWidth( void ) { return ani->GetWidth() / 2; };
		int GetHalfHeight( void ) { return ani->GetHeight() / 2; };

	private:
		Ani *ani;
		Uint32 startTime;
		float loopPercent;
		int fnum;
};

#endif // __h_animation__

