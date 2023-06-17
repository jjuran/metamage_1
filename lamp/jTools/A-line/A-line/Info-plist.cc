/*
	Info-plist.cc
	-------------
*/

#include "A-line/Info-plist.hh"

// POSIX
#include <unistd.h>
#include <sys/wait.h>

// Standard C
#include <errno.h>
#include <string.h>

// plus
#include "plus/string/concat.hh"

// pfiles
#include "pfiles/common.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)


namespace tool
{
	
	namespace p7 = poseven;
	
	using namespace io::path_descent_operators;
	
	
	InfoPListTask::InfoPListTask( const plus::string&  name,
	                              const plus::string&  input,
	                              const plus::string&  contents )
	:
		FileTask( contents / "Info.plist" ),
		its_name ( name  ),
		its_input( input )
	{
	}
	
	void InfoPListTask::Make()
	{
		const plus::string jobhead = "INFO  " + its_name + "\n";
		
		write( STDOUT_FILENO, jobhead.data(), jobhead.size() );
		
		const char* txt   = its_input.c_str();
		const char* plist = OutputPath().c_str();
		
		int fd = open( plist, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
		
		p7::throw_posix_result( fd );
		
		pid_t pid = vfork();
		
		p7::throw_posix_result( pid );
		
		if ( pid == 0 )
		{
			int dupe = dup2( fd, STDOUT_FILENO );
			
			if ( dupe < 0 )
			{
				const plus::string dup_error = plus::string( "A-line: dup2: " )
				                             + strerror( errno )
				                             + "\n";
				
				write( STDERR_FILENO, dup_error.data(), dup_error.size() );
				
				_exit( 126 );
			}
			
			const char* convert = "make-info-plist";
			
			execlp( convert, convert, txt, (void*) NULL );
			
			const int exit_status = errno == ENOENT ? 127 : 126;
			
			if ( errno == ENOENT )
			{
				write( STDERR_FILENO,
				       STR_LEN( "A-line: make-info-plist not found\n" ) );
			}
			
			_exit( exit_status );
		}
		
		close( fd );
		
		int wait_status;
		pid_t waited = waitpid( pid, &wait_status, 0 );
		
		p7::throw_posix_result( waited );
		
		if ( wait_status != 0 )
		{
			write( STDERR_FILENO,
			       STR_LEN( "A-line: error generating Info.plist" ) );
			
			unlink( plist );
			
			throw Info_plist_error();
		}
	}
	
	void InfoPListTask::Return( bool succeeded )
	{
	}
	
}
