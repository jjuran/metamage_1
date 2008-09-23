/*	=====
 *	ps.cc
 *	=====
 */

// Standard C++
#include <string>

// POSIX
#include <dirent.h>

// Iota
#include "iota/strings.hh"

// Nucleus
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/FileDescriptor.hh"
#include "POSeven/Open.hh"
#include "POSeven/Pathnames.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
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
	
	inline std::string left_padded( const std::string& word, unsigned length )
	{
		return left_padded( &*word.begin(), &*word.end(), length );
	}
	
	inline std::string right_padded( const std::string& word, unsigned length )
	{
		return right_padded( &*word.begin(), &*word.end(), length );
	}
	
	static void report_process( const std::string& pid_name )
	{
		using namespace io::path_descent_operators;
		
		std::string pid_dir = "/proc" / pid_name;
		
		char buffer[ 4096 ];
		
		const char* begin = buffer;
		const char* end   = buffer + p7::read( io::open_for_reading( pid_dir / "stat" ), buffer, 4096 );
		
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
		
		pid_t pid = std::atoi( pid_name.c_str() );
		
		pid_t ppid  = std::atoi( p_ppid  );
		pid_t pgid  = std::atoi( p_pgid  );
		pid_t sid   = std::atoi( p_sid   );
		pid_t tpgid = std::atoi( p_tpgid );
		
		term_string.erase( 0, STRLEN( "/dev/" ) );
		
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
		
		char* cmdline_end = buffer + p7::read( io::open_for_reading( pid_dir / "cmdline" ), buffer, 4096 );
		
		ASSERT( cmdline_end > buffer );
		
		std::replace( buffer, cmdline_end - 1, '\0', ' ' );  // replace NUL with space except last
		
		report.append( buffer, cmdline_end - 1 );
		
		if ( report.size() > 80 )
		{
			report.resize( 80 );
		}
		
		report += "\n";
		
		p7::write( p7::stdout_fileno, report.data(), report.size() );
	}
	
	static void ps()
	{
		p7::write( p7::stdout_fileno, STR_LEN( "  PID TERM    STAT   PPID   PGID    SID  COMMAND\n" ) );
		
		DIR* iter = opendir( "/proc" );
		
		while ( const dirent* ent = readdir( iter ) )
		{
			if ( pid_t pid = std::atoi( ent->d_name ) )
			{
				// A process could exit while we're examining it
				try
				{
					report_process( ent->d_name );
				}
				catch ( ... )
				{
				}
			}
		}
		
		closedir( iter );
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		ps();
		
		return 0;
	}
	
}

