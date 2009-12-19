/*	==============
 *	BinaryImage.cc
 *	==============
 */

#include "Genie/BinaryImage.hh"

// Debug
#include "debug/assert.hh"

// Nitrogen
#include "Nitrogen/Resources.h"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// MacIO
#include "MacIO/FSRead_Sync.hh"
#include "MacIO/GetCatInfo_Sync.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/Utilities/AsyncIO.hh"


namespace Nucleus
{
	
	template <>
	struct Converter< Owned< Nitrogen::Ptr >, Nitrogen::Handle >: public std::unary_function< Nitrogen::Handle, Owned< Nitrogen::Ptr > >
	{
		Owned< Nitrogen::Ptr > operator()( const Nitrogen::Handle& h ) const
		{
			Nitrogen::Size size = Nitrogen::GetHandleSize( h );
			
			Owned< Nitrogen::Ptr > result = Nitrogen::NewPtr( size );
			
			std::copy( *h.Get(), *h.Get() + size, result.Get().Get() );
			
			return result;
		}
	};
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	struct BinaryFileMetadata
	{
		UInt32 dataForkLength;
		UInt32 creationDate;
		UInt32 modificationDate;
		UInt32 fileID;
		
		BinaryFileMetadata()  {}
		
		BinaryFileMetadata( const HFileInfo& hFileInfo ) : dataForkLength  ( hFileInfo.ioFlLgLen ),
		                                                   creationDate    ( hFileInfo.ioFlCrDat ),
		                                                   modificationDate( hFileInfo.ioFlMdDat ),
		                                                   fileID          ( hFileInfo.ioDirID   )
		{
		}
		
		friend bool operator==( const BinaryFileMetadata& a, const BinaryFileMetadata& b )
		{
			return    a.dataForkLength   == b.dataForkLength
			       && a.creationDate     == b.creationDate
			       && a.modificationDate == b.modificationDate
			       && a.fileID           == b.fileID;
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
	
	template < class T > static int cmp( const T& a, const T& b )
	{
		return   a < b ? -1
		       : b < a ? +1
		       :          0;
	}
	
	static int less_PascalStrings( const unsigned char* a, const unsigned char* b )
	{
		return std::lexicographical_compare( a + 1, a + 1 + a[0], b + 1, b + 1 + b[0] );
	}
	
	struct less_FSSpecs
	{
		bool operator()( const FSSpec& a, const FSSpec& b ) const
		{
			return   a.vRefNum != b.vRefNum ? a.vRefNum > b.vRefNum  // -1 before -2
			       : a.parID   != b.parID   ? a.parID   < b.parID
			       :                          less_PascalStrings( a.name, b.name );
		}
	};
	
	typedef std::map< FSSpec, BinaryImageCacheEntry, less_FSSpecs > BinaryImageCache;
	
	static BinaryImageCache gBinaryImageCache;
	
	static BinaryFileMetadata GetFileMetadata( const FSSpec& file )
	{
		CInfoPBRec pb = { 0 };
		
		MacIO::GetCatInfo< MacIO::Throw_All >( pb, file );
		
		return BinaryFileMetadata( pb.hFileInfo );
	}
	
	
	static BinaryImage ReadProgramAsCodeResource()
	{
		N::ResType  resType = N::ResType( 'Wish' );
		N::ResID    resID   = N::ResID  ( 0      );
		
		BinaryImage code = N::DetachResource( N::Get1Resource( resType, resID ) );
		
		N::HLockHi( code.Get() );
		
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
		
		NN::Owned< N::FSFileRefNum > refNum = N::FSpOpenDF( file, N::fsRdPerm );
		
		if ( length == kCFragGoesToEOF )
		{
			UInt32 eof = N::GetEOF( refNum );
			
			if ( offset >= eof )
			{
				p7::throw_errno( ENOEXEC );
			}
			
			length = eof - offset;
		}
		
		BinaryImage data = N::NewHandle( length );
		
		N::HLockHi( data );
		
		MacIO::FSRead( MacIO::kThrowEOF_Always,
		               refNum,
		               N::fsFromStart,
		               offset,
		               length,
		               *data.Get().Get() );
		
		return data;
	}
	
	inline BinaryImage ReadImageFromFile( const FSSpec& file )
	{
		NN::Owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( file, N::fsRdPerm );
		
		const bool rsrc = TARGET_CPU_68K && !TARGET_RT_MAC_CFM;
		
		return rsrc ? ReadProgramAsCodeResource(      )
		            : ReadProgramAsCodeFragment( file );
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
		
		if ( image.Sole() && image.get() && *image.get() )
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
		
		if ( TARGET_API_MAC_CARBON )
		{
			// GetMemFragment / kPrivateCFragCopy is broken on OS X
			// FIXME:  Find out which versions exactly,
			// and still cache for the good ones and OS 9
			// Or just test for it at runtime
			return ReadImageFromFile( file );
		}
		
		ReleaseUnusedCode();
		
		BinaryFileMetadata metadata = GetFileMetadata( file );
		
		BinaryImageCache::iterator it = gBinaryImageCache.find( file );
		
		BinaryImageCacheEntry* cacheEntry = NULL;
		
		// Do we have a cached image for this binary?
		if ( it != gBinaryImageCache.end() )
		{
			cacheEntry = &it->second;
			
			// Do the metadata match?
			if ( cacheEntry->metadata == metadata )
			{
				const BinaryImage& image = cacheEntry->image;
				
			retry:
				
				N::Handle h = image.get();
				
				if ( h == NULL )
				{
					// If the handle is NULL, then another process is loading
					// the image.
					
					const bool simultaneous_load_never_occurs = false;
					
					ASSERT( simultaneous_load_never_occurs );
					
					AsyncYield();
					
					goto retry;
				}
				
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
			cacheEntry = &gBinaryImageCache[ file ];  // insert null cache entry
		}
		
		// cacheEntry->image is NULL
		
		BinaryImageCacheEntry newEntry;
		
		newEntry.metadata = metadata;
		newEntry.image    = ReadImageFromFile( file );  // this may async-yield
		
		// Mark it purgeable for when we later unlock it
		N::HPurge( newEntry.image );
		
		// Install the new cache entry
		*cacheEntry = newEntry;
		
		return newEntry.image;
	}
	
}

