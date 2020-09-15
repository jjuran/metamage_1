/*
	get_string_from_resource.hh
	---------------------------
*/

#ifndef MACRSRC_GETSTRINGFROMRESOURCE_HH
#define MACRSRC_GETSTRINGFROMRESOURCE_HH


namespace mac  {
namespace rsrc {
	
	bool get_string_from_resource( unsigned char*  result,
	                               unsigned long   type,
	                               short           id );
	
	inline
	bool get_STR_resource( unsigned char* result, short id )
	{
		return get_string_from_resource( result, 'STR ', id );
	}
	
}
}

#endif
