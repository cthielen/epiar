/**\file			ui_action.h
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Saturday, April 30, 2011
 * \date			Modified: Saturday, April 30, 2011
 * \brief
 * \details
 */

#ifndef __H_ACTION__
#define __H_ACTION__

#include "includes.h"
#include "Utilities/lua.h"

enum action_type{
	Action_MouseDrag,
	Action_MouseMotion,
	Action_MouseEnter,
	Action_MouseLeave,
	Action_MouseLUp,
	Action_MouseLDown,
	Action_MouseLRelease,
	Action_MouseMUp,
	Action_MouseMDown,
	Action_MouseMRelease,
	Action_MouseRUp,
	Action_MouseRDown,
	Action_MouseRRelease,
	Action_MouseWUp,
	Action_MouseWDown,
	Action_KeyboardEnter,
	Action_KeyboardLeave,
	//Action_KeyPress
	Action_Close,
	
	Action_Last // Always the last action type
}; ///< Actions that can be registered.

class Action {
	public:
		virtual void Activate( int x, int y ) = 0;
		virtual ~Action() {};
};

class LuaAction : public Action {
	public:
		LuaAction( string callback );
		void Activate( int x, int y );
	private:
		string lua_callback;
};

class LuaPositionalAction : public Action {
	public:
		LuaPositionalAction( string callback );
		void Activate( int x, int y );
	private:
		string lua_callback;
};

class VoidAction : public Action {
	public:
		VoidAction( void (*callback)() );
		void Activate( int x, int y );
	private:
		void (*clickCallBack)();
};

class ObjectAction : public Action {
	public:
		ObjectAction( void (*function)(void*), void* value );
		void Activate( int x, int y );
	private:
		void (*clickCallBack)(void*);
		void *object;
};

class MessageAction : public Action {
	public:
		MessageAction( void (*function)(void*,void*), void* obj, void* msg );
		void Activate( int x, int y );
	private:
		void (*clickCallBack)(void*,void*);
		void *object;
		void *message;
};

class PositionalAction : public Action {
	public:
		PositionalAction ( void (*function)(void* obj, void*msg, int x, int y), void* obj, void* msg );
		void Activate( int x, int y );
	private:
		void (*clickCallBack)(void*,void*,int,int);
		void *object;
		void *message;
};

#endif // __H_ACTION__

