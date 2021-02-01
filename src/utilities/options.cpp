/**\file			options.cpp
 * \author			Matt Zweig
 * \date			Created:  Sunday, May 29, 2011
 * \date			Modified: Saturday, December 26, 2020
 * \brief			Global Options
 * \details
 */

#include "includes.h"
#include "utilities/log.h"
#include "utilities/options.h"

std::map<string,string> Options::values;
std::map<string,string> Options::defaults;
bool Options::isLoaded = false;

/**\class Options
 * \brief Container and accessor of Game options
 *
 */

/**
 * Load game options stored on filesystem, if any.
 */
void Options::Load( const string& path ) {
	// Load existing options, if file exists
	XMLFile *optionsfile = new XMLFile();

	LogMsg(DEBUG, "Loading options from %s, if exists", path.c_str());

	if( optionsfile->Open( path ) ) {
		// Existing options are stored. Use them to override defaults.
		LogMsg(DEBUG, "Found options stored on disk. Overriding defaults ...");

		for(std::map<string,string>::iterator iter = defaults.begin(); iter != defaults.end(); ++iter) {
			string key = iter->first;
			string file_value = optionsfile->Get(key);

			if( file_value.empty() == false ) {
				LogMsg(DEBUG, "Overriding default option with value from file: %s %s -> %s", key.c_str(), defaults[key].c_str(), file_value.c_str());
				values[key] = file_value;
			}
		}
	} else {
		LogMsg(DEBUG, "Did not find options stored on disk. Will use defaults combined with any CLI arguments.");
	}

	delete optionsfile;

	Options::isLoaded = true;
}

/**
 * Sets in-memory defaults for Options().
 * Values can be overriden if options are loaded from file or set via command line switch.
 */
void Options::Initialize( void ) {
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
	defaults.insert( std::pair<string,string>("options/sound/engines", "1") );
	defaults.insert( std::pair<string,string>("options/sound/explosions", "1") );
	defaults.insert( std::pair<string,string>("options/sound/buttons", "1") );

	// Simultaion
	defaults.insert( std::pair<string,string>("options/scenario/automatic-load", "0") );

	// Timing
	defaults.insert( std::pair<string,string>("options/timing/mouse-fade", "500") );
	defaults.insert( std::pair<string,string>("options/timing/target-zoom", "500") );
	defaults.insert( std::pair<string,string>("options/timing/alert-drop", "7500") );
	defaults.insert( std::pair<string,string>("options/timing/alert-fade", "4500") );

	// Development
	defaults.insert( std::pair<string,string>("options/development/debug-ai", "0") );
	defaults.insert( std::pair<string,string>("options/development/debug-ui", "0") );

	values = defaults;
}

bool Options::Save( const string& path ) {
	if( defaults == values ) return true; // nothing to save

	for(std::map<string,string>::iterator iter = values.begin(); iter != values.end(); ++iter) {
		string key = iter->first;
		
	}
	
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
	values = defaults;
}

string Options::Get( const string& path ) {
	std::map<string,string>::iterator it;

	it = values.find(path);

	if(it == values.end()) {
		LogMsg(DEBUG, "Could not Options::Get() for path '%s'", path.c_str());

		//for(std::map<string,string>::iterator iter = values.begin(); iter != values.end(); ++iter) {
		//	string key = iter->first;
		//	cout << "Valid key: " << key << endl;
		//}

		return "";
	}

	return it->second;
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

