/*
	get-bndl-icon.cc
	----------------
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"

// chars
#include "conv/mac_utf8.hh"


const int sizeof_icon = 256;

struct FREF_resource
{
	OSType  file_type;
	short   local_id;
};

struct BNDL_header
{
	OSType  signature;
	short   owner_rsrc_id;
	short   array_count_1;    // should be 1 (meaning 2), 'ICN#' and 'FREF'
};

struct BNDL_array_header
{
	ResType  type;
	short    count_1;
};

struct BNDL_array_element
{
	short local_id;
	short res_id;
};

static inline
BNDL_array_header* begin( BNDL_header* bndl )
{
	return (BNDL_array_header*) (bndl + 1);
}

static inline
BNDL_array_header* next( BNDL_array_header* it )
{
	short sizeof_elements = (it->count_1 + 1) * sizeof (BNDL_array_element);
	
	return (BNDL_array_header*) ((char*) (it + 1) + sizeof_elements );
}

static inline
BNDL_array_header* find( BNDL_header* bndl, ResType type )
{
	BNDL_array_header* it = begin( bndl );
	
	short n_1 = bndl->array_count_1;
	
	do
	{
		if ( it->type == type )
		{
			return it;
		}
		
		it = next( it );
	}
	while ( --n_1 >= 0 );
	
	return NULL;
}

static inline
BNDL_array_header* find( BNDL_header** bndl, ResType type )
{
	return find( *bndl, type );
}

static inline
BNDL_array_element* begin( BNDL_array_header* array )
{
	return (BNDL_array_element*) (array + 1);
}

static
short get_local_id_for_type( OSType type )
{
	short i = 0;
	
	while ( Handle h = Get1IndResource( 'FREF', ++i ) )
	{
		FREF_resource* fref = *(FREF_resource**) h;
		
		if ( fref->file_type == type )
		{
			return fref->local_id;
		}
	}
	
	return -1;
}

static inline
BNDL_header** get_bndl()
{
	/*
		There should only be one 'BNDL' resource.
		
		Most applications have a 'BNDL' ID 128, but Lode Runner's is ID 256.
		Thanks, Glenn!
	*/
	
	return (BNDL_header**) Get1IndResource( 'BNDL', 1 );
}

int main( int argc, char** argv )
{
	if ( argc < 3 )
	{
		return 0;
	}
	
	using conv::mac_from_utf8_nothrow;
	
	const char* path = argv[ 1 ];
	const char* type = argv[ 2 ];
	
	const size_t len = strlen( path );
	
	Str31 appName;
	
	char* buffer = (char*) appName + 1;
	
	size_t n_bytes = mac_from_utf8_nothrow( buffer, 31, path, len );
	
	if ( n_bytes > 31 )
	{
		return 2;
	}
	
	appName[ 0 ] = n_bytes;
	
	OSType file_type;
	
	if ( strlen( type ) != sizeof file_type )
	{
		return 2;
	}
	
	mempcpy( &file_type, type, sizeof file_type );
	
	short refnum = OpenResFile( appName );
	
	if ( refnum > 0 )
	{
		short local_id = get_local_id_for_type( file_type );
		
		if ( BNDL_header** bndl = get_bndl() )
		{
			if ( local_id >= 0 )
			{
				BNDL_array_header* icons = find( bndl, 'ICN#' );
				
				short count_1 = icons->count_1;
				
				BNDL_array_element* icon = begin( icons );
				
				while ( count_1-- >= 0 )
				{
					if ( icon->local_id == local_id )
					{
						Handle h = GetResource( 'ICN#', icon->res_id );
						
						if ( h )
						{
							ssize_t n = write( STDOUT_FILENO, *h, sizeof_icon );
							
							if ( n == sizeof_icon )
							{
								return 0;
							}
						}
						
						break;
					}
					
					++icon;
				}
			}
		}
	}
	
	return 1;
}
