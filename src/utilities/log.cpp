/**\file			log.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Sunday, June 4, 2006
 * \date			Modified: Saturday, December 26, 2020
 * \brief			Main logging facilities for the codebase
 * \details
 */

#include "includes.h"
#include "common.h"
#include "utilities/log.h"
#include "engine/hud.h"

/**\class Log
 * \brief Main logging facilities for the code base. */

/**\brief Empty destructor.*/
Log::~Log() {
}

/**\brief Retrieves the current instance of the log class.*/
Log& Log::Instance( void ) {
	static Log _instance;
	return _instance;
}

/**\brief Allows changing of the log level dynamically (string version).*/
bool Log::SetLevel( const string& _loglvl ) {
	// Check logging level
	this->loglvl = this->ReverseLookUp( _loglvl );

	if( this->loglvl == INVALID ) {
		LogMsg(WARN, "Invalid logging level specified, reverting to default log level.");
		this->loglvl = this->loglvldefault;

		return false;
	}

	return true;
}

/**\brief Allows changing of the log level dynamically ( enum version ).*/
bool Log::SetLevel( LogLevel _loglvl ){
	LogMsg(DEBUG, "Changing Log Level from '%s' to '%s'.", lvlStrings[loglvl].c_str(), lvlStrings[_loglvl].c_str());

	this->loglvl = _loglvl;

	return true;
}

/**\brief Changes the function filter.*/
void Log::SetFuncFilter( const string& _funcfilter ) {
	this->funcfilter.clear();

	// Check function filter
	if( _funcfilter.empty() == false ) {
		LogMsg(DEBUG, "Filtering log by function named: %s.", _funcfilter.c_str());
		this->funcfilter = _funcfilter;
	}
}

/**\brief Changes the message filter.*/
void Log::SetMsgFilter( const string& msgfilter ){
	this->filter.clear();
	// Check message filter
	if( !msgfilter.empty() ){
		LogMsg(DEBUG, "Filter log by message text: %s", msgfilter.c_str());
		this->filter=msgfilter;
	}
}


