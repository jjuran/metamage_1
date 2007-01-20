/*	==============
 *	BinaryImage.cc
 *	==============
 */

#include "Genie/BinaryImage.hh"

// Nitrogen
#include "Nitrogen/Files.h"

// Io
#include "io/slurp.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
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
		//BinaryImage         image;
		NN::Shared< N::Ptr >  image;
		BinaryFileMetadata   metadata;
	};
	
	template < class T > static int cmp( const T& a, const T& b )
	{
		return   a < b ? -1
		       : b < a ? +1
		       :          0;
	}
	
	static int ComparePascalStrings( const unsigned char* a, const unsigned char* b )
	{
		int result = std::lexicographical_compare( a, a + 1 + a[0], b, b + 1 + b[0] );
		
		if ( result == 0 )  result = cmp( a[0], b[0] );
		
		return result;
	}
	
	struct less_FSSpecs
	{
		bool operator()( const FSSpec& a, const FSSpec& b ) const
		{
			return   a.vRefNum != b.vRefNum ? a.vRefNum > b.vRefNum  // -1 before -2
			       : a.parID   != b.parID   ? a.parID   < b.parID
			       : a.name[0] != b.name[0] ? a.name[0] < b.name[0]  // ok to sort by length
			       :                          ComparePascalStrings( a.name, b.name ) < 0;
		}
	};
	
	typedef std::map< FSSpec, BinaryImageCacheEntry, less_FSSpecs > BinaryImageCache;
	
	static BinaryImageCache gBinaryImageCache;
	
	static BinaryFileMetadata GetFileMetadata( const FSSpec& file )
	{
		CInfoPBRec pb;
		
		N::FSpGetCatInfo( file, pb );
		
		return BinaryFileMetadata( pb.hFileInfo );
	}
	
	NN::Shared< N::Ptr > GetBinaryImage( const FSSpec& file )
	{
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
				return cacheEntry->image;  // Yup, we're done
			}
		}
		else
		{
			cacheEntry = &gBinaryImageCache[ file ];  // insert null cache entry
		}
		
		BinaryImageCacheEntry newEntry;
		
		newEntry.metadata = metadata;
		newEntry.image    = io::slurp_file< N::PtrFlattener >( file );
		
		// Install the new cache entry
		*cacheEntry = newEntry;
		
		return newEntry.image;
	}
	
}

