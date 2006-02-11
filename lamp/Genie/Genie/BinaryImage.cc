/*	==============
 *	BinaryImage.cc
 *	==============
 */

#include "Genie/BinaryImage.hh"

// Nitrogen / Mac OS
#include "Nitrogen/Files.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	BinaryImage::BinaryImage( std::auto_ptr< Data > data, std::size_t size )
	: fData( data ), fDataSize( size )
	{
		
	}
	
	struct BinaryFileMetadata
	{
		UInt32 fDataForkLength;
		UInt32 fCreationDate;
		UInt32 fModificationDate;
		UInt32 fFileID;
		
		friend bool operator==( const BinaryFileMetadata& a, const BinaryFileMetadata& b )
		{
			return    a.fDataForkLength   == b.fDataForkLength
			       && a.fCreationDate     == b.fCreationDate
			       && a.fModificationDate == b.fModificationDate
			       && a.fFileID           == b.fFileID;
		}
		
		friend bool operator!=( const BinaryFileMetadata& a, const BinaryFileMetadata& b )
		{
			return !( a == b );
		}
	};
	
	struct BinaryImageCacheEntry
	{
		BinaryImage fImage;
		BinaryFileMetadata fMetadata;
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
		
		BinaryFileMetadata result;
		
		result.fDataForkLength   = pb.hFileInfo.ioFlLgLen;
		result.fCreationDate     = pb.hFileInfo.ioFlCrDat;
		result.fModificationDate = pb.hFileInfo.ioFlMdDat;
		result.fFileID           = pb.hFileInfo.ioDirID;
		
		return result;
	}
	
	static BinaryImage LoadBinaryImage( const FSSpec& file )
	{
		NN::Owned< N::FSFileRefNum > filehandle = N::FSpOpenDF( file, fsRdPerm );
		
		std::size_t size = N::GetEOF( filehandle );
		
		std::auto_ptr< BinaryImage::Data > result;
		
		result.reset( static_cast< BinaryImage::Data* >( ::operator new( size ) ) );
		
		int bytes = N::FSRead( filehandle, size, reinterpret_cast< char* >( result.get() ) );
		
		return BinaryImage( result, size );
	}
	
	BinaryImage GetBinaryImage( const FSSpec& file )
	{
		BinaryFileMetadata metadata = GetFileMetadata( file );
		
		BinaryImageCache::iterator it = gBinaryImageCache.find( file );
		
		BinaryImageCacheEntry* cacheEntry = NULL;
		
		// Do we have a cached image for this binary?
		if ( it != gBinaryImageCache.end() )
		{
			cacheEntry = &it->second;
			
			// Do the metadata match?
			if ( cacheEntry->fMetadata == metadata )
			{
				return cacheEntry->fImage;  // Yup, we're done
			}
		}
		else
		{
			cacheEntry = &gBinaryImageCache[ file ];  // insert null cache entry
		}
		
		BinaryImageCacheEntry newEntry;
		
		newEntry.fMetadata = metadata;
		newEntry.fImage = LoadBinaryImage( file );
		
		// Install the new cache entry
		*cacheEntry = newEntry;
		
		return newEntry.fImage;
	}
	
}

