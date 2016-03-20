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
std::map<string,string> Options::defaults;

/**\class Options
 * \brief Container and accessor of Game options
 *
 */

void Options::Initialize( const string& path ) {
	SetDefaults();

	values = defaults;

	// Load existing options, if file exists
	optionsfile = new XMLFile();

	if( optionsfile->Open( path ) ) {
		// Existing options are stored. Use them to override defaults.

		for(std::map<string,string>::iterator iter = defaults.begin(); iter != defaults.end(); ++iter) {
			string key = iter->first;
			string file_value = optionsfile->Get(key);

			if( file_value.empty() == false ) {
				values[key] = file_value;
			}
		}

	}
}

void Options::SetDefaults( void ) {
	// Logging
	defaults.insert( std::pair<string,string>("options/log/xml", "0") );
	defaults.insert( std::pair<string,string>("options/log/out", "0") );
	defaults.insert( std::pair<string,string>("options/log/alert", "0") );
	defaults.insert( std::pair<string,string>("options/log/ui", "0") );
	defaults.insert( std::pair<string,string>("options/log/sprites", "0") );

	// Video
	defaults.insert( std::pair<string,string>("options/video/w", "1024") );
	defaults.insert( std::pair<string,string>("options/video/h", "768") );
	defaults.insert( std::pair<string,string>("options/video/bpp", "32") );
	defaults.insert( std::pair<string,string>("options/video/fullscreen", "0") );
	defaults.insert( std::pair<string,string>("options/video/fps", "60") );

	// Sound
	defaults.insert( std::pair<string,string>("options/sound/disable-audio", "0") );
	defaults.insert( std::pair<string,string>("options/sound/musicvolume", "0.50") );
	defaults.insert( std::pair<string,string>("options/sound/soundvolume", "0.50") );
	defaults.insert( std::pair<string,string>("options/sound/background", "1") );
	defaults.insert( std::pair<string,string>("options/sound/weapons", "1") );
	defaults.insert( std::pair<string,string>("options/sound/engine", "1") );
	defaults.insert( std::pair<string,string>("options/sound/explosions", "1") );
	defaults.insert( std::pair<string,string>("options/sound/buttons", "1") );

	// Simultaion
	defaults.insert( std::pair<string,string>("options/scenario/automatic-load", "0") );

	// Timing
	defaults.insert( std::pair<string,string>("options/timing/mouse-fade", "500") );
	defaults.insert( std::pair<string,string>("options/timing/target-zoom", "500") );
	defaults.insert( std::pair<string,string>("options/timing/alert-drop", "3500") );
	defaults.insert( std::pair<string,string>("options/timing/alert-fade", "2500") );

	// Development
	defaults.insert( std::pair<string,string>("options/development/debug-ai", "0") );
	defaults.insert( std::pair<string,string>("options/development/debug-ui", "0") );
}

bool Options::IsLoaded() {
	return( optionsfile != NULL );
}

bool Options::Save( const string& path ) {
	// TODO: Save only if values differ from defaults or optionsfile already exists.
	LogMsg(WARN, "Cannot save options - unimplemented.." );

	return false;

	//if( path == "" ) {
	//	return optionsfile->Save();
	//} else {
	//	return optionsfile->Save( path );
	//}
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

