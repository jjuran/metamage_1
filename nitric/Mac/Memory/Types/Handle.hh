/*
	Mac/Memory/Types/Handle.hh
	--------------------------
*/

#ifndef MAC_MEMORY_TYPES_HANDLE_HH
#define MAC_MEMORY_TYPES_HANDLE_HH

#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

// nucleus
#include "nucleus/disposer.hh"


namespace Mac
{
	
	class Handle
	{
		private:
			::Handle its_handle;
		
		public:
			Handle() : its_handle()
			{
			}
			
			template < class T >
			Handle( T** h ) : its_handle( reinterpret_cast< ::Handle >( h ) )
			{
			}
			
			::Handle Get() const       { return its_handle; }
			operator ::Handle() const  { return its_handle; }
			
			void* operator*() const    { return *its_handle; }
			
			friend bool operator==( Handle a, Handle b )    { return a.Get() == b.Get(); }
			friend bool operator!=( Handle a, Handle b )    { return a.Get() != b.Get(); }
	};
	
}

namespace nucleus
{
	
	template <> struct disposer< Mac::Handle >
	{
		typedef Mac::Handle  argument_type;
		typedef void         result_type;
		
		void operator()( Mac::Handle h ) const
		{
			::DisposeHandle( h );
		}
	};
	
}

#endif

