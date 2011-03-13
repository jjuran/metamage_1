/*	=====
 *	ps.cc
 *	=====
 */

// POSIX
#include <dirent.h>

// iota
#include "iota/strings.hh"

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
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


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
	namespace o = orion;
	
	
	static bool globally_wide = false;
	
	static p7::fd_t g_proc = p7::open( "/proc", p7::o_rdonly | p7::o_directory ).release();
	
	
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
		
		if ( q_termname - p_termname == STRLEN( "/gui/port/12345678/tty" ) )
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
			report.resize( incoming_report_size + 80 );
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
		bool monitor = false;
		
		const char* sleep_arg = NULL;
		
		o::bind_option_to_variable( "--monitor", monitor );
		
		o::bind_option_to_variable( "--wide", globally_wide );
		
		o::bind_option_to_variable( "--sleep", sleep_arg );
		
		o::get_options( argc, argv );
		
		float min_sleep = 1.0;
		
		if ( sleep_arg )
		{
			monitor = true;
			
			min_sleep = gear::parse_float( sleep_arg );
		}
		
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
				p7::ftruncate( p7::stdout_fileno, output.size() );
				
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

