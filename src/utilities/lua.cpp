/**\file			lua.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Saturday, January 5, 2008
 * \date			Modified: Saturday, November 21, 2009
 * \brief			Provides abilities to load, store, and run Lua scripts
 * \details
 * To be used in conjunction with various other subsystems, A.I., GUI, etc.
 */

#include "includes.h"
#include "common.h"

#include "utilities/file.h"
#include "utilities/lua.h"
#include "utilities/log.h"

/**\class Lua
 * \brief Lua subsystem. */

bool Lua::luaInitialized = false;
lua_State *Lua::L = NULL;

bool Lua::Load( const string& filename ) {
	File pathTranslator; // use this to determine the physfs-resolved path, e.g. absolute/full path

	if( ! luaInitialized ) {
		if( Init() == false ) {
			LogMsg(WARN, "Could not load Lua script. Unable to initialize Lua." );
			return( false );
		}
	}

	if( pathTranslator.OpenRead( filename ) == false ) {
		LogMsg(ERR,"Error loading '%s' from filesystem", filename.c_str());
		return false;
	}

	// Load the lua script
	if( 0 != luaL_loadfile(L, pathTranslator.GetAbsolutePath().c_str()) ) {
		LogMsg(ERR,"Error loading '%s': %s", pathTranslator.GetAbsolutePath().c_str(), lua_tostring(L, -1));
		return false;
	}

	// Execute the lua script
	if( 0 != lua_pcall(L, 0, 0, 0) ) {
		LogMsg(ERR,"Error Executing '%s': %s", filename.c_str(), lua_tostring(L, -1));
		return false;
	}

	LogMsg(INFO,"Loaded Lua Script '%s'",filename.c_str());
	return( true );
}


/**\brief Run an arbitrary string as Lua code
 * \returns The number of return values from that string.
 *
 * \note If the function is known at compile time, use 'Call' instead of 'Run'.
 */
int Lua::Run( string line, bool allowReturns ) {
	int stack_before, stack_after;

	if( ! luaInitialized ) {
		if( Init() == false ) {
			LogMsg(WARN, "Could not load Lua script. Unable to initialize Lua." );
			return( false );
		}
	}

	// Record the stack position so that the stack can be reset if returns are not allowed.
	stack_before = lua_gettop(L);
	if( allowReturns ) {
		line = "return " + line;
	}

	// Run the String!
	if( luaL_dostring(L,line.c_str()) ) {
		LogMsg(ERR, "Error running '%s': %s", line.c_str(), lua_tostring(L, -1));
		lua_settop(L, stack_before);  /* pop error message from the stack */
		Lua::stackDump( L );
		return 0;
	}

	// Reset the stack or return a count of returned values
	if( !allowReturns ) {
		lua_settop(L, stack_before);
		stack_after = lua_gettop(L);
		assert( stack_before == stack_after );
		return 0;
	} else {
		stack_after = lua_gettop(L);
		return( stack_after - stack_before );
	}
}

// This function is from the Lua PIL
// http://www.lua.org/pil/25.3.html
// It was originally named "call_va"
//
// WARNING: any s as a return must be a pointer to a string (not a c str)
//          This allows Lua::Call to clear the stack when we're done.
bool Lua::Call(const char *func, const char *sig, ...) {
	va_list vl;
	int narg, nres,resultcount;  /* number of arguments and results */

	va_start(vl, sig);
	lua_getglobal(L, func);  /* get function */

	/* push arguments */
	narg = 0;
	while (*sig) {  /* push arguments */
		switch (*sig++) {

			case 'd':  /* double argument */
				lua_pushnumber(L, va_arg(vl, double));
				break;

			case 'i':  /* int argument */
				lua_pushnumber(L, va_arg(vl, int));
				break;

			case 's':  /* string argument */
				lua_pushstring(L, va_arg(vl, char *));
				break;

			case '>':
				goto endwhile;

			default:
				luaL_error(L, "invalid option (%c)", *(sig - 1));
		}
		narg++;
		luaL_checkstack(L, 1, "too many arguments");
	} endwhile:

	/* do the call */
	resultcount = strlen(sig);  /* number of expected results */
	if (lua_pcall(L, narg, resultcount, 0) != 0)  /* do the call */
	{
		luaL_error(L, "error running function `%s': %s",
				func, lua_tostring(L, -1));
		Lua::stackDump( L );
	}

	/* retrieve results */
	nres = -resultcount;  /* stack index of first result */
	while (*sig) {  /* get results */
		switch (*sig++) {

			case 'd':  /* double result */
				if (!lua_isnumber(L, nres))
					luaL_error(L, "wrong result kind");
				*va_arg(vl, double *) = lua_tonumber(L, nres);
				break;

			case 'i':  /* int result */
				if (!lua_isnumber(L, nres))
					luaL_error(L, "wrong result kind");
				*va_arg(vl, int *) = (int)lua_tonumber(L, nres);
				break;

			case 's':  /* string result */
				if (!lua_isstring(L, nres))
					luaL_error(L, "wrong result kind");
				*va_arg(vl, string*) = lua_tostring(L, nres);
				break;

			default:
				luaL_error(L, "invalid option (%c)", *(sig - 1));
		}
		nres++;
	}
	va_end(vl);
	lua_pop(L,resultcount);
	return true;
}

