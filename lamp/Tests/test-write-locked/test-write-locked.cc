/*	====================
 *	test-write-locked.cc
 *	====================
 */

// Iota
#include "iota/strings.hh"

// POSeven
#include "POSeven/FileDescriptor.hh"
#include "POSeven/extras/slurp.hh"
#include "POSeven/functions/fchmod.hh"
#include "POSeven/functions/open.hh"
#include "POSeven/functions/stat.hh"
#include "POSeven/functions/unlink.hh"
#include "POSeven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	#define TEST_STRING "Testing\n"
	
	
	#define RUN_TEST( condition, i )  run_test( condition, STR_LEN( "ok " #i "\n" ) )
	
	static void run_test( bool condition, const char* ok, std::size_t ok_length )
	{
		if ( !condition )
		{
			p7::write( p7::stdout_fileno, STR_LEN( "not " ) );
		}
		
		p7::write( p7::stdout_fileno, ok, ok_length );
	}
	
	static void lock_and_write( p7::fd_t fd )
	{
		p7::fchmod( fd, p7::_400 );
		
		p7::write( fd, STR_LEN( TEST_STRING ) );
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		p7::write( p7::stdout_fileno, STR_LEN( "1..3\n" ) );
		
		const char* path = "/tmp/test-write-locked.txt";
		
		(void) ::unlink( path );
		
		lock_and_write( p7::open( path, p7::o_wronly | p7::o_creat | p7::o_excl ) );
		
		RUN_TEST( (p7::stat( path ).st_mode & 0200) == 0, 1 );
		
		std::string slurped = p7::slurp( path );
		
		const bool match = std::strcmp( slurped.c_str(), TEST_STRING ) == 0;
		
		RUN_TEST( match, 2 );
		
		p7::unlink( path );
		
		p7::open( path, p7::o_wronly | p7::o_creat | p7::o_excl, p7::_400 );
		
		RUN_TEST( (p7::stat( path ).st_mode & 0200) == 0, 3 );
		
		p7::unlink( path );
		
		return 0;
	}
	
}

