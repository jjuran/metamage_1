/*
	json-reader.cc
	--------------
*/

// POSIX
#include <unistd.h>
#include <sys/stat.h>

// Standard C
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// vxo
#include "vxo/container.hh"
#include "vxo/error.hh"

// vxs
#include "vxs/lib/json/decode.hh"
#include "vxs/string.hh"


#define PROGRAM  "json-count"


using vxo::Box;
using vxo::Container;
using vxo::Error;
using vxo::String;


static inline
int count( const Box& box )
{
	if ( const Container* container = box.is< Container >() )
	{
		return container->size();
	}
	
	return 1;
}

static
int count( const Box& box, const char* key )
{
	if ( const Container* container = box.is< Container >() )
	{
		const Container& box = *container;
		
		const size_t n = box.size();
		
		int total = 0;
		
		if ( const int is_map = box.subtype_byte() & 1 )
		{
			for ( size_t i = 0;  i < n;  ++i )
			{
				if ( static_cast< const String& >( box[ i++ ] ).get() == key )
				{
					total += count( box[ i ] );
				}
				else
				{
					total += count( box[ i ], key );
				}
			}
		}
		else
		{
			for ( size_t i = 0;  i < n;  ++i )
			{
				total += count( box[ i ], key );
			}
		}
		
		return total;
	}
	
	return 0;
}

int main( int argc, char** argv )
{
	int fd = STDIN_FILENO;
	
	struct stat st;
	
	size_t n;
	
	char* p = NULL;
	
	int nok = fstat( fd, &st )                 ||
	          (! S_ISREG( st.st_mode )  &&
	           (errno = ESPIPE))               ||
	          ! (n = st.st_size,
	             p = (char*) malloc( n + 1 ))  ||
	          read( fd, p, n ) != n;
	
	if ( nok )
	{
		perror( "<stdin>" );
		return 1;
	}
	
	p[ n ] = '\0';
	
	if ( strlen( p ) != n )
	{
		fprintf( stderr, "%s: %s\n", PROGRAM, "forbidden NUL byte" );
		return 1;
	}
	
	const Box box = vxo::json_decode( p );
	
	if ( const Error* error = box.is< Error >() )
	{
		fprintf( stderr, "%s: %s\n", PROGRAM, error->get() );
		return 1;
	}
	
	for ( int i = 1;  i < argc;  ++i )
	{
		const char* key = argv[ i ];
		
		int n = count( box, key );
		
		printf( "%s: %d\n", key, n );
	}
	
	return 0;
}
