/*	==============
 *	BinaryImage.cc
 *	==============
 */

#include "Genie/BinaryImage.hh"

// Standard C
#include <string.h>

// Standard C++
#include <map>

// mac-sys-utils
#include "mac_sys/has/FSSpec_calls.hh"
#include "mac_sys/has/native_Carbon.hh"
#include "mac_sys/has/RealTempMemory.hh"
#include "mac_sys/mem_error.hh"

// mac-file-utils
#include "mac_file/open_data_fork.hh"
#include "mac_file/rw.hh"

// Debug
#include "debug/assert.hh"

// Nitrogen
#include "Nitrogen/Files.hh"
#include "Nitrogen/Resources.hh"

// MacIO
#include "MacIO/GetCatInfo_Sync.hh"


#if TARGET_CPU_68K
#define CACHE_LINKAGE extern
#else
#define CACHE_LINKAGE static
#endif

namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	struct code_rsrc_header
	{
		uint16_t branch;
		uint16_t flags;
		uint32_t type;
		uint16_t id;
		uint16_t version;
	};
	
	struct BinaryFileMetadata
	{
		UInt32 dataForkLength;
		UInt32 creationDate;
		UInt32 modificationDate;
		UInt32 fileID;
		UInt32 vRefNum;  // vRefNum is SInt16, but this makes things simple
		
		BinaryFileMetadata()  {}
		
		BinaryFileMetadata( const HFileInfo& hFileInfo ) : dataForkLength  ( hFileInfo.ioFlLgLen ),
		                                                   creationDate    ( hFileInfo.ioFlCrDat ),
		                                                   modificationDate( hFileInfo.ioFlMdDat ),
		                                                   fileID          ( hFileInfo.ioDirID   ),
		                                                   vRefNum         ( hFileInfo.ioVRefNum )
		{
		}
		
		friend bool operator==( const BinaryFileMetadata& a, const BinaryFileMetadata& b )
		{
			return    a.dataForkLength   == b.dataForkLength
			       && a.creationDate     == b.creationDate
			       && a.modificationDate == b.modificationDate
			       && a.fileID           == b.fileID
			       && a.vRefNum          == b.vRefNum;
		}
		
		friend bool operator!=( const BinaryFileMetadata& a, const BinaryFileMetadata& b )
		{
			return !( a == b );
		}
	};
	
	struct BinaryImageCacheEntry
	{
		BinaryImage         image;
		BinaryFileMetadata  metadata;
	};
	
	typedef std::map< UInt32, BinaryImageCacheEntry > BinaryImageCache;
	
	CACHE_LINKAGE BinaryImageCache gBinaryImageCache;
	
	static BinaryFileMetadata GetFileMetadata( const FSSpec& file )
	{
		CInfoPBRec pb;
		
		MacIO::GetCatInfo< MacIO::Throw_All >( pb, file );
		
		return BinaryFileMetadata( pb.hFileInfo );
	}
	
	static inline
	Handle new_handle_nothrow( size_t size )
	{
		OSErr err;
		Handle h;
		
		(mac::sys::has_RealTempMemory()  &&  (h = TempNewHandle( size, &err )))
		                                 ||  (h = NewHandle    ( size       ));
		
		return h;
	}
	
	static BinaryImage ReadProgramFromDataFork( const FSSpec& file, UInt32 offset, UInt32 length )
	{
		Handle h;
		
		short opened = mac::file::open_data_fork( file, fsRdPerm );
		
		OSErr err = opened;
		
		if ( opened >= 0 )
		{
			short refNum = opened;
			
			if ( length == kCFragGoesToEOF )
			{
				Size eof;
				
				err = GetEOF( refNum, &eof );
				
				if ( err )  goto closing;
				
				if ( offset >= eof )
				{
					err = paramErr;  // converts to EINVAL
					goto closing;
				}
				
				length = eof - offset;
			}
			
			h = new_handle_nothrow( length );
			
			if ( h == NULL )
			{
				err = memFullErr;
				
				goto closing;
			}
			
			HLockHi( h );
			
			err = mac::file::read_all( refNum, *h, length, offset );
			
			if ( TARGET_CPU_68K  &&  err == noErr )
			{
				code_rsrc_header& header = *(code_rsrc_header*) *h;
				
				// Handle dereferenced here
				
				if ( header.branch != 0x600A )
				{
					err = paramErr;  // converts to EINVAL
				}
			}
			
			if ( err != noErr )
			{
				DisposeHandle( h );
			}
			
		closing:
			
			FSClose( refNum );
		}
		
		Mac::ThrowOSStatus( err );
		
		return n::owned< Mac::Handle >::seize( h );
	}
	
	
	static BinaryImage ReadProgramAsCodeResource()
	{
		N::ResType  resType = N::ResType( 'Tool' );
		N::ResID    resID   = N::ResID  ( 0      );
		
		BinaryImage code = N::DetachResource( N::Get1Resource( resType, resID ) );
		
		HLockHi( code.get() );
		
		return code;
	}
	
	static bool
	ends_with( const uint8_t* whole, const char* part, size_t len )
	{
		size_t length = whole[ 0 ];
		size_t offset = 1 + length - len;
		
		return length >= len  &&  memcmp( whole + offset, part, len ) == 0;
	}
	
	static bool CFragResourceMemberIsLoadable( const CFragResourceMember& member )
	{
		// We aren't using CFM-68K, but it's worth writing correct code anyway
		if ( member.architecture != kCompiledCFragArch )
		{
			return false;
		}
		
		if ( TARGET_CPU_68K )
		{
			return true;  // No Carbon on 68K
		}
		
		// Check if fragment name is or ends with "Carbon"
		ConstStr255Param name = member.name;
		
		if ( ends_with( name, "(None)", 6 ) )
		{
			return true;
		}
		
		bool forCarbon = ends_with( name, "Carbon", 6 );
		
		return forCarbon == TARGET_API_MAC_CARBON;
	}
	
	static const CFragResourceMember* FindLoadableMemberInCFragResource( const CFragResource& cfrg )
	{
		unsigned memberCount = cfrg.memberCount;
		
		const CFragResourceMember* member = &cfrg.firstMember;
		
		while ( memberCount > 0 )
		{
			if ( CFragResourceMemberIsLoadable( *member ) )
			{
				return member;
			}
			
			member += member->memberSize;
			
			--memberCount;
		}
		
		return NULL;
	}
	
	static BinaryImage ReadProgramAsCodeFragment( const FSSpec& file )
	{
		N::ResType  resType = N::ResType( kCFragResourceType );  // cfrg
		N::ResID    resID   = N::ResID  ( kCFragResourceID   );  // 0
		
		::CFragResource** cfrg = N::Handle_Cast< ::CFragResource >( N::Get1Resource( resType, resID ) );
		
		// Handle dereferenced here
		const ::CFragResourceMember* member = FindLoadableMemberInCFragResource( **cfrg );
		
		if ( member == NULL )
		{
			Mac::ThrowOSStatus( paramErr );  // converts to EINVAL
		}
		
		UInt32 offset = member->offset;
		UInt32 length = member->length;
		
		// Handle no longer used here
		
		return ReadProgramFromDataFork( file, offset, length );
	}
	
	static inline BinaryImage ReadImageFromFile( const FSSpec& file )
	{
		if ( ! mac::sys::has_FSSpec_calls() )
		{
			goto data_fork_only;
		}
		
		try
		{
			n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( file, N::fsRdPerm );
			
			const bool rsrc = TARGET_CPU_68K && !TARGET_RT_MAC_CFM;
			
			return rsrc ? ReadProgramAsCodeResource(      )
			            : ReadProgramAsCodeFragment( file );
		}
		catch ( const N::OSStatus& err )
		{
			if ( err == eofErr )
			{
				// Empty resource fork, try data fork
			}
			else if ( err == resNotFound )
			{
				// No code resource, try data fork
			}
			else
			{
				throw;
			}
		}
		
	data_fork_only:
		
		return ReadProgramFromDataFork( file, 0, kCFragGoesToEOF );
	}
	
	static bool CachedImageIsPurged( const BinaryImageCache::value_type& value )
	{
		const BinaryImageCacheEntry& cacheEntry = value.second;
		
		const BinaryImage& image = cacheEntry.image;
		
		return *image.get() == NULL;
	}
	
	static void ReleaseCachedImageIfUnused( const BinaryImageCache::value_type& value )
	{
		const BinaryImageCacheEntry& cacheEntry = value.second;
		
		const BinaryImage& image = cacheEntry.image;
		
		if ( image.sole() && image.get() && *image.get() )
		{
			HUnlock( image.get() );
		}
	}
	
	static void ReleaseUnusedCode()
	{
		typedef BinaryImageCache::iterator Iter;
		
		Iter begin = gBinaryImageCache.begin();
		Iter end   = gBinaryImageCache.end();
		
		while ( begin != end )
		{
			Iter it = begin;
			
			++begin;
			
			const BinaryImageCache::value_type& value = *it;
			
			if ( CachedImageIsPurged( value ) )
			{
				gBinaryImageCache.erase( it );
			}
			else
			{
				ReleaseCachedImageIfUnused( value );
			}
		}
	}
	
	BinaryImage GetBinaryImage( const FSSpec& file )
	{
		if ( TARGET_CPU_68K )
		{
			// Can't share code resources among threads, so don't bother caching it
			return ReadImageFromFile( file );
		}
		
		if ( mac::sys::has_native_Carbon() )
		{
			// kPrivateCFragCopy is broken on OS X as early as 10.2.8
			return ReadImageFromFile( file );
		}
		
		ReleaseUnusedCode();
		
		BinaryFileMetadata metadata = GetFileMetadata( file );
		
		const UInt32 hash = metadata.vRefNum << 24 ^ metadata.fileID;
		
		BinaryImageCache::iterator it = gBinaryImageCache.find( hash );
		
		BinaryImageCacheEntry* cacheEntry = NULL;
		
		// Do we have a cached image for this binary?
		if ( it != gBinaryImageCache.end() )
		{
			cacheEntry = &it->second;
			
			// Do the metadata match?
			if ( cacheEntry->metadata == metadata )
			{
				const BinaryImage& image = cacheEntry->image;
				
				N::Handle h = image.get();
				
				/*
					h could theoretically be NULL here *if* the code loader
					used async I/O, but it doesn't.
				*/
				
				ASSERT( h != NULL );
				
				// Is the handle unpurged?
				if ( *h )
				{
					// Make sure it's locked
					HLockHi( h );
					
					return image;
				}
				
				// Our cached image got purged; load another
			}
			
			// Reset the handle to NULL to indicate that we're loading it.
			cacheEntry->image.reset();
		}
		else
		{
			cacheEntry = &gBinaryImageCache[ hash ];  // insert null cache entry
		}
		
		// cacheEntry->image is NULL
		
		BinaryImageCacheEntry& newEntry = *cacheEntry;
		
		newEntry.metadata = metadata;
		newEntry.image    = ReadImageFromFile( file );
		
		// Mark it purgeable for when we later unlock it
		HPurge( newEntry.image.get() );
		
		return newEntry.image;
	}
	
}
