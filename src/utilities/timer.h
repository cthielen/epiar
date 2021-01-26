/**\file			timer.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Monday, January 25, 2021
 * \brief
 * \details
 */

#ifndef __h_timer__
#define __h_timer__

/* DO NOT CHANGE LOGIC_FPS. THE VELOCITIES IN THIS GAME ARE BASED ON A LOGICAL FPS of 30 / s.
 * THIS IS THE GAME LOGIC RATE, NOT THE VIDEO FRAME RATE! */
#define LOGIC_FPS     30.0
#define INITIAL_DELAY 5

#include "includes.h"

class Timer {
	public:
		static void Initialize( void );
		static int Update( void );
		static void Delay( void );
		static Uint32 GetTicks( void );
		static Uint32 GetRealTicks( void );
		static double GetFFrame( void );
		static double GetDelta( void );
		static Uint32 GetLogicalFrameCount( void );
		static void Pause( void );
		static void Unpause( void );
		static Uint32 TicksSince( Uint32 timestamp );

  	private:
		static Uint32 lastLoopLength;
		static Uint32 lastLoopTick;
		static Uint32 logicalFrameCount;
		static double frames; // running count of total logic frames that should have occurred
		static double fframe;
		static int lastLogicalLoops;
		static int delayMS;
		static Uint32 desiredFPS;
		static Uint32 pausedAt;
		static Uint32 pauseDelay;
};

#endif // __h_timer__
