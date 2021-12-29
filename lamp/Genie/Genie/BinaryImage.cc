/*	==============
 *	BinaryImage.cc
 *	==============
 */

#include "Genie/BinaryImage.hh"

// Standard C++
#include <algorithm>
#include <map>

// mac-sys-utils
#include "mac_sys/has/FSSpec_calls.hh"
#include "mac_sys/has/RealTempMemory.hh"
#include "mac_sys/mem_error.hh"

// Debug
#include "debug/assert.hh"

// Nitrogen
#include "Nitrogen/Files.hh"
#include "Nitrogen/Resources.hh"

// MacFeatures
#include "MacFeatures/Features.hh"

// MacIO
#include "MacIO/FSRead_Sync.hh"
#include "MacIO/GetCatInfo_Sync.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/Utilities/OpenDataFork.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
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
	
	static BinaryImageCache gBinaryImageCache;
	
	static BinaryFileMetadata GetFileMetadata( const FSSpec& file )
	{
		CInfoPBRec pb = {{ 0 }};
		
		MacIO::GetCatInfo< MacIO::Throw_All >( pb, file );
		
		return BinaryFileMetadata( pb.hFileInfo );
	}
	
	static
	n::owned< N::Handle > new_handle( size_t size )
	{
		OSErr err;
		Handle h;
		
		if (( h = NewHandle( size ) ))  goto done;
		
		if ( mac::sys::has_RealTempMemory() )
		{
			if (( h = TempNewHandle( size, &err ) ))  goto done;
		}
		else
		{
			err = mac::sys::mem_error();
		}
		
		Mac::ThrowOSStatus( err );
		
	done:
		
		return n::owned< N::Handle >::seize( N::Handle( h ) );
	}
	
	static BinaryImage ReadProgramFromDataFork( const FSSpec& file, UInt32 offset, UInt32 length )
	{
		n::owned< N::FSFileRefNum > refNum = OpenDataFork( file, N::fsRdPerm );
		
		if ( length == kCFragGoesToEOF )
		{
			UInt32 eof = N::GetEOF( refNum );
			
			if ( offset >= eof )
			{
				p7::throw_errno( EINVAL );
			}
			
			length = eof - offset;
		}
		
		BinaryImage data = new_handle( length );
		
		N::HLockHi( data );
		
		MacIO::FSRead( MacIO::kThrowEOF_Always,
		               refNum,
		               N::fsFromStart,
		               offset,
		               length,
		               *data.get().Get() );
		
		if ( TARGET_CPU_68K )
		{
			code_rsrc_header& header = *(code_rsrc_header*) *data.get().Get();
			
			// Handle dereferenced here
			
			if ( header.branch != 0x600A )
			{
				p7::throw_errno( EINVAL );
			}
		}
		
		return data;
	}
	
	
	static BinaryImage ReadProgramAsCodeResource()
	{
		N::ResType  resType = N::ResType( 'Tool' );
		N::ResID    resID   = N::ResID  ( 0      );
		
		BinaryImage code = N::DetachResource( N::Get1Resource( resType, resID ) );
		
		N::HLockHi( code.get() );
		
		return code;
	}
	
	static bool SuffixMatches( const unsigned char* name, const unsigned char* endOfName, const char* pattern, std::size_t length )
	{
		return name[0] >= length  &&  std::equal( endOfName - length, endOfName, pattern );
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
		N::Str255 name = member.name;
		
		const unsigned char* endOfName = name + 1 + name[0];
		
		if ( SuffixMatches( name, endOfName, "(None)", 6 ) )
		{
			return true;
		}
		
		bool forCarbon = SuffixMatches( name, endOfName, "Carbon", 6 );
		
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
			p7::throw_errno( EINVAL );
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
	
	template < class Container, class Predicate >
	static void erase_if( Container& container, Predicate f )
	{
		typename Container::iterator it;
		
		while ( (it = std::find_if( container.begin(), container.end(), f )) != container.end() )
		{
			container.erase( it );
		}
	}
	
	static void ReleaseCachedImageIfUnused( const BinaryImageCache::value_type& value )
	{
		const BinaryImageCacheEntry& cacheEntry = value.second;
		
		const BinaryImage& image = cacheEntry.image;
		
		if ( image.sole() && image.get() && *image.get() )
		{
			N::HUnlock( image.get() );
		}
	}
	
	static void ReleaseUnusedCode()
	{
		erase_if( gBinaryImageCache, std::ptr_fun( CachedImageIsPurged ) );
		
		std::for_each( gBinaryImageCache.begin(),
		               gBinaryImageCache.end(),
		               std::ptr_fun( ReleaseCachedImageIfUnused ) );
	}
	
	BinaryImage GetBinaryImage( const FSSpec& file )
	{
		if ( TARGET_CPU_68K )
		{
			// Can't share code resources among threads, so don't bother caching it
			return ReadImageFromFile( file );
		}
		
		if ( MacFeatures::Is_Running_OSXNative() )
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
					N::HLockHi( h );
					
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
		
		BinaryImageCacheEntry newEntry;
		
		newEntry.metadata = metadata;
		newEntry.image    = ReadImageFromFile( file );
		
		// Mark it purgeable for when we later unlock it
		N::HPurge( newEntry.image );
		
		// Install the new cache entry
		*cacheEntry = newEntry;
		
		return newEntry.image;
	}
	
}
