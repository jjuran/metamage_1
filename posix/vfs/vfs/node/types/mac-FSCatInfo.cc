/*
	mac-FSCatInfo.cc
	----------------
*/

#include "vfs/node/types/mac.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

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
#include "vfs/node/types/FSCatInfo.hh"
#include "vfs/node/types/namedfork.hh"


namespace vfs
{

namespace p7 = poseven;

using iota::big_u16;
using iota::big_u32;
using iota::u16_from_big;
using iota::u32_from_big;


#ifdef __APPLE__

struct macinfo_extra_FSCatInfo
{
	FSRef         ref;
	special_info  type;
};

typedef macinfo_extra_FSCatInfo macinfo_extra;

static
void macinfo_stat( const node*   that,
                   struct stat&  st )
{
	macinfo_extra& extra = *(macinfo_extra*) that->extra();
	
	memset( &st, '\0', sizeof st );
	
	FSCatalogInfoBitmap bits = kFSCatInfoPermissions;
	FSCatalogInfo       info;
	
	OSStatus err;
	
	err = FSGetCatalogInfo( &extra.ref, bits, &info, NULL, NULL, NULL );
	
	if ( err )
	{
		return;
	}
	
	st.st_mode = permissions( info ).mode;
	
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
	
	FSCatalogInfoBitmap bits = info_bits[ extra.type ];
	FSCatalogInfo       info;
	
	OSStatus err;
	
	err = FSGetCatalogInfo( &extra.ref, bits, &info, NULL, NULL, NULL );
	
	if ( err )
	{
		return plus::string::null;  // FIXME
	}
	
	plus::string result;
	
	char* begin = result.reset( info_sizes[ extra.type ] );
	
	if ( extra.type == Info_GetFInfo )
	{
		*begin++ = 0;  // ioFlAttrib -- FIXME
		*begin++ = 0;  // ioFlVersNum
	}
	
	uint32_t* p4 = (uint32_t*) begin;
	
	const OSType* q4 = (const OSType*) info.finderInfo;
	
	*p4++ = big_u32( *q4++ );  // type
	*p4++ = big_u32( *q4++ );  // creator
	
	uint16_t* p2 = (uint16_t*) p4;
	
	if ( extra.type >= Info_FInfo )
	{
		const UInt16* q2 = (const UInt16*) q4;
		
		*p2++ = big_u16( *q2++ );  // finderFlags
		*p2++ = big_u16( *q2++ );  // location.v
		*p2++ = big_u16( *q2++ );  // location.h
		*p2++ = big_u16( *q2++ );  // reservedField
	}
	
	if ( extra.type == Info_GetFInfo )
	{
		p4 = (uint32_t*) p2;
		
		*p4++ = big_u32( info.nodeID );  // Host information leak?
		
		p2 = (uint16_t*) p4;
		
		*p2++ = 0;  // ioFlStBlk
		
		p4 = (uint32_t*) p2;
		
		*p4++ = big_u32( info.dataLogicalSize );
		*p4++ = big_u32( info.dataPhysicalSize );
		
		p2 = (uint16_t*) p4;
		
		*p2++ = 0;  // ioFlRStBlk
		
		p4 = (uint32_t*) p2;
		
		*p4++ = big_u32( info.rsrcLogicalSize  );
		*p4++ = big_u32( info.rsrcPhysicalSize );
		*p4++ = big_u32( d1904_from_d1904z( info.createDate    .lowSeconds ) );
		*p4++ = big_u32( d1904_from_d1904z( info.contentModDate.lowSeconds ) );
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
	
	OSStatus err;
	
	FSCatalogInfoBitmap bits = info_bits[ extra.type ];
	FSCatalogInfo       info = {};
	
	const OSType* p4 = (const OSType*) data;
	
	OSType* q4 = (OSType*) info.finderInfo;
	
	*q4++ = u32_from_big( *p4++ );  // type
	*q4++ = u32_from_big( *p4++ );  // creator
	
	const UInt16* p2 = (const UInt16*) p4;
	
	if ( extra.type >= Info_FInfo )
	{
		UInt16* q2 = (UInt16*) q4;
		
		*q2++ = u16_from_big( *p2++ );  // finderFlags
		*q2++ = u16_from_big( *p2++ );  // location.v
		*q2++ = u16_from_big( *p2++ );  // location.h
		*q2++ = u16_from_big( *p2++ );  // reservedField
	}
	
	if ( extra.type == Info_SetFInfo )
	{
		p4 = (const UInt32*) (data + size - 2 * sizeof (UInt32));
		
		info.createDate    .lowSeconds = d1904z_from_d1904( u32_from_big( *p4++ ) );
		info.contentModDate.lowSeconds = d1904z_from_d1904( u32_from_big( *p4++ ) );
	}
	
	err = FSSetCatalogInfo( &extra.ref, bits, &info );
	
	if ( err )
	{
		p7::throw_errno( EIO );
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
	FSRef ref;
	Boolean isDir;
	
	FSCatalogInfoBitmap bits = kFSCatInfoPermissions;
	FSCatalogInfo       info;
	
	OSStatus err;
	
	(err = FSPathMakeRef( (UInt8*) path, &ref, &isDir ))  ||
	(err = FSGetCatalogInfo( &ref, bits, &info, NULL, NULL, NULL ));
	
	if ( err != noErr )
	{
		return node_ptr();  // A call ancestor will deal with it
	}
	
	mode_t mode = permissions( info ).mode;
	
	node* result = new node( parent,
	                         "<info>",
	                         mode,
	                         user,
	                         &macinfo_methods,
	                         sizeof (macinfo_extra) );
	
	macinfo_extra& extra = *(macinfo_extra*) result->extra();
	
	extra.ref  = ref;
	extra.type = type;
	
	return result;
}

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

#endif  // #ifdef __APPLE__

}
