/*
	json-print.cc
	-------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Standard C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// iota
#include "iota/char_types.hh"

// vxo
#include "vxo/boolean.hh"
#include "vxo/container.hh"
#include "vxo/error.hh"
#include "vxo/lib/quote.hh"
#include "vxo/number.hh"
#include "vxo/symbol.hh"

// vxs
#include "vxs/lib/json/decode.hh"
#include "vxs/string.hh"


static plus::string spaces;

static
bool can_be_unquoted( const char* begin, const char* end )
{
	if ( begin == end  ||  iota::is_digit( *begin ) )
	{
		return false;
	}
	
	while ( begin < end )
	{
		const char c = *begin++;
		
		if ( ! iota::is_alnum( c )  &&  c != '_' )
		{
			return false;
		}
	}
	
	return true;
}

static
int display( const vxo::Box& box, int depth, char sep, char term )
{
	const int n_spaces = depth * 2;
	
	size_t spaces_size = spaces.size();
	
	if ( spaces_size < n_spaces )
	{
		spaces_size *= 2;
		
		char* p = spaces.reset_nothrow( spaces_size );
		
		if ( ! p )
		{
			return 108;
		}
		
		memset( p, ' ', spaces_size );
		
		p[ spaces_size ] = '\0';
	}
	
	const char* indent = spaces.data() + (spaces_size - n_spaces);
	
	if ( const vxo::String* string = box.is< vxo::String >() )
	{
		const plus::string& s = *string;
		
		const char*  data = s.data();
		const size_t size = s.size();
		
		const size_t length = vxo::quoted_length( data, size );
		
		plus::string quoted;
		
		char* p = quoted.reset_nothrow( length + 2 );
		
		if ( ! p )
		{
			return 108;
		}
		
		char* q = vxo::quote_string( data, size, p );
		
		if ( sep == ':'  &&  can_be_unquoted( p + 1, q - 1 ) )
		{
			++p;
			--q;
		}
		
		*q++ = sep;
		*q++ = term;
		*q = '\0';
		
		printf( "%s%s", indent, p );
	}
	else if ( const vxo::Number* number = box.is< vxo::Number >() )
	{
		// Assume double
		
		double x = *(const double*) number;
		
		printf( "%s%.16g%c\n", indent, x, sep );
	}
	else if ( const vxo::Bool* boolean = box.is< vxo::Bool >() )
	{
		const char* value = *(const char*) boolean ? "true" : "false";
		
		printf( "%s%s%c\n", indent, value, sep );
	}
	else if ( box.is< vxo::Symbol >() )
	{
		printf( "%s%s%c\n", indent, "null", sep );
	}
	else if ( const vxo::Container* container = box.is< vxo::Container >() )
	{
		const char opening[] = "[{";
		const char closing[] = "]}";
		const char odd_sep[] = ",:";
		const char odd_eol[] = "\n ";
		
		const vxo::Container& box = *container;
		
		const int is_map = box.subtype_byte() & 1;
		
		int is_odd = 0;
		
		++depth;
		
		printf( "%s%c\n", indent, opening[ is_map ] );
		
		const size_t n = box.size();
		
		for ( size_t i = 0;  i < n;  ++i )
		{
			const vxo::Box& v = box[ i ];
			
			const int is_deep = ! is_odd  ||  v.is< vxo::Container >();
			
			if ( is_odd  &&  v.is< vxo::Container >() )
			{
				putchar( '\n' );
			}
			
			is_odd = is_map & ! is_odd;
			
			const char sep = odd_sep[ is_odd ];
			const char eol = odd_eol[ is_odd ];
			
			if ( int xs = display( v, depth * is_deep, sep, eol ) )
			{
				return xs;
			}
		}
		
		printf( "%s%c%c\n", indent, closing[ is_map ], sep );
	}
	else
	{
		const int is_map = box.control_byte() & 1;
		
		printf( "%s%s%c\n", indent, is_map ? "{}" : "[]", sep );
	}
	
	return 0;
}

static
void display( const vxo::Box& box )
{
	char* p = spaces.reset_nothrow( vxo::inline_char_size );  // always succeeds
	
	memset( p, ' ', vxo::inline_char_size );
	
	display( box, 0, ' ', '\n' );
}

int main( int argc, char** argv )
{
	char** argp = argv + 1;
	
	if ( argc < 2 )
	{
		fprintf( stderr, "usage: %s path\n", argv[ 0 ] );
		return 2;
	}
	
	const char* path = *argp++;
	
	char* p = NULL;
	
	struct stat st;
	
	size_t n;
	int fd;
	int nok = (fd = open( path, O_RDONLY )) < 0  ||
	          fstat( fd, &st )                   ||
	          ! S_ISREG( st.st_mode )            ||
	          ! (n = st.st_size,
	             p = (char*) malloc( n + 1 ))    ||
	          read( fd, p, n ) != n;
	
	if ( nok )
	{
		perror( path );
		return 1;
	}
	
	p[ n ] = '\0';
	
	const vxo::Box box = vxo::json_decode( p );
	
	if ( const vxo::Error* error = box.is< vxo::Error >() )
	{
		fprintf( stderr, "%s: %s\n", argv[ 0 ], error->get() );
		return 1;
	}
	
	display( box );
	
	return 0;
}
