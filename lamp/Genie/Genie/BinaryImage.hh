/*	==============
 *	BinaryImage.hh
 *	==============
 */

#ifndef GENIE_BINARYIMAGE_HH
#define GENIE_BINARYIMAGE_HH

// Mac OS
#include <Files.h>

// boost
#include "boost/shared_ptr.hpp"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	class BinaryImage
	{
		public:
			struct Data {};
		
		private:
			boost::shared_ptr< Data > itsData;
			std::size_t itsDataSize;
		
		public:
			BinaryImage() : itsDataSize()  {}
			
			BinaryImage( std::auto_ptr< Data > data, std::size_t size );
			
			std::size_t GetSize() const  { return itsDataSize; }
			
			const void* GetPointer() const  { return itsData.get(); }
	};
	
	BinaryImage GetBinaryImage( const FSSpec& file );
	
}

#endif

