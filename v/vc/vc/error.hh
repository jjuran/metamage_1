/*
	error.hh
	--------
*/

#ifndef VC_ERROR_HH
#define VC_ERROR_HH


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define VC_ERROR( type, s )  ::vc::error( STR_LEN( type " ERROR: " s "\n" ) )

#define DOMAIN_ERROR( s )    VC_ERROR( "DOMAIN",   s )
#define INTERNAL_ERROR( s )  VC_ERROR( "INTERNAL", s )
#define SYMBOL_ERROR( s )    VC_ERROR( "SYMBOL",   s )
#define SYNTAX_ERROR( s )    VC_ERROR( "SYNTAX",   s )
#define TYPE_ERROR( s )      VC_ERROR( "TYPE",     s )

	
namespace vc
{
	
	void error( const char* msg, unsigned len );
	
}

#endif
