/*
	error.hh
	--------
*/

#ifndef VLIB_ERROR_HH
#define VLIB_ERROR_HH


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define VLIB_ERROR( type, s )  ::vlib::error( STR_LEN( type " ERROR: " s "\n" ) )

#define INTERNAL_ERROR( s )  VLIB_ERROR( "INTERNAL", s )

	
namespace vlib
{
	
	void error( const char* msg, unsigned len );
	
}

#endif
