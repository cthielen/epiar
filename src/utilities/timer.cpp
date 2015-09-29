/**\file			timer.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "utilities/timer.h"

/**\class Timer
 * \brief Timer class. */

Uint32 Timer::lastLoopLength = 0;
Uint32 Timer::lastLoopTick = SDL_GetTicks();
double Timer::frames = 0.;
double Timer::fframe = 0.;
Uint32 Timer::logicalFrameCount = 0;
int Timer::lastLogicalLoops = 0;
int Timer::delayMS = INITIAL_DELAY;
Uint32 Timer::desiredFPS = 60;

void Timer::Initialize( void ) {
	lastLoopLength = 0;
	lastLoopTick = SDL_GetTicks();
	Uint32 fps = OPTION( Uint32, "options/video/fps" );
	if( fps > 0 ) desiredFPS = fps;
}

// Updates the Timer class and returns the number of logical loops to run.
int Timer::Update( void ) {
	Uint32 tick = SDL_GetTicks();

	lastLoopLength = tick - lastLoopTick;
	lastLoopTick = tick;

	double dt = (double)lastLoopLength * 0.001f;
	double new_frames = dt * LOGIC_FPS;

	int logical_loops = static_cast<int>(floor(frames + new_frames) - floor(frames));
	frames += new_frames;

	fframe = frames - floor(frames);

	lastLogicalLoops = logical_loops;

	logicalFrameCount += logical_loops;

	return logical_loops;
}

// Returns the fraction of a frame at this point in the draw cycle.
// Used to interpolate graphic screen coordinates.
double Timer::GetFFrame( void ) {
	return fframe;
}

Uint32 Timer::GetTicks( void )
{
	return( lastLoopTick );
}

/** \brief Get the true real-time tick.
 *  \details Use GetTicks() for gametime ticks, use this for animations that
 *  need to occur while the game is paused.
 *
 */
Uint32 Timer::GetRealTicks( void )
{
	return SDL_GetTicks();
}

inline int clamp(int x, int a, int b)
{
    return x < a ? a : (x > b ? b : x);
}

// Adaptive timer. Adjusts the sleep count to always ensure at least
// one logical loop was needed. If too many logical loops are needed,
// it will decrease the sleep time.
// This attempts to balance game performance and energy savings/CPU usage.
void Timer::Delay() {
	if( lastLoopLength < (desiredFPS / 1000.) ) {
		// If the machine is too fast, we can delay longer
		delayMS += 5;
	} else if(lastLoopLength > (desiredFPS / 1000.)) {
		delayMS -= 5;
	}
	delayMS = clamp(delayMS, 5, (int)(desiredFPS / 1000.0));

	SDL_Delay( delayMS );
}

double Timer::GetDelta( void ) {
	return LOGIC_FPS * 0.001f;
}

Uint32 Timer::GetLogicalFrameCount( void )
{
	return logicalFrameCount;
}