bool Lua::Init() {
	if( luaInitialized ) {
		LogMsg(WARN, "Cannot initialize Lua. It is already initialized." );
		return( false );
	}
	
	L = lua_open();

	if( !L ) {
		LogMsg(WARN, "Could not initialize Lua VM." );
		return( false );
	}

	luaL_openlibs( L );

	RegisterFunctions();
	
	luaInitialized = true;
	
	return( true );
}

bool Lua::Close() {
	if( luaInitialized ) {
		lua_close( L );
		L = NULL;
		luaInitialized = false;
	} else {
		LogMsg(WARN, "Cannot deinitialize Lua. It is either not initialized or a script is still loaded." );
		return( false );
	}
	
	return( true );
}

void Lua::RegisterFunctions() {
	lua_atpanic(L, &Lua::ErrorCatch);
}

int Lua::ErrorCatch(lua_State *L) {
	LogMsg(ERR,"Fatal Error in Lua '%s'", lua_tostring(L, lua_gettop(L)));
	Lua::stackDump( L );
	assert(0);
	exit( -500 );
}

void Lua::RegisterGlobal(string name, int value) {
	lua_pushinteger(L, value );
	lua_setglobal(L, name.c_str() );
}

void Lua::RegisterGlobal(string name, float value) {
	lua_pushnumber(L, value );
	lua_setglobal(L, name.c_str() );
}

void Lua::RegisterGlobal(string name, string value) {
	lua_pushstring(L, value.c_str() );
	lua_setglobal(L, name.c_str() );
}

void Lua::setField(const char* index, int value) {
	lua_pushstring(L, index);
	lua_pushinteger(L, value);
	lua_settable(L, -3);
}

void Lua::setField(const char* index, float value) {
	lua_pushstring(L, index);
	lua_pushnumber(L, value);
	lua_settable(L, -3);
}

void Lua::setField(const char* index, const char* value) {
	lua_pushstring(L, index);
	lua_pushstring(L, value);
	lua_settable(L, -3);
}

int Lua::getIntField(int index, const char* name) {
	int val;
	assert(lua_istable(L,index));
	lua_pushstring(L, name);
	assert(lua_istable(L,index));
	lua_gettable(L,index);
	val = luaL_checkint(L,lua_gettop(L));
	lua_pop(L,1);
	return val;
}

float Lua::getNumField(int index, const char* name) {
	float val;
	assert(lua_istable(L,index));
	lua_pushstring(L, name);
	assert(lua_istable(L,index));
	lua_gettable(L, index);
	val = static_cast<float>(luaL_checknumber(L,lua_gettop(L)));
	lua_pop(L,1);
	return val;
}

string Lua::getStringField(int index, const char* name) {
	string val;
	assert(lua_istable(L,index));
	lua_pushstring(L, name);
	assert(lua_istable(L,index));
	lua_gettable(L, index);
	val = luaL_checkstring(L,lua_gettop(L));
	lua_pop(L,1);
	return val;
}

void Lua::pushStringList(lua_State *L, list<string> *names){
	lua_createtable(L, names->size(), 0);
	int newTable = lua_gettop(L);
	int index = 1;
	list<string>::const_iterator iter = names->begin();
	while(iter != names->end()) {
		lua_pushstring(L, (*iter).c_str());
		lua_rawseti(L, newTable, index);
		++iter;
		++index;
	}
}

list<string> Lua::getStringListField(int index) {
	list<string> results;

	// Go to the nil element of the array
	lua_pushnil(L);
	// While there are elements in the array
	// push the "next" one to the top of the stack
	while(lua_next(L, index)) {
		string val =  lua_tostring(L, -1);
		results.push_back(val);
		// Pop off this value
		lua_pop(L, 1);
	}
	return results;
}

list<string> Lua::getStringListField(int index, const char* name) {
	list<string> results;

	// Get the value from lua_stack[index][name]
	lua_pushstring(L, name);
	lua_gettable(L, index);
	// This needs to be a list of strings
	int stringTable = lua_gettop(L);
	assert(lua_istable(L,stringTable));

	// Go to the nil element of the array
	lua_pushnil(L);
	// While there are elements in the array
	// push the "next" one to the top of the stack
	while(lua_next(L, stringTable)) {
		string val =  lua_tostring(L, -1);
		results.push_back(val);
		// Pop off this value
		lua_pop(L, 1);
	}
	return results;
}

