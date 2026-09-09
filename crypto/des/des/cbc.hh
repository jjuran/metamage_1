/*
	cbc.hh
	------
*/

#ifndef DES_CBC_HH
#define DES_CBC_HH

// Standard C
#include <stdint.h>

// des
#include "des/types.hh"


namespace des
{

template < class Cipher, class Operation >
struct CBC_mode
{
	Cipher    cipher;
	uint64_t  iv;
	
	CBC_mode( const key_t* key, const iv_t& iv )
	:
		cipher( Operation(), key ), iv( iv )
	{
	}
	
	void crypt_block( uint64_t& text );
};

/*
	We use partial specialization to define
	encryption and decryption separately.
	
	This doesn't work with member functions,
	so we define free functions here.  Since
	the IV gets updated, the object must be
	passed by non-const reference.
	
	But suppose you only needed to make a single
	call and wanted to pass a temporary object?
	An rvalue reference can't be converted to a
	non-const reference, but you can use it to
	invoke a non-const member function -- so we
	need a member function as well.
*/

template < class Cipher >
void crypt_block( CBC_mode< Cipher, Encryption >& cbc, uint64_t& text )
{
	text ^= cbc.iv;
	
	cbc.cipher.crypt_block( text );  // plaintext -> ciphertext
	
	cbc.iv = text;
}

template < class Cipher >
void crypt_block( CBC_mode< Cipher, Decryption >& cbc, uint64_t& text )
{
	uint64_t iv = cbc.iv;
	
	cbc.iv = text;
	
	cbc.cipher.crypt_block( text );  // ciphertext -> plaintext
	
	text ^= iv;
}

template < class Cipher, class Operation >
inline
void CBC_mode< Cipher, Operation >::crypt_block( uint64_t& text )
{
	::des::crypt_block( *this, text );
}

}

#endif
