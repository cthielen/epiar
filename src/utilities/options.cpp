/**\file			options.cpp
 * \author			Matt Zweig
 * \date			Created:  Sunday, May 29, 2011
 * \date			Modified: Monday, February 22, 2016
 * \brief			Global Options
 * \details
 */

#include "includes.h"
#include "utilities/log.h"
#include "utilities/options.h"

XMLFile *Options::optionsfile = NULL; ///< Static instance of the optionsfile.
list<string> Options::nonPersistentOptions;
std::map<string,string> Options::values;

/**\class Options
 * \brief Container and accessor of Game options
 *
 */

void Options::Initialize( const string& path ) {
	optionsfile = new XMLFile();

	if( !optionsfile->Open( path ) ) {
		// Create the default Options file
		optionsfile->New( "options" );
	}

	assert( false );
	// TODO: Need to set the defaults below, then copy in anything in 'optionsfile', if it exists.

	// Logging
	values.insert( std::pair<string,string>("options/log/xml", "0") );
	values.insert( std::pair<string,string>("options/log/out", "0") );
	values.insert( std::pair<string,string>("options/log/alert", "0") );
	values.insert( std::pair<string,string>("options/log/ui", "0") );
	values.insert( std::pair<string,string>("options/log/sprites", "0") );

	// Video
	values.insert( std::pair<string,string>("options/video/w", "1024") );
	values.insert( std::pair<string,string>("options/video/h", "768") );
	values.insert( std::pair<string,string>("options/video/bpp", "32") );
	values.insert( std::pair<string,string>("options/video/fullscreen", "0") );
	values.insert( std::pair<string,string>("options/video/fps", "60") );

	// Sound
	values.insert( std::pair<string,string>("options/sound/disable-audio", "0") );
	values.insert( std::pair<string,string>("options/sound/musicvolume", "0.50") );
	values.insert( std::pair<string,string>("options/sound/soundvolume", "0.50") );
	values.insert( std::pair<string,string>("options/sound/background", "1") );
	values.insert( std::pair<string,string>("options/sound/weapons", "1") );
	values.insert( std::pair<string,string>("options/sound/engine", "1") );
	values.insert( std::pair<string,string>("options/sound/explosions", "1") );
	values.insert( std::pair<string,string>("options/sound/buttons", "1") );

	// Simultaion
	values.insert( std::pair<string,string>("options/scenario/automatic-load", "0") );

	// Timing
	values.insert( std::pair<string,string>("options/timing/mouse-fade", "500") );
	values.insert( std::pair<string,string>("options/timing/target-zoom", "500") );
	values.insert( std::pair<string,string>("options/timing/alert-drop", "3500") );
	values.insert( std::pair<string,string>("options/timing/alert-fade", "2500") );

	// Development
	values.insert( std::pair<string,string>("options/development/debug-ai", "0") );
	values.insert( std::pair<string,string>("options/development/debug-ui", "0") );
}

bool Options::IsLoaded() {
	return( optionsfile != NULL );
}

bool Options::Save( const string& path ) {
	assert( optionsfile );

	assert( false ); // we need to copy 'values' to 'optionsfile'

	if( path == "" ) {
		return optionsfile->Save();
	} else {
		return optionsfile->Save( path );
	}
}

void Options::RestoreDefaults() {
	assert( false ); // not implemented
	//optionsfile->Copy( defaults );
}

string Options::Get( const string& path ) {
	std::map<string,string>::iterator it;
	it = values.find(path);
	if(it == values.end()) {
		cout << "Could not Options::Get() for path '" << path << "'" << endl;
	}

	return (it->second);
}

void Options::Set( const string& path, const string& value ) {
	values[path] = value;
}

void Options::Set( const string& path, const float value ) {
	values[path] = std::to_string(value);
}

void Options::Set( const string& path, const int value ) {
	values[path] = std::to_string(value);
}

