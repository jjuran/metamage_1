/*	=====
 *	ps.cc
 *	=====
 */

// Standard C++
#include <string>

// Standard C
#include <errno.h>

// POSIX
#include <dirent.h>

// Iota
#include "iota/strings.hh"

// Nucleus
#include "Nucleus/Flattener.h"
#include "Nucleus/NAssert.h"

// Io
#include "io/slurp.hh"

// POSeven
#include "POSeven/FileDescriptor.hh"
#include "POSeven/Open.hh"
#include "POSeven/Stat.hh"
#include "POSeven/Pathnames.hh"

// Orion
#include "Orion/Main.hh"


namespace NN = Nucleus;
namespace p7 = poseven;
namespace O = Orion;


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

static void report_process( const std::string& pid_name )
{
	std::string report;
	
	report += left_padded( pid_name, 5 );
	
	report += "  ";
	
	using namespace io::path_descent_operators;
	
	std::string pid_dir = "/proc" / pid_name;
	
	char buffer[ 4096 ];
	
	const char* begin = buffer;
	const char* end   = buffer + p7::read( io::open_for_reading( pid_dir / "stat" ), buffer, 4096 );
	
	const char* close_paren = std::find( begin, end, ')' );
	
	ASSERT( close_paren != end );
	
	const char* stat = close_paren + 2;
	
	const char* space = std::find( stat, end, ' ' );
	
	report += right_padded( stat, space, 4 );
	
	report += "  ";
	
	const char* ppid = space + 1;
	
	space = std::find( ppid, end, ' ' );
	
	report += left_padded( ppid, space, 5 );
	
	report += "  ";
	
	const char* pgid = space + 1;
	
	space = std::find( pgid, end, ' ' );
	
	report += left_padded( pgid, space, 5 );
	
	report += "  ";
	
	const char* sid = space + 1;
	
	space = std::find( sid, end, ' ' );
	
	report += left_padded( sid, space, 5 );
	
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
	p7::write( p7::stdout_fileno, STR_LEN( "  PID  STAT   PPID   PGID    SID  COMMAND\n" ) );
	
	DIR* iter = opendir( "/proc" );
	
	while ( const dirent* ent = readdir( iter ) )
	{
		if ( pid_t pid = std::atoi( ent->d_name ) )
		{
			report_process( ent->d_name );
		}
	}
	
	closedir( iter );
}

int O::Main( int argc, argv_t argv )
{
	ps();
	
	return 0;
}

