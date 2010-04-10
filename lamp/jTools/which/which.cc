/*	========
 *	which.cc
 *	========
 */

// Standard C/C++
#include <cstdlib>
#include <cstring>

// Standard C++
#include <vector>

// POSIX
#include <sys/stat.h>
#include <unistd.h>

// plus
#include "plus/string.hh"


int main( int argc, char const *const argv[] )
{
	// Check for sufficient number of args
	if ( argc <= 1 )
	{
		return 1;
	}
	
	const char* path = std::getenv( "PATH" );
	
	if ( path == NULL )
	{
		path = "/usr/bin:/bin";
	}
	
	std::vector< plus::string > dirs;
	
	for ( const char* p = path;  ;  )
	{
		const char* q = std::strchr( p, ':' );
		
		if ( q == NULL )
		{
			q = std::strchr( p, '\0' );
		}
		
		if ( q > p )
		{
			dirs.push_back( plus::string( p, q ) );
		}
		
		if ( *q != ':' )
		{
			break;
		}
		
		p = q + 1;
	}
	
	
	bool failed = false;
	
	for ( const char *const *program = argv + 1;  *program != NULL;  ++program )
	{
		typedef std::vector< plus::string >::const_iterator Iter;
		
		bool found = false;
		
		for ( Iter it = dirs.begin();  it != dirs.end();  ++it )
		{
			plus::string pathname = *it + "/" + *program;
			
			struct ::stat sb;
			
			int status = ::stat( pathname.c_str(), &sb );
			
			int mask = S_IFMT | S_IXUSR;
			
			if ( status == 0  &&  (sb.st_mode & mask) == (S_IFREG | S_IXUSR) )
			{
				found = true;
				
				pathname += "\n";
				
				::write( STDOUT_FILENO, pathname.data(), pathname.length() );
				
				break;
			}
		}
		
		if ( !found )
		{
			failed = true;
		}
	}
	
	return failed ? 1 : 0;
}

