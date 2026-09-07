/*
	des.cc
	------
*/

#include "des/des.hh"

// des
#include "des/common.hh"
#include "des/subkeys.hh"


#pragma exceptions off


namespace des
{

void cipher_DES::set_encryption_key( const key_t* key )
{
	make_subkeys_straight( keys, *key );
}

void cipher_DES::set_decryption_key( const key_t* key )
{
	make_subkeys_reversed( keys, *key );
}

void cipher_DES::crypt_block( uint64_t& text ) const
{
	uint64_t temp;
	
	initial_permutation( temp, text );
	
	crypt_block_core( keys, temp );
	
	final_permutation( text, temp );
}

}  // namespace des
