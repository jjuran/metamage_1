/*
	data.cc
	-------
*/

#include "mac_cg/data.hh"

// Standard C
#include <stdlib.h>
#include <string.h>


namespace mac {
namespace cg  {

static
void release_data( void* info, const void* data, size_t size )
{
	free( const_cast< void* >( data ) );
}

CGDataProviderRef make_data_provider_xfer( void* buffer, size_t size )
{
	CGDataProviderRef result = CGDataProviderCreateWithData( NULL,
	                                                         buffer,
	                                                         size,
	                                                         &release_data );
	
	if ( result == NULL )
	{
		free( buffer );
	}
	
	return result;
}

CGDataProviderRef make_data_provider_copy( void* data, size_t size )
{
	void* buffer = malloc( size );
	
	if ( buffer == NULL )
	{
		return NULL;
	}
	
	memcpy( buffer, data, size );
	
	return make_data_provider_xfer( buffer, size );
}

}
}
