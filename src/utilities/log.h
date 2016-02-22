/**\file			log.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Sunday, June 4, 2006
 * \date			Modified: Wednesday, December 16, 2015
 * \brief			Main logging facilities for the codebase
 * \details
 */

#ifndef __H_LOG__
#define __H_LOG__

#include "includes.h"

// Work around for PRETTY_FUNCTIONs
#ifndef __GNUC__
	#if defined(_MSC_VER)
		#define __PRETTY_FUNCTION__ __FUNCSIG__
	#else
		#define __PRETTY_FUNCTION__ __FUNCTION__
	#endif
#endif

// We use macros to define logging facilities so that we can
// compile out Logging facilities if needed for performance
//#define DISABLE_LOGGING // Do not log anything
//#define DISABLE_LOGGER // Print logs to stdout
#if defined( DISABLE_LOGGING )
	#define LogMsg(LVL,...)
#elif defined( DISABLE_LOGGER )
	#define LogMsg(LVL,...) printf( __VA_ARGS__ )
#else
	#define LogMsg(LVL,...) Log::Instance().realLog(LVL,__PRETTY_FUNCTION__,__VA_ARGS__)
#endif//ENABLE_LOGGING

typedef enum {
	INVALID = 0,		/**< Invalid log level, used for internal purposes.*/
	NONE,			/**< No logging. */
	FATAL,			/**< Will kill the program. */
	DEBUG,			/**< Debug information. */
	INFO,			/**< (System level) Information that the user might want. */
	WARN,			/**< Expected (but should be noted) problems. */
	ERR,			/**< Unexpected errors. */
	ALL             /**< This is always the highest Logging level.*/
} LogLevel;

class Log {
	public:
		~Log();

		static Log& Instance(void);
		void Start( const string& _filter="", const string& _funfilter="" );
		bool SetLevel( const string& _loglvl );
		bool SetLevel( LogLevel _loglvl );
		void SetFuncFilter( const string& _funcfilter );
		void SetMsgFilter( const string& msgfilter );
		void Close( void );
		static string GetTimestamp( void );

		void realLog( LogLevel lvl, const string& func, const char *message, ... );

	private:
		Log();
		Log(Log const&);
		Log& operator=(Log const&);
		void Open( void );
		LogLevel ReverseLookUp( const string& _lvl );

		map<LogLevel,string> lvlStrings;
		LogLevel loglvl;
		LogLevel loglvldefault;

#ifndef _WIN32
		bool istty;
		map<LogLevel,int> colors;
		void StartTermColor( LogLevel lvl ) { if(istty) printf("\e[1;%dm", colors[lvl] ); }
		void EndTermColor( LogLevel lvl ) { if(istty) printf("\e[m"); }
#else
		#define StartTermColor(lvl)
		#define EndTermColor(lvl)
#endif
		
		string filter;				/**< Message filter.*/
		string funcfilter;			/**< Function filter.*/

		char *timestamp;
		string logFilename;
		FILE *fp; // pointer to the log
};

class LogEntry {
	public:
		LogEntry( string func, LogLevel lvl, string message ) {
			this->func = func;
			this->lvl = lvl;
			this->message = message;
		}

		LogLevel lvl;

		string func;
		string message;
};

#endif // __H_LOG__
