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

// mac-types
#include "mac_types/epoch.hh"

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
	
#endif  // #ifdef __APPLE__
	
	struct macinfo_extra
	{
	#ifdef __APPLE__
		FSRef         ref;
	#else
		struct stat   st;
	#endif
		special_info  type;
	};
	
	static
	void macinfo_stat( const node*   that,
	                   struct stat&  st )
	{
		macinfo_extra& extra = *(macinfo_extra*) that->extra();
		
		memset( &st, '\0', sizeof st );
		
	#ifdef __APPLE__
		
		FSCatalogInfoBitmap bits = kFSCatInfoPermissions;
		FSCatalogInfo       info;
		
		OSStatus err;
		
		err = FSGetCatalogInfo( &extra.ref, bits, &info, NULL, NULL, NULL );
		
		if ( err )
		{
			return;
		}
		
		st.st_mode = permissions( info ).mode;
		
	#else
		
		st.st_mode = extra.st.st_mode;
		
	#endif
		
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
		
	#ifdef __APPLE__
		
		FSCatalogInfoBitmap bits = info_bits[ extra.type ];
		FSCatalogInfo       info;
		
		OSStatus err;
		
		err = FSGetCatalogInfo( &extra.ref, bits, &info, NULL, NULL, NULL );
		
		if ( err )
		{
			return plus::string::null;  // FIXME
		}
		
	#endif
		
		plus::string result;
		
		char* begin = result.reset( info_sizes[ extra.type ] );
		
		if ( extra.type == Info_GetFInfo )
		{
			*begin++ = 0;  // ioFlAttrib -- FIXME
			*begin++ = 0;  // ioFlVersNum
		}
		
		uint32_t* p4 = (uint32_t*) begin;
		
	#ifdef __APPLE__
		
		const OSType* q4 = (const OSType*) info.finderInfo;
		
		*p4++ = iota::big_u32( *q4++ );  // type
		*p4++ = iota::big_u32( *q4++ );  // creator
		
	#else
		
		*p4++ = '*' * 0x01010101;  // type
		*p4++ = '?' * 0x01010101;  // creator
		
	#endif
		
		uint16_t* p2 = (uint16_t*) p4;
		
		if ( extra.type >= Info_FInfo )
		{
		#ifdef __APPLE__
			
			const UInt16* q2 = (const UInt16*) q4;
			
			*p2++ = iota::big_u16( *q2++ );  // finderFlags
			*p2++ = iota::big_u16( *q2++ );  // location.v
			*p2++ = iota::big_u16( *q2++ );  // location.h
			*p2++ = iota::big_u16( *q2++ );  // reservedField
			
		#else
			
			memset( p2, '\0', 4 * sizeof (uint16_t) );
			
			p2 += 4;
			
		#endif
		}
		
		if ( extra.type == Info_GetFInfo )
		{
			p4 = (uint32_t*) p2;
			
		#ifdef __APPLE__
			
			*p4++ = iota::big_u32( info.nodeID );  // Host information leak?
			
		#else
			
			*p4++ = iota::big_u32( extra.st.st_ino );  // Host information leak?
			
		#endif
			
			p2 = (uint16_t*) p4;
			
			*p2++ = 0;  // ioFlStBlk
			
			p4 = (uint32_t*) p2;
			
		#ifdef __APPLE__
			
			*p4++ = iota::big_u32( info.dataLogicalSize );
			*p4++ = iota::big_u32( info.dataPhysicalSize );
			
		#else
			
			*p4++ = iota::big_u32( extra.st.st_size );
			*p4++ = iota::big_u32( extra.st.st_blocks * 512 );
			
		#endif
			
			p2 = (uint16_t*) p4;
			
			*p2++ = 0;  // ioFlRStBlk
			
			p4 = (uint32_t*) p2;
			
		#ifdef __APPLE__
			
			*p4++ = iota::big_u32( info.rsrcLogicalSize  );
			*p4++ = iota::big_u32( info.rsrcPhysicalSize );
			*p4++ = iota::big_u32( info.createDate    .lowSeconds );
			*p4++ = iota::big_u32( info.contentModDate.lowSeconds );
			
		#else
			
			using mac::types::epoch_delta;
			
			*p4++ = 0;
			*p4++ = 0;
			*p4++ = 0;
			*p4++ = iota::big_u32( epoch_delta() + extra.st.st_mtime );
			
		#endif
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
		
	#ifdef __APPLE__
		
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
		
	#endif
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
	#ifdef __APPLE__
		
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
		
		mode_t mode = permissions( info ).mode;
		
	#else
		
		struct stat st;
		
		int nok = stat( path.c_str(), &st );
		
		if ( nok )
		{
			return node_ptr();  // A call ancestor will deal with it
		}
		
		mode_t mode = st.st_mode;
		
	#endif
		
		node* result = new node( parent,
		                         "<info>",
		                         mode,
		                         user,
		                         &macinfo_methods,
		                         sizeof (macinfo_extra) );
		
		macinfo_extra& extra = *(macinfo_extra*) result->extra();
		
	#ifdef __APPLE__
		extra.ref  = ref;
	#else
		extra.st   = st;
	#endif
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
	
}
