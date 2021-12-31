/*	======================
 *	Portage/DepthBuffer.hh
 *	======================
 */

#ifndef PORTAGE_DEPTHBUFFER_HH
#define PORTAGE_DEPTHBUFFER_HH

// Standard C
#include <string.h>

// Standard C++
#include <vector>


namespace Portage
{
	
	template < class Scalar >
	class DepthBuffer
	{
		private:
			unsigned itsWidth;
			unsigned itsHeight;
			
			std::vector< Scalar > itsCells;
		
		public:
			typedef Scalar value_type;
			
			unsigned Width()  const  { return itsWidth;  }
			unsigned Height() const  { return itsHeight; }
			
			std::size_t Size() const  { return itsCells.size(); }
			
			std::size_t DataSize() const  { return Size() * sizeof (Scalar); }
			
			DepthBuffer() : itsWidth( 0 ), itsHeight( 0 ), itsCells()  {}
			
			DepthBuffer( unsigned width, unsigned height ) : itsWidth( width ), itsHeight( height ), itsCells( Size() )
			{
				Reset();
			}
			
			void Reset()
			{
				char* begin = (char*) &*itsCells.begin();
				size_t size = (char*) &*itsCells.end() - begin;
				
				memset( begin, '\0', size );
			}
			
			void Resize( unsigned width, unsigned height )
			{
				itsCells.resize( width * height );
				
				itsWidth  = width;
				itsHeight = height;
				
				Reset();
			}
			
			// Get/Set
			const Scalar& Cell( unsigned h, unsigned v ) const  { return itsCells[ v * itsWidth + h ]; }
			      Scalar& Cell( unsigned h, unsigned v )        { return itsCells[ v * itsWidth + h ]; }
			
			bool Nearer( unsigned x, unsigned y, Scalar z )
			{
				return ( Cell( x, y ) == 0  ||  z < Cell( x, y ) );
			}
			
			void Set( unsigned x, unsigned y, Scalar z )
			{
				Cell( x, y ) = z;
			}
			
			bool SetIfNearer( unsigned x, unsigned y, Scalar z )
			{
				if ( Nearer( x, y, z ) )
				{
					Set( x, y, z );
					
					return true;
				}
				
				return false;
			}
	};
	
}

#endif
