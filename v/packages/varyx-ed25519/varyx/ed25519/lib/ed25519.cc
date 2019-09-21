/*
	ed25519.cc
	----------
*/

#include "varyx/ed25519/lib/ed25519.hh"

// Standard C
#include <stdint.h>

// ed25519-donna
#include "ed25519.h"

// debug
#include "debug/assert.hh"


namespace ed25519
{
	
	plus::string publickey( const plus::string& secret_key )
	{
		ASSERT( secret_key.size() == 32 );
		
		ed25519_public_key public_key;
		
		ed25519_publickey( (const uint8_t*) secret_key.data(), public_key );
		
		return plus::string( (char*) public_key, sizeof public_key );
	}
	
	plus::string sign( const plus::string&  secret_key,
	                   const plus::string&  message )
	{
		ASSERT( secret_key.size() == 32 );
		
		const uint8_t* secret = (const uint8_t*) secret_key.data();
		const uint8_t* msg    = (const uint8_t*) message.data();
		
		ed25519_public_key public_key;
		
		ed25519_publickey( secret, public_key );
		
		ed25519_signature sig;
		
		ed25519_sign( msg, message.size(), secret, public_key, sig );
		
		return plus::string( (char*) sig, sizeof sig );
	}
	
	bool verify( const plus::string&  public_key,
	             const plus::string&  message,
	             const plus::string&  signature )
	{
		ASSERT( public_key.size() == 32 );
		ASSERT( signature .size() == 64 );
		
		const uint8_t* pub = (const uint8_t*) public_key.data();
		const uint8_t* msg = (const uint8_t*) message.data();
		const uint8_t* sig = (const uint8_t*) signature.data();
		
		int nok = ed25519_sign_open( msg, message.size(), pub, sig );
		
		return nok == 0;
	}
	
}
