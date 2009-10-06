/*	=============
 *	test-pread.cc
 *	=============
 */

// Standard C
#include <unistd.h>

// Iota
#include "iota/strings.hh"

// poseven
#include "poseven/functions/lseek.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/unlink.hh"
#include "poseven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	#define RUN_TEST( condition, i )  run_test( condition, STR_LEN( "ok " #i "\n" ) )
	
	static void run_test( bool condition, const char* ok, std::size_t ok_length )
	{
		if ( !condition )
		{
			p7::write( p7::stdout_fileno, STR_LEN( "not " ) );
		}
		
		p7::write( p7::stdout_fileno, ok, ok_length );
	}
	
	
	static void pread_pipe()
	{
		int fds[2];
		
		p7::throw_posix_result( pipe( fds ) );
		
		char buffer[ 16 ];
		
		int n_read = pread( fds[0], buffer, sizeof buffer, 0 );
		
		RUN_TEST( n_read == -1  &&  errno == ESPIPE, 1 );
		
		close( fds[0] );
		close( fds[1] );
	}
	
	static const char *const tmp_file_path = "/tmp/pread.txt";
	
	static void create_tmp_file()
	{
		p7::write( p7::open( tmp_file_path,
		                     p7::o_rdwr | p7::o_creat | p7::o_trunc ),
		           STR_LEN( "123456789abcdef\n" ) );
	}
	
	static void pread_file()
	{
		NN::Owned< p7::fd_t > tmp_file = p7::open( tmp_file_path, p7::o_rdonly );
		
		char buffer[ 16 ];
		
		
		int length = STRLEN( "1234" );
		
		int n_read = pread( tmp_file, buffer, length, 0 );
		
		RUN_TEST( n_read == length  &&  memcmp( buffer, STR_LEN( "1234" ) ) == 0, 2 );
		
		RUN_TEST( p7::lseek( tmp_file ) == 0, 3 );
		
		
		length = STRLEN( "34567" );
		
		n_read = pread( tmp_file, buffer, length, 2 );
		
		RUN_TEST( n_read == length  &&  memcmp( buffer, STR_LEN( "34567" ) ) == 0, 4 );
		
		RUN_TEST( p7::lseek( tmp_file ) == 0, 5 );
		
		
		p7::lseek( tmp_file, 6 );
		
		length = STRLEN( "cdef\n" );
		
		n_read = pread( tmp_file, buffer, length + 3, 11 );
		
		RUN_TEST( n_read == length  &&  memcmp( buffer, STR_LEN( "cdef\n" ) ) == 0, 6 );
		
		RUN_TEST( p7::lseek( tmp_file ) == 6, 7 );
	}
	
	static void delete_tmp_file()
	{
		p7::unlink( tmp_file_path );
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		p7::write( p7::stdout_fileno, STR_LEN( "1..7\n" ) );
		
		pread_pipe();
		
		create_tmp_file();
		
		pread_file();
		
		delete_tmp_file();
		
		return 0;
	}
	
}

