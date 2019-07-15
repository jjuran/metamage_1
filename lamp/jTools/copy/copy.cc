/*
	copy.cc
	-------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Standard C
#include <string.h>

// relix
#include "relix/pump.h"


#define STR_LEN( s )  "" s, (sizeof s - 1)


static off_t get_file_size( int fd )
{
	struct stat st;
	
	int nok = fstat( fd, &st );
	
	return nok ? 0 : st.st_size;
}

static char* inscribe_backwards( unsigned long long x, char* end )
{
	char* p = end;
	
	for ( ;  x != 0;  x /= 10 )
	{
		*--p = '0' + x % 10;
	}
	
	return p;
}

static int copy( const char* src, const char* dest, bool dumping_progress )
{
	int nok;
	
	int in = nok = open( src, O_RDONLY );
	
	if ( in >= 0 )
	{
		char progress_buffer[ 20 + 1 + 20 + 1 ];  // numer / denom \n
		
		char* end = progress_buffer + sizeof progress_buffer;
		
		char* p = end;
		
		if ( dumping_progress )
		{
			*--p = '\n';
			
			p = inscribe_backwards( get_file_size( in ), p );
			
			*--p = '/';
		}
		
		int out = nok = open( dest, O_WRONLY|O_CREAT|O_EXCL, 0666 );
		
		if ( out >= 0 )
		{
			const size_t length = dumping_progress ? 16 * 1024 : 0;
			
			size_t n_pumped = 0;
			
			ssize_t pumped;
			
			while ( (pumped = pump( in, NULL, out, NULL, length, 0 )) > 0 )
			{
				n_pumped += pumped;
				
				if ( dumping_progress )
				{
					char* q = inscribe_backwards( n_pumped, p );
					
					write( STDOUT_FILENO, q, end - q );
				}
			}
			
			nok = -(pumped != 0);
			
			close( out );
		}
		
		close( in );
	}
	
	return nok;
}

static inline bool is( const char* a, const char* b )
{
	return strcmp( a, b ) == 0;
}

int main( int argc, char** argv )
{
	bool dumping_progress = false;
	
	if ( argc >= 1 + 1  &&  is( argv[ 1 ], "--dump-progress" ) )
	{
		dumping_progress = true;
		
		--argc;
		++argv;
	}
	
	// Check for sufficient number of args
	if ( argc != 1 + 2 )
	{
		write( STDERR_FILENO, STR_LEN( "Usage: copy <source> <dest>\n" ) );
		
		return 1;
	}
	
	const char* source = argv[1];
	const char* dest   = argv[2];
	
	int nok = copy( source, dest, dumping_progress );
	
	if ( nok )
	{
		write( STDERR_FILENO, STR_LEN( "copy failed (sorry)\n" ) );
	}
	
	return -nok;
}
