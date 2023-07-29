/*
	get_string_from_resource.hh
	---------------------------
*/

#ifndef MACRSRC_GETSTRINGFROMRESOURCE_HH
#define MACRSRC_GETSTRINGFROMRESOURCE_HH


namespace mac  {
namespace rsrc {
	
	typedef char** Handle;
	
	signed char get_string_from_handle( unsigned char*  result,
	                                    Handle          h );
	
	signed char get_string_from_resource( unsigned char*  result,
	                                      unsigned long   type,
	                                      short           id );
	
	inline
	signed char get_STR_resource( unsigned char* result, short id )
	{
		return get_string_from_resource( result, 'STR ', id );
	}
	
	signed char get_vers_ShortVersionString( unsigned char* result, short id );
	
}
}

#endif
