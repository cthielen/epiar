/**\file			options.h
 * \author			Matt Zweig
 * \date			Created:  Sunday, May 29, 2011
 * \date			Modified: Sunday, May 29, 2011
 * \brief			Global Options
 * \details
 */

#ifndef __H_OPTIONS
#define __H_OPTIONS

#include "utilities/xmlfile.h"
#include "utilities/string_convert.h"

// To simply access options
#define OPTION(T, path) ( convertTo<T>( Options::Get(path) ) )
#define SETOPTION(path, value) (Options::Set((path),(value)) )

class Options {
	public:
		static void Initialize( void );
		static void Load( const string& path );

		static bool IsLoaded( void ) { return isLoaded; };

		static bool Save( const string& path = "" );

		static void RestoreDefaults();

		static string Get( const string& path );

		static void Set( const string& path, const string& value );
		static void Set( const string& path, const float value );
		static void Set( const string& path, const int value );

	private:
		static std::map<string,string> defaults;
		static std::map<string,string> values;
		static bool isLoaded;
};

#endif // __H_OPTIONS
