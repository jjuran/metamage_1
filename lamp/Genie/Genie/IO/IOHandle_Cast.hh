/*
	IOHandle_Cast.hh
	----------------
*/

#ifndef GENIE_IO_IOHANDLECAST_HH
#define GENIE_IO_IOHANDLECAST_HH

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	template < class Handle > struct IOHandle_Downcast_Traits;
	
	template < class Handle >
	Handle* IOHandle_Cast( IOHandle* handle )
	{
		if ( handle == NULL )
		{
			return NULL;
		}
		
		IOHandle* base = handle->GetBaseForCast( IOHandle_Downcast_Traits< Handle >::GetTest() );
		
		return static_cast< Handle* >( base );
	}
	
	void Check_IOHandle_Cast( const void* cast, int errorToThrow );
	
	template < class Handle >
	inline Handle& IOHandle_Cast( IOHandle& handle, int errnum = 0 )
	{
		Handle* cast = IOHandle_Cast< Handle >( &handle );
		
		if ( errnum == 0 )
		{
			errnum = IOHandle_Downcast_Traits< Handle >::GetError( handle );
		}
		
		Check_IOHandle_Cast( cast, errnum );
		
		return *cast;
	}
	
}

#endif

