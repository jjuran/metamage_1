/*
	report.hh
	---------
*/

#ifndef LOGOFWAR_REPORT_HH
#define LOGOFWAR_REPORT_HH

// log-of-war
#include "logofwar/levels.hh"
#include "logofwar/print.hh"


namespace logofwar
{
	
	extern bool verbose_reports;
	
	void report( diagnostic_level level, const char* file, int line );
	
	struct line
	{
		~line()  { print( "\n" ); }
	
		template < class T > line& operator=(T x)  { print( x ); return *this; }
		template < class T > line& operator,(T x)  { print( x ); return *this; }
	};
	
}

#define REPORT( level )  \
	if ( (level) <= ::logofwar::maximum_verbosity  \
		&&  (level) <= ::logofwar::current_verbosity  \
		&&  (report( (level), __FILE__, __LINE__ ), true) )  ::logofwar::line()

#define FATAL    REPORT( ::logofwar::Level_fatal   )
#define ERROR    REPORT( ::logofwar::Level_error   )
#define WARNING  REPORT( ::logofwar::Level_warning )
#define NOTICE   REPORT( ::logofwar::Level_notice  )
#define INFO     REPORT( ::logofwar::Level_info    )
#define DEBUG    REPORT( ::logofwar::Level_debug   )

#endif
