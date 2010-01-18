/*	=====
 *	ps.cc
 *	=====
 */

// Standard C++
#include <string>

// Standard C
#include <stdio.h>

// POSIX
#include <dirent.h>

// iota
#include "iota/decimal.hh"
#include "iota/strings.hh"

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/Pathnames.hh"
#include "poseven/functions/ftruncate.hh"
#include "poseven/functions/lseek.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/openat.hh"
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
	
	
	static std::string left_padded( const char* begin, const char* end, unsigned length )
	{
		int pad_length = std::max< int >( length - (end - begin), 0 );
		
		std::string result( pad_length, ' ' );
		
		result.append( begin, end );
		
		return result;
	}
	
	static std::string right_padded( const char* begin, const char* end, unsigned length )
	{
		int pad_length = std::max< int >( length - (end - begin), 0 );
		
		std::string result( begin, end );
		
		result.append( pad_length, ' ' );
		
		return result;
	}
	
	static inline std::string left_padded( const std::string& word, unsigned length )
	{
		return left_padded( &*word.begin(), &*word.end(), length );
	}
	
	static inline std::string right_padded( const std::string& word, unsigned length )
	{
		return right_padded( &*word.begin(), &*word.end(), length );
	}
	
	static std::string report_process( const std::string& pid_name )
	{
		using namespace io::path_descent_operators;
		
		n::owned< p7::fd_t > proc_pid = p7::openat( g_proc, pid_name, p7::o_rdonly | p7::o_directory );
		
		char buffer[ 4096 ];
		
		const char* begin = buffer;
		const char* end   = buffer + p7::read( p7::openat( proc_pid, "stat", p7::o_rdonly ), buffer, 4096 );
		
		const char* close_paren = std::find( begin, end, ')' );
		
		ASSERT( close_paren != end );
		
		const char* p_stat = close_paren + 2;
		
		const char* space = std::find( p_stat, end, ' ' );
		
		std::string stat_string( p_stat, space );
		
		const char* p_ppid = space + 1;
		
		space = std::find( p_ppid, end, ' ' );
		
		std::string ppid_string( p_ppid, space );
		
		const char* p_pgid = space + 1;
		
		space = std::find( p_pgid, end, ' ' );
		
		std::string pgid_string( p_pgid, space );
		
		const char* p_sid = space + 1;
		
		space = std::find( p_sid, end, ' ' );
		
		std::string sid_string( p_sid, space );
		
		const char* p_termname = space + 1;
		
		space = std::find( p_termname, end, ' ' );
		
		std::string term_string( p_termname, space );
		
		const char* p_tpgid = space + 1;
		
		space = std::find( p_tpgid, end, ' ' );
		
		std::string tpgid_string( p_tpgid, space );
		
		pid_t pid = iota::parse_unsigned_decimal( pid_name.c_str() );
		
		pid_t ppid  = iota::parse_unsigned_decimal( p_ppid  );
		pid_t pgid  = iota::parse_unsigned_decimal( p_pgid  );
		pid_t sid   = iota::parse_unsigned_decimal( p_sid   );
		pid_t tpgid = iota::parse_unsigned_decimal( p_tpgid );
		
		if ( term_string.length() == STRLEN( "/sys/window/12345678/tty" ) )
		{
			term_string.erase( STRLEN( "/sys/window/1234567" ), STRLEN( "0/tty" ) );
			
			term_string.erase( 0, STRLEN( "/sys/window/" ) );
		}
		else
		{
			term_string.erase( 0, STRLEN( "/dev/" ) );
		}
		
		if ( pid == sid )
		{
			stat_string += 's';
		}
		
		if ( pgid == tpgid )
		{
			stat_string += '+';
		}
		
		std::string report;
		
		report += left_padded( pid_name, 5 );
		
		report += " ";
		
		report += right_padded( term_string, 7 );
		
		report += " ";
		
		report += right_padded( stat_string, 4 );
		
		report += "  ";
		
		report += left_padded( ppid_string, 5 );
		
		report += "  ";
		
		report += left_padded( pgid_string, 5 );
		
		report += "  ";
		
		report += left_padded( sid_string, 5 );
		
		report += "  ";
		
		char* cmdline_end = buffer + p7::read( p7::openat( proc_pid, "cmdline", p7::o_rdonly ), buffer, 4096 );
		
		if ( cmdline_end > buffer )
		{
			std::replace( buffer, cmdline_end - 1, '\0', ' ' );  // replace NUL with space except last
			
			report.append( buffer, cmdline_end - 1 );
		}
		
		if ( !globally_wide  &&  report.size() > 80 )
		{
			report.resize( 80 );
		}
		
		report += "\n";
		
		return report;
	}
	
	static std::string ps()
	{
		std::string output = "  PID TERM    STAT   PPID   PGID    SID  COMMAND\n";
		
		DIR* iter = opendir( "/proc" );
		
		while ( const dirent* ent = readdir( iter ) )
		{
			if ( pid_t pid = iota::parse_unsigned_decimal( ent->d_name ) )
			{
				// A process could exit while we're examining it
				try
				{
					output += report_process( ent->d_name );
				}
				catch ( ... )
				{
				}
			}
		}
		
		closedir( iter );
		
		return output;
	}
	
	int Main( int argc, iota::argv_t argv )
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
			
			int scanned = sscanf( sleep_arg, "%f", &min_sleep );
		}
		
		const struct timespec minimum = timespec_from_seconds( min_sleep );
		
		std::string output;
		std::string previous;
		
	again:
		
		output = ps();
		
		if ( output != previous )
		{
			p7::write( p7::stdout_fileno, output );
			
			if ( monitor )
			{
				p7::ftruncate( p7::stdout_fileno, output.size() );
				
				std::swap( output, previous );
				
				p7::lseek( p7::stdout_fileno, 0 );
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

