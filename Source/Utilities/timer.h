/**\file			timer.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#ifndef __h_timer__
#define __h_timer__

#define LOGIC_FPS 50.0

#include "includes.h"

class Timer {
	public:
		static void Initialize( void );
		static int Update( void );
		static void Delay( int waitMS );
		static Uint32 GetTicks( void );
		static Uint32 GetRealTicks( void );
		static double GetFFrame( void );
		
		static double GetDelta( void );

		static Uint32 GetLogicalFrameCount( void );
	
  	private:
  		static Uint32 lastLoopLength;
  		static Uint32 lastLoopTick;
		static Uint32 logicalFrameCount;
		static double frames;
		static double fframe;
};

#endif // __h_timer__
