/*
	Mac/Memory/Types/Ptr.hh
	-----------------------
*/

#ifndef MAC_MEMORY_TYPES_PTR_HH
#define MAC_MEMORY_TYPES_PTR_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

// nucleus
#ifndef NUCLEUS_DISPOSER_HH
#include "nucleus/disposer.hh"
#endif


namespace Mac
{
	
	class Ptr
	{
		private:
			::Ptr its_ptr;
		
		public:
			Ptr() : its_ptr()
			{
			}
			
			template < class T >
			Ptr( T* thePtr ) : its_ptr( reinterpret_cast< ::Ptr >( thePtr ) )
			{
			}
			
			::Ptr Get() const       { return its_ptr; }
			operator ::Ptr() const  { return its_ptr; }
			
			friend bool operator==( Ptr a, Ptr b )    { return a.Get() == b.Get(); }
			friend bool operator!=( Ptr a, Ptr b )    { return a.Get() != b.Get(); }
	};
	
}

namespace nucleus
{
	
	template <> struct disposer< Mac::Ptr >
	{
		typedef Mac::Ptr  argument_type;
		typedef void      result_type;
		
		void operator()( Mac::Ptr h ) const
		{
			::DisposePtr( h );
		}
	};
	
}

#endif