xmlNodePtr Lua::ConvertToXML( lua_State *L, int value_index, int key_index) {
	int t;
	char buff[1024];
	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST "value");

	// Using lua_tostring will convert a value into a string.
	// Never use this on a table key.
	lua_pushvalue(L, key_index);
	t = lua_type(L, key_index);

	// Save the Key and keytype
	xmlSetProp( section, BAD_CAST "key", BAD_CAST lua_tostring(L, lua_gettop(L)) );
	xmlSetProp( section, BAD_CAST "keytype", BAD_CAST lua_typename(L, t));

	lua_pop(L,1); // Pop the copied key now that we're done with it.

	// Save the type
	t = lua_type(L, value_index);
	xmlSetProp( section, BAD_CAST "type", BAD_CAST lua_typename(L, t));

	// Save the value
	switch (t) {
	    case LUA_TBOOLEAN:
			xmlNodeSetContent( section, BAD_CAST (lua_toboolean(L, value_index) ? "true" : "false"));
			break;
	    case LUA_TNUMBER:
			snprintf(buff, sizeof(buff), "%f", lua_tonumber(L, value_index) );
			xmlNodeSetContent( section, BAD_CAST buff);
			break;
	    case LUA_TSTRING:
			xmlNodeSetContent( section, BAD_CAST lua_tostring(L, value_index) );
			break;
	    case LUA_TTABLE:
			lua_pushnil(L);
			while(lua_next(L, value_index)) {
				xmlAddChild( section, Lua::ConvertToXML(L, lua_gettop(L), lua_gettop(L)-1));

				// Pop off this value
				lua_pop(L, 1);
			}
			
			break;
	    case LUA_TNIL:
	    case LUA_TLIGHTUSERDATA:
	    case LUA_TFUNCTION:
	    case LUA_TUSERDATA:
	    case LUA_TTHREAD:
			xmlAddChild( section, xmlNewComment( BAD_CAST "Cannot convert to XML"));
			break;
	    default:
		    assert(0);
		    break;
	}
	return section;
}

int Lua::ConvertFromXML( lua_State *L, xmlDocPtr doc, xmlNodePtr node )
{
	string key;
	string type;
	string keytype;

	keytype = (const char *)xmlGetProp(node, BAD_CAST "keytype");
	type    = (const char *)xmlGetProp(node, BAD_CAST "type");

	// Process the Key
	if(keytype == "number") {
		lua_pushnumber(L, atof( (const char *)xmlGetProp(node, BAD_CAST "key") ) );
	} else if(keytype == "string") {
		lua_pushstring(L, (const char *)xmlGetProp(node, BAD_CAST "key"));
	} else {
		LogMsg(WARN, "Unknown Lua/XML conversion type '%s'", type.c_str());
		return 0;
	}

	// Process the Value
	if( type == "number"){
		float value = NodeToFloat(doc,node);
		lua_pushnumber(L, value);
	} else if( type == "string"){
		string value = NodeToString( doc, node );
		lua_pushstring(L, value.c_str());
	} else if( type == "boolean"){
		string value = NodeToString( doc, node );
		if (value == "true") {
			lua_pushboolean(L, 1);
		} else if (value == "false") {
			lua_pushboolean(L, 0);
		} else {
			LogMsg(WARN, "Unknown value '%s' for type '%s'", value.c_str(), type.c_str());
			assert(0);
		}

	} else if( type == "table"){
		int tableIndex;
		lua_newtable(L);
		tableIndex = lua_gettop(L);
		// Create a table
		for( node = FirstChildNamed(node, "value"); node!=NULL; node = NextSiblingNamed(node, "value") ){
			Lua::ConvertFromXML(L, doc, node);
			assert(lua_istable(L, tableIndex));
			lua_settable(L, tableIndex); // Pops the name and value.
			assert( tableIndex == lua_gettop(L) ); // The table should be at the top now.
		}
	} else {
		LogMsg(WARN, "Unknown Lua/XML conversion type '%s'", type.c_str());
		return 1; // Just Key
	}
	return 2; // Key, Value
}

//can be found here  http://www.lua.org/pil/24.2.3.html
void Lua::stackDump(lua_State *L) {
	int i;
	int top = lua_gettop(L);
	for (i = 1; i <= top; i++) {  /* repeat for each level */
		int t = lua_type(L, i);
		switch (t) {

			case LUA_TSTRING:  /* strings */
				printf("[%d]`%s'", i, lua_tostring(L, i));
				break;

			case LUA_TBOOLEAN:  /* booleans */
				printf("[%d]%s", i, lua_toboolean(L, i) ? "true" : "false");
				break;

			case LUA_TNUMBER:  /* numbers */
				printf("[%d]%g", i, lua_tonumber(L, i));
				break;

			default:  /* other values */
				printf("[%d]%s", i, lua_typename(L, t));
				break;

		}
		printf("  ");  /* put a separator */
	}
	printf("\n");  /* end the listing */
}
