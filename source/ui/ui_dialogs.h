/**\file			ui_common.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Thursday, May 5, 2011
 * \date			Modified: Thursday, May 5, 2011
 * \brief
 * \details
 */

#ifndef __H_DIALOGS__
#define __H_DIALOGS__

#include "graphics/image.h"
#include "ui/ui.h"

class Dialogs {
	public:
		static bool Confirm( const string message );
		static void Alert( const string message );
		static void OptionsWindow();
};

#endif // __H_DIALOGS__
