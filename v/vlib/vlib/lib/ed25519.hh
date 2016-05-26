/*
	ed25519.hh
	----------
*/

#ifndef VLIB_LIB_ED25519_HH
#define VLIB_LIB_ED25519_HH

// plus
#include "plus/string.hh"


namespace vlib
{
namespace ed25519
{
	
	plus::string publickey( const plus::string& secret_key );
	
	plus::string sign( const plus::string&  secret_key,
	                   const plus::string&  message );
	
	bool verify( const plus::string&  public_key,
	             const plus::string&  message,
	             const plus::string&  signature );
	
}
}

#endif
