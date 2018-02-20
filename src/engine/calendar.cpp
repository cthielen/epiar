/**\file			calendar.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Sunday, June 24, 2012
 * \date			Modified: Sunday, August 14, 2016
 * \brief
 * \details
 */

#include "includes.h"
#include "engine/calendar.h"
#include "engine/hud.h"

/**\class Calendar
 * \brief A stardate system.
 */

/**\brief Initializes a new Calendar to default values.
 *
 */
Calendar::Calendar() :
	period(1),
	epoch(3002)
{
	ticker = 0;
}

string Calendar::Now(void) {
	stringstream ret;
  
	ret << "Period " << period << " of Epoch " << epoch;
    
	return ret.str();
}

/**\brief To be called once per logic frame update. Advances the calendar after a certain number of ticks.
 *
 */
void Calendar::Update(void) {
	int old_period = period;
	int old_epoch = epoch;
  
	ticker++;
  
	if(ticker > LOGIC_FRAMES_PER_PERIOD) {
		ticker = 0;
		period++;
	}
  
	AdjustEpoch();
  
	if((old_period != period) || (old_epoch != epoch)) {
		Hud::Alert(true, "Day changed to %s", Now().c_str());
	}
}

/**\brief Advances the date.
 *
 */
void Calendar::Advance() {
	period++;
	ticker = 0;
  
	AdjustEpoch();
  
	Hud::Alert(true, "Day changed to %s", Now().c_str());
}

/**\brief Advances the date after n seconds.
 *
 */
void Calendar::AdvanceAfter(int seconds) {
	ticker = LOGIC_FRAMES_PER_PERIOD - (seconds * LOGIC_FPS);
}

void Calendar::AdjustEpoch() {
	while(period > PERIODS_PER_EPOCH) {
		period -= PERIODS_PER_EPOCH;
		epoch++;
	}
}