/**\brief Opens the log file.*/
void Log::Open() {
	// open the debug file
	fp = fopen( logFilename.c_str(), "wb" );
	if( !fp ) {
		fprintf( stderr, "Could not open \"%s\" for debugging!", logFilename.c_str() );
	} else {
		// Write the xml header
		fprintf(fp, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"	);
		fprintf(fp, "<?xml-stylesheet type=\"text/xsl\" href=\"epiar.xsl\"?>\n" );
		fprintf(fp, "<debugSession time=\"%s\">\n", timestamp );
	}	
}

/**\brief Frees the handle to the log file.*/
void Log::Close( void ) {
	if( fp ) {
		fprintf(fp, "</debugSession>\n");
		fclose( fp );
	}
	fp = NULL;
}

/**\brief The _real_ log function.
 * \todo The filtering is broken and should be refactored.
 * \details Note: If Options::Load() hasn't been called, messages are queued but not logged.
 *                This is only an issue if debugging an issue that occurs during program init.
 */

// FIXME: since command line args are parsed after options are loaded, the default option of not logging out
// is not respected for the buffered log messages

void Log::realLog( LogLevel messageLevel, const string& func, const char *message, ... ) {
	// Compose LogEntry based on passed information
	va_list args;
	time_t rawtime;
	char logBuffer[1024] = {0};

	time( &rawtime );

	timestamp = ctime( &rawtime );
	timestamp[ strlen(timestamp) - 1 ] = 0;

	va_start( args, message );
	vsnprintf( logBuffer, sizeof(logBuffer), message, args );
	va_end( args );

	// Trim the final '\n' if necessary
	if( logBuffer[ strlen(logBuffer) - 1 ] == '\n' ) logBuffer[ strlen(logBuffer) - 1 ] = 0;

	LogEntry entry = LogEntry(func, messageLevel, logBuffer);

	// Buffer message for later processing or process right away, depending on flags
	if(useBuffer) {
		logEntryBuffer.push( entry );
	} else {
		processLogEntry( entry );
	}
}

/**
 * Prints a log entry whether to console, file, etc.
 */
void Log::processLogEntry( LogEntry entry ) {
	// Check log level
	if( entry.lvl < this->loglvl ) {
		return;
	}

	// Check function filter
	if( !this->funcfilter.empty() ){
		if( entry.func.find(this->funcfilter) == false ) {
			return;
		}
	}

	if( OPTION(int, "options/log/out") == 1 ) {
#ifndef _WIN32
		StartTermColor( entry.lvl );
#endif
		cout << entry.func << " (" << lvlStrings[entry.lvl] << ") - " << entry.message << endl;
#ifndef _WIN32
		EndTermColor( entry.lvl );
#endif
	}

	if( OPTION(int, "options/log/alert") == 1 ) {
		Hud::Alert(false, "%s - %s", lvlStrings[entry.lvl].c_str(), entry.message.c_str());
	}
	
	// Save the message to a file
	if( OPTION(int, "options/log/xml") == 1 ) {
		if( fp == NULL ) { Log::Open(); }

		fprintf(fp, "<log>\n");
		fprintf(fp, "\t<function>%s</function>\n", entry.func.c_str() );
		fprintf(fp, "\t<type>%s</type>\n", lvlStrings[entry.lvl].c_str() );
		fprintf(fp, "\t<time>%s</time>\n", timestamp );
		fprintf(fp, "\t<message>%s</message>\n", entry.message.c_str() );
		fprintf(fp, "</log>\n" );
		fflush( fp );
	}
}

/**
 * Prints all messages in the buffer and empties the buffer.
 */
void Log::flushBuffer() {
	while(logEntryBuffer.empty() == false) {
		LogEntry entry = logEntryBuffer.front();
		logEntryBuffer.pop();

		processLogEntry(entry);
	}
}

/**\brief Constructor, used to initialize variables.*/
Log::Log()
	:loglvldefault(DEBUG)
{
	lvlStrings[NONE] = "None";
	lvlStrings[FATAL] = "Fatal";
	lvlStrings[ERR] = "Error";
	lvlStrings[WARN] = "Warn";
	lvlStrings[INFO] = "Info";
	lvlStrings[DEBUG] = "Debug";

	loglvl = loglvldefault;

#ifndef _WIN32
	int Black   = 30;
	//int Blue    = 34;
	int Green   = 32;
	int Cyan    = 36;
	int Red     = 31;
	//int Purple  = 35;
	int Brown   = 33;

	// BLACK
	colors[NONE]    = Black;
	// Red
	colors[FATAL]   = Red;
	colors[ERR]     = Red;
	// Brown
	colors[WARN]    = Brown;
	// Cyan
	colors[INFO]    = Cyan;
	// Green
	colors[DEBUG]  = Green;
	
	istty = isatty(fileno(stdin));
#endif

	// generate the log's filename based on the time
	logFilename = string("Epiar-Log-") + GetTimestamp() + string(".xml");

	fp = NULL;
}

string Log::GetTimestamp( void ) {
	time_t rawtime;
	string timestamp_str;
	char *timestamp;

	time( &rawtime );
	timestamp = ctime( &rawtime );
	timestamp[ strlen(timestamp) - 1 ] = 0;

	// Need to replace ":" with "_" because windows can't handle it
	timestamp[ 13 ] ='_';
	timestamp[ 16 ] ='_';

	// generate the log's filename based on the time
	timestamp_str = string(timestamp);

	return timestamp_str;
}

/**\brief Does a reverse lookup of the log level based on a string.*/
LogLevel Log::ReverseLookUp( const string& _lvl ) {
	// Figure out which log level we're doing.
	map<LogLevel,string>::iterator it;

	for ( it = lvlStrings.begin() ; it != lvlStrings.end(); it++ ){
		if( (*it).second == _lvl )
			return (*it).first;
	}

	return INVALID;
}

