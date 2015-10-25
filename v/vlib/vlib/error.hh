/*
	error.hh
	--------
*/

#ifndef VLIB_ERROR_HH
#define VLIB_ERROR_HH


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define VLIB_ERROR( type, s )  ::vlib::error( STR_LEN( type " ERROR: " s "\n" ) )

#define DOMAIN_ERROR( s )    VLIB_ERROR( "DOMAIN",   s )
#define INTERNAL_ERROR( s )  VLIB_ERROR( "INTERNAL", s )
#define SYMBOL_ERROR( s )    VLIB_ERROR( "SYMBOL",   s )
#define SYNTAX_ERROR( s )    VLIB_ERROR( "SYNTAX",   s )
#define TYPE_ERROR( s )      VLIB_ERROR( "TYPE",     s )

	
namespace vlib
{
	
	void error( const char* msg, unsigned len );
	
}

#endif
