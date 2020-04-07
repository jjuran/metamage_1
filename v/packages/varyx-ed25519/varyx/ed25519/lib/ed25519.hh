/*
	ed25519.hh
	----------
*/

#ifndef VARYX_ED25519_LIB_ED25519_HH
#define VARYX_ED25519_LIB_ED25519_HH

// plus
#include "plus/string.hh"


namespace ed25519
{
	
	plus::string publickey( const plus::string& secret_key );
	
	plus::string sign( const plus::string&  secret_key,
	                   const plus::string&  message );
	
	bool verify( const plus::string&  public_key,
	             const plus::string&  message,
	             const plus::string&  signature );
	
}

#endif
