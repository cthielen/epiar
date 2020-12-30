/**\file			console.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Saturday, February 21, 2009
 * \date			Modified: Thursday, December 24, 2015
 * \brief			Handles the Heads-Up-Display
 * \details
 */

#include "includes.h"
#include "common.h"
#include "engine/console.h"
#include "graphics/video.h"
#include "utilities/log.h"
#include "utilities/lua.h"
#include "utilities/timer.h"

/**\class Console
 * \brief Handles the Heads-Up-Display. */

const char* PROMPT = "> ";
const char* CURSOR = "_";

/**\brief Initialize Console instance.
 */
Console::Console( lua_State *state ) {
	Buffer.push_back("Console initialized.");
	command = "";
	enabled = false;
	cursor = 0;
	L = state;
}

/**\brief Handles a list of Input events.
 * \param events A list of events
 */
void Console::HandleInput( list<InputEvent> & events ) {
	// look for the backquote (`) key to toggle the console
	for( list<InputEvent>::iterator i = events.begin(); i != events.end(); ) {
		bool skipIncrement = false;

		switch( i->type ) {
			case KEY:
				if( (i->key == SDLK_BACKQUOTE) && (i->kstate == KEYTYPED)) {
					enabled = enabled ? false : true;
				} else if(( i->kstate == KEYDOWN ) || ( i->kstate == KEYPRESSED )) {
					if( enabled ) {
						// Disable other commands if console is open (KEYDOWN on space fires laser and inserts space in console otherwise)
						i = events.erase( i );
						skipIncrement = true;
					}
				} else if( i->kstate == KEYUP ) {
					if( enabled ) {
						if(i->key == SDLK_RETURN) {
							RunCommand();
							i = events.erase( i );
						}
					}
				} else if( i->kstate == KEYTYPED) {
					if( enabled ) {
						switch(i->key) {
						case SDLK_ESCAPE:
							enabled = false;
						break;

						// Ignore Modifiers
						case SDLK_LSHIFT:
						case SDLK_RSHIFT:
						case SDLK_RGUI:
						case SDLK_LGUI:
						case SDLK_RALT:
						case SDLK_LALT:
						case SDLK_RCTRL:
						case SDLK_LCTRL:
						break;

						// Cursor Movements
						case SDLK_LEFT:
							cursor = cursor!=0 ? cursor-1 : cursor;
						break;
						case SDLK_RIGHT:
							cursor = cursor < command.size() ? cursor+1 : cursor;
						break;

						case SDLK_UP:
						case SDLK_DOWN:
							// TODO: add cursor history
						break;
						case SDLK_RETURN:
						case SDLK_KP_ENTER:
						//case '\n': // this shouldn't be necessary but on os x and fedora the two above it don't seem to catch
							RunCommand();
						break;
						case SDLK_BACKSPACE:
							if(command.size() > 0) {
								command.erase( --cursor, 1 );
							}
						break;
						default:
							if(i->key != '\n') {
								command.insert(cursor++, 1, i->key );
							}
						break;
						}

						// Remove it from the queue
						i = events.erase( i );
						skipIncrement = true;

					}
				}
			break;

			default:
				break;
		}

		if(!skipIncrement)
			++i;
	}
}

/**\brief Draws the current console.
 */
void Console::Draw() {
	if( enabled ) {
		int pos = 8;

		// Draw background
		Video::DrawRect(150, 5, 550, Mono->LineHeight() * (pos + 1),
				static_cast<float>(.3), static_cast<float>(.3),
				static_cast<float>(.3), static_cast<float>(.5) );

		Mono->SetColor(.9f, .9f, .9f, 1.0);

		Mono->Render(155, pos-- * Mono->LineHeight() + 5,  PROMPT + command.substr(0,cursor) + CURSOR + command.substr(cursor)  );

		for(int i = Buffer.size() - 1; i >= 0 && pos > 0; --i, --pos) {
			Mono->Render(155, pos * Mono->LineHeight() + 5, Buffer[i]);
		}
	}
}

/**\brief Console update function.
 */
void Console::Update() {
	if( enabled ) {

	}
}

void Console::RunCommand() {
	int returnvals;
	const char* returnval;

	// Run the Command
	returnvals = Lua::Run( command, true);

	// Save this command
	InsertResult(string(PROMPT) + command);
	command.clear();
	cursor = 0;

	// Insert each result value as a new line
	for(int n = returnvals; n > 0; --n) {
		returnval = lua_tostring(L, -n);
		if( returnval != NULL ) {
			InsertResult( returnval );
		} else {
			InsertResult( "nil" );
		}
	}

	// Cleanup the Stack
	lua_pop(L, returnvals);
}

/**\brief Used by lua functions, eg echo.
 */
void Console::InsertResult(string result) {
	// insert result into buffer
	Buffer.push_back(result);
}
