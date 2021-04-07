/*
	mac.cc
	------
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

// plus
#include "plus/string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/node_method_set.hh"


#define STRLEN( s )      (sizeof "" s - 1)
#define STR_LEN( s )  s, (sizeof "" s - 1)


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	enum special_info
	{
		Info_null,
		Info_PkgInfo,
		Info_FInfo,
		Info_GetFInfo,
		Info_SetFInfo,
	};
	
	static const size_t info_sizes[] =
	{
		0,
		2 * 4,    // two 4-byte OSTypes
		16,       // FInfo, which is 16 bytes
		80 - 30,  // size of HFileParam starting at ioFlAttrib
		80 - 32,  // size of HFileParam starting at ioFlFndrInfo
	};
	
#ifdef __APPLE__
	
	const FSCatalogInfoBitmap bitmap_for_GetFInfo
		= kFSCatInfoNodeID
		| kFSCatInfoCreateDate
		| kFSCatInfoContentMod
		| kFSCatInfoFinderInfo
		| kFSCatInfoDataSizes
		| kFSCatInfoRsrcSizes;
	
	const FSCatalogInfoBitmap bitmap_for_SetFInfo
		= kFSCatInfoCreateDate
		| kFSCatInfoContentMod
		| kFSCatInfoFinderInfo;
	
	static const FSCatalogInfoBitmap info_bits[] =
	{
		0,
		kFSCatInfoFinderInfo,
		kFSCatInfoFinderInfo,
		bitmap_for_GetFInfo,
		bitmap_for_SetFInfo,
	};
	
	static inline
	FSPermissionInfo& get( FSPermissionInfo& permissions )
	{
		return permissions;
	}
	
	static inline
	FSPermissionInfo& get( UInt32* permissions )
	{
		return *(FSPermissionInfo*) permissions;
	}
	
	static inline
	FSPermissionInfo& permissions( FSCatalogInfo& info )
	{
		return get( info.permissions );
	}
	
	struct macinfo_extra
	{
		FSRef         ref;
		special_info  type;
	};
	
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
		
		*p4++ = iota::big_u32( *q4++ );  // type
		*p4++ = iota::big_u32( *q4++ );  // creator
		
		uint16_t* p2 = (uint16_t*) p4;
		
		if ( extra.type >= Info_FInfo )
		{
			const UInt16* q2 = (const UInt16*) q4;
			
			*p2++ = iota::big_u16( *q2++ );  // finderFlags
			*p2++ = iota::big_u16( *q2++ );  // location.v
			*p2++ = iota::big_u16( *q2++ );  // location.h
			*p2++ = iota::big_u16( *q2++ );  // reservedField
		}
		
		if ( extra.type == Info_GetFInfo )
		{
			p4 = (uint32_t*) p2;
			
			*p4++ = iota::big_u32( info.nodeID );  // Host information leak?
			
			p2 = (uint16_t*) p4;
			
			*p2++ = 0;  // ioFlStBlk
			
			p4 = (uint32_t*) p2;
			
			*p4++ = iota::big_u32( info.dataLogicalSize );
			*p4++ = iota::big_u32( info.dataPhysicalSize );
			
			p2 = (uint16_t*) p4;
			
			*p2++ = 0;  // ioFlRStBlk
			
			p4 = (uint32_t*) p2;
			
			*p4++ = iota::big_u32( info.rsrcLogicalSize  );
			*p4++ = iota::big_u32( info.rsrcPhysicalSize );
			*p4++ = iota::big_u32( info.createDate    .lowSeconds );
			*p4++ = iota::big_u32( info.contentModDate.lowSeconds );
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
		
		*q4++ = iota::u32_from_big( *p4++ );  // type
		*q4++ = iota::u32_from_big( *p4++ );  // creator
		
		const UInt16* p2 = (const UInt16*) p4;
		
		if ( extra.type >= Info_FInfo )
		{
			UInt16* q2 = (UInt16*) q4;
			
			*q2++ = iota::u16_from_big( *p2++ );  // finderFlags
			*q2++ = iota::u16_from_big( *p2++ );  // location.v
			*q2++ = iota::u16_from_big( *p2++ );  // location.h
			*q2++ = iota::u16_from_big( *p2++ );  // reservedField
		}
		
		if ( extra.type == Info_SetFInfo )
		{
			p4 = (const UInt32*) (data + size - 2 * sizeof (UInt32));
			
			info.createDate    .lowSeconds = iota::u32_from_big( *p4++ );
			info.contentModDate.lowSeconds = iota::u32_from_big( *p4++ );
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
	node_ptr new_info( special_info         type,
	                   const plus::string&  path,
	                   const node*          parent,
	                   uid_t                user )
	{
		FSRef ref;
		Boolean isDir;
		
		FSCatalogInfoBitmap bits = kFSCatInfoPermissions;
		FSCatalogInfo       info;
		
		OSStatus err;
		
		(err = FSPathMakeRef( (UInt8*) path.c_str(), &ref, &isDir ))  ||
		(err = FSGetCatalogInfo( &ref, bits, &info, NULL, NULL, NULL ));
		
		if ( err != noErr )
		{
			return node_ptr();  // A call ancestor will deal with it
		}
		
		node* result = new node( parent,
		                         "<info>",
		                         permissions( info ).mode,
		                         user,
		                         &macinfo_methods,
		                         sizeof (macinfo_extra) );
		
		macinfo_extra& extra = *(macinfo_extra*) result->extra();
		
		extra.ref  = ref;
		extra.type = type;
		
		return result;
	}
	
	node_ptr mac_lookup_info( const plus::string&  path,
	                          const plus::string&  name,
	                          const node*          parent,
	                          uid_t                user )
	{
		const char* subpath = name.c_str();
		
		if ( strncmp( subpath, STR_LEN( "..namedfork/" ) ) == 0 )
		{
			special_info type = Info_null;
			
			const char* fork_name = subpath + STRLEN( "..namedfork/" );
			
			if ( strcmp( fork_name, "rsrc" ) == 0 )
			{
				// Just return, since OS X's filesystem handles it
			}
			else if ( strcmp( fork_name, "PkgInfo" ) == 0 )
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
			
			if ( type )
			{
				return new_info( type, path, parent, user );
			}
		}
		
		return node_ptr();
	}
	
#endif  // #ifdef __APPLE__
	
}
