/*	=====
 *	ps.cc
 *	=====
 */

// POSIX
#include <dirent.h>

// Standard C
#include <stdlib.h>
#include <string.h>

// Standard C++
#include <algorithm>

// iota
#include "iota/strings.hh"

// command
#include "command/get_option.hh"

// gear
#include "gear/parse_decimal.hh"
#include "gear/parse_float.hh"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/ftruncate.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/openat.hh"
#include "poseven/functions/pwrite.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


using namespace command::constants;

enum
{
	Option_last_byte = 255,
	
	Option_monitor,
	Option_sleep,
	Option_wide,
};

static command::option options[] =
{
	{ "monitor", Option_monitor               },
	{ "sleep",   Option_sleep, Param_required },
	{ "wide",    Option_wide                  },
	
	{ NULL }
};

static bool globally_wide = false;
static bool monitor = false;

static float min_sleep = 1.0;

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_sleep:
				min_sleep = gear::parse_float( command::global_result.param );
				
				// fall through
			
			case Option_monitor:
				monitor = true;
				break;
			
			case Option_wide:
				globally_wide = true;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}

static struct timespec timespec_from_seconds( float time )
{
	const unsigned long seconds     = time;
	const unsigned long nanoseconds = (time - seconds) * 1000 * 1000 * 1000;
	
	const struct timespec result = { seconds, nanoseconds };
	
	return result;
}

namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	static p7::fd_t g_proc = p7::open( "/proc", p7::o_rdonly | p7::o_directory ).release();
	
	
	static inline bool is_ascii( signed char c )
	{
		return c >= 0;
	}
	
	static plus::string left_padded( const char* begin, const char* end, unsigned length )
	{
		length = std::max< unsigned >( length, end - begin );
		
		const size_t pad_length = length - (end - begin);
		
		plus::var_string result;
		
		result.reserve( length );
		
		result.resize( pad_length, ' ' );
		
		result.append( begin, end );
		
		return result;
	}
	
	static plus::string right_padded( const char* begin, const char* end, unsigned length )
	{
		length = std::max< unsigned >( length, end - begin );
		
		const size_t pad_length = length - (end - begin);
		
		plus::var_string result;
		
		result.reserve( length );
		
		result.assign( begin, end );
		
		result.append( pad_length, ' ' );
		
		return result;
	}
	
	static void report_process( plus::var_string& report, const char* pid_name )
	{
		const size_t incoming_report_size = report.size();
		
		n::owned< p7::fd_t > proc_pid = p7::openat( g_proc, pid_name, p7::o_rdonly | p7::o_directory );
		
		char buffer[ 4096 ];
		
		const char* begin = buffer;
		const char* end   = buffer + p7::read( p7::openat( proc_pid, "stat", p7::o_rdonly ), buffer, 4096 );
		
		const char* close_paren = std::find( begin, end, ')' );
		
		ASSERT( close_paren != end );
		
		const char* p_stat = close_paren + 2;
		
		const char* q_stat = std::find( p_stat, end, ' ' );
		
		plus::var_string stat_string( p_stat, q_stat );
		
		const char* p_ppid = q_stat + 1;
		
		const char* q_ppid = std::find( p_ppid, end, ' ' );
		
		const char* p_pgid = q_ppid + 1;
		
		const char* q_pgid = std::find( p_pgid, end, ' ' );
		
		const char* p_sid = q_pgid + 1;
		
		const char* q_sid = std::find( p_sid, end, ' ' );
		
		const char* p_termname = q_sid + 1;
		
		const char* q_termname = std::find( p_termname, end, ' ' );
		
		const char* p_tpgid = q_termname + 1;
		
		pid_t pid = gear::parse_unsigned_decimal( pid_name );
		
	//	pid_t ppid  = gear::parse_unsigned_decimal( p_ppid  );
		pid_t pgid  = gear::parse_unsigned_decimal( p_pgid  );
		pid_t sid   = gear::parse_unsigned_decimal( p_sid   );
		pid_t tpgid = gear::parse_unsigned_decimal( p_tpgid );
		
		if ( q_termname - p_termname >= STRLEN( "/gui/port/_/tty" ) )
		{
			p_termname += STRLEN( "/gui/port/"        );
			q_termname -= STRLEN(              "/tty" );
		}
		else if ( q_termname - p_termname > STRLEN( "/dev/" ) )
		{
			p_termname += STRLEN( "/dev/" );
		}
		
		if ( pid == sid )
		{
			stat_string += 's';
		}
		
		if ( pgid == tpgid )
		{
			stat_string += '+';
		}
		
		report += left_padded( pid_name, pid_name + strlen( pid_name ), 5 );
		
		report += " ";
		
		report += right_padded( p_termname, q_termname, 8 );
		
		report += " ";
		
		report += right_padded( stat_string.begin(), stat_string.end(), 4 );
		
		report += "  ";
		
		report += left_padded( p_ppid, q_ppid, 5 );
		
		report += "  ";
		
		report += left_padded( p_pgid, q_pgid, 5 );
		
		report += "  ";
		
		report += left_padded( p_sid, q_sid, 5 );
		
		report += "  ";
		
		char* cmdline_end = buffer + p7::read( p7::openat( proc_pid, "cmdline", p7::o_rdonly ), buffer, 4096 );
		
		if ( cmdline_end > buffer )
		{
			std::replace( buffer, cmdline_end - 1, '\0', ' ' );  // replace NUL with space except last
			
			report.append( buffer, cmdline_end - 1 );
		}
		
		if ( !globally_wide  &&  report.size() > incoming_report_size + 80 )
		{
			size_t new_size = incoming_report_size + 80;
			
			if ( !is_ascii( report[ new_size ] ) )
			{
				do {} while ( !is_ascii( report[ --new_size ] ) );
			}
			
			report.resize( new_size );
		}
		
		report += "\n";
	}
	
	static plus::string ps()
	{
		plus::var_string output = "  PID TERM     STAT   PPID   PGID    SID  COMMAND\n";
		
		DIR* iter = opendir( "/proc" );
		
		while ( const dirent* ent = readdir( iter ) )
		{
			if ( pid_t pid = gear::parse_unsigned_decimal( ent->d_name ) )
			{
				// A process could exit while we're examining it
				try
				{
					 report_process( output, ent->d_name );
				}
				catch ( ... )
				{
				}
			}
		}
		
		closedir( iter );
		
		return output;
	}
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		const struct timespec minimum = timespec_from_seconds( min_sleep );
		
		if ( !monitor )
		{
			p7::write( p7::stdout_fileno, ps() );
			
			return 0;
		}
		
		plus::string output;
		plus::string previous;
		
	again:
		
		output = ps();
		
		if ( output != previous )
		{
			if ( monitor )
			{
				p7::ftruncate( p7::stdout_fileno, 0 );
				
				p7::pwrite( p7::stdout_fileno, output, 0 );
				
				using std::swap;
				
				swap( output, previous );
			}
		}
		
		if ( monitor )
		{
			int dozed = doze( &minimum, NULL, NULL );  // min, max, remaining
			
			goto again;
		}
		
		return 0;
	}
	
}
