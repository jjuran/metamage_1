/*
	mac-generic.cc
	--------------
*/

#include "vfs/node/types/mac.hh"

// POSIX
#include <sys/stat.h>

// Standard C
#include <string.h>

// iota
#include "iota/endian.hh"

// d1904z
#include "d1904z.hh"

// plus
#include "plus/string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node.hh"
#include "vfs/node/types/namedfork.hh"


namespace vfs
{

namespace p7 = poseven;

using iota::big_u16;
using iota::big_u32;
using iota::u16_from_big;
using iota::u32_from_big;


struct macinfo_extra_generic
{
	struct stat   st;
	special_info  type;
};

typedef macinfo_extra_generic macinfo_extra;

static
void macinfo_stat( const node*   that,
                   struct stat&  st )
{
	macinfo_extra& extra = *(macinfo_extra*) that->extra();
	
	memset( &st, '\0', sizeof st );
	
	st.st_mode = extra.st.st_mode;
	
	st.st_size = info_sizes[ extra.type ];
}

static
plus::string macinfo_slurp( const node* that )
{
	macinfo_extra& extra = *(macinfo_extra*) that->extra();
	
	if ( extra.type == Info_SetFInfo )
	{
		p7::throw_errno( EPERM );
	}
	
	plus::string result;
	
	char* begin = result.reset( info_sizes[ extra.type ] );
	
	if ( extra.type == Info_GetFInfo )
	{
		*begin++ = 0;  // ioFlAttrib -- FIXME
		*begin++ = 0;  // ioFlVersNum
	}
	
	uint32_t* p4 = (uint32_t*) begin;
	
	*p4++ = '*' * 0x01010101;  // type
	*p4++ = '?' * 0x01010101;  // creator
	
	uint16_t* p2 = (uint16_t*) p4;
	
	if ( extra.type >= Info_FInfo )
	{
		memset( p2, '\0', 4 * sizeof (uint16_t) );
		
		p2 += 4;
	}
	
	if ( extra.type == Info_GetFInfo )
	{
		p4 = (uint32_t*) p2;
		
		*p4++ = big_u32( extra.st.st_ino );  // Host information leak?
		
		p2 = (uint16_t*) p4;
		
		*p2++ = 0;  // ioFlStBlk
		
		p4 = (uint32_t*) p2;
		
		*p4++ = big_u32( extra.st.st_size );
		*p4++ = big_u32( extra.st.st_blocks * 512 );
		
		p2 = (uint16_t*) p4;
		
		*p2++ = 0;  // ioFlRStBlk
		
		p4 = (uint32_t*) p2;
		
		*p4++ = 0;
		*p4++ = 0;
		*p4++ = 0;
		*p4++ = big_u32( d1904_from_time( extra.st.st_mtime ) );
	}
	
	return result;
}

static
void macinfo_splat( const node* that, const char* data, size_t size )
{
	macinfo_extra& extra = *(macinfo_extra*) that->extra();
	
	if ( extra.type == Info_GetFInfo )
	{
		p7::throw_errno( EPERM );
	}
	
	const size_t expected_size = info_sizes[ extra.type ];
	
	if ( size != expected_size )
	{
		p7::throw_errno( EINVAL );
	}
}

static const item_method_set macinfo_item_methods =
{
	&macinfo_stat,
};

static const data_method_set macinfo_data_methods =
{
	NULL,
	NULL,
	NULL,
	&macinfo_slurp,
	&macinfo_splat,
};

static const node_method_set macinfo_methods =
{
	&macinfo_item_methods,
	&macinfo_data_methods,
};

static
node_ptr new_info( special_info  type,
                   const char*   path,
                   const node*   parent,
                   uid_t         user )
{
	struct stat st;
	
	int nok = stat( path, &st );
	
	if ( nok )
	{
		return node_ptr();  // A call ancestor will deal with it
	}
	
	mode_t mode = st.st_mode;
	
	node* result = new node( parent,
	                         "<info>",
	                         mode,
	                         user,
	                         &macinfo_methods,
	                         sizeof (macinfo_extra) );
	
	macinfo_extra& extra = *(macinfo_extra*) result->extra();
	
	extra.st   = st;
	extra.type = type;
	
	return result;
}

#ifdef __APPLE__
#define mac_lookup_info  mac_lookup_info_generic
#endif

node_ptr mac_lookup_info( const char*  path,
                          const char*  fork_name,
                          const node*  parent,
                          uid_t        user )
{
	special_info type = Info_null;
	
	if ( strcmp( fork_name, "PkgInfo" ) == 0 )
	{
		type = Info_PkgInfo;
	}
	else if ( strcmp( fork_name, "FInfo" ) == 0 )
	{
		type = Info_FInfo;
	}
	else if ( strcmp( fork_name, "GetFInfo" ) == 0 )
	{
		type = Info_GetFInfo;
	}
	else if ( strcmp( fork_name, "SetFInfo" ) == 0 )
	{
		type = Info_SetFInfo;
	}
	else
	{
		return node_ptr();
	}
	
	return new_info( type, path, parent, user );
}

}
