/*
	des3.cc
	-------
*/

#include "des/des3.hh"

// des
#include "des/common.hh"
#include "des/subkeys.hh"


#pragma exceptions off


namespace des
{

void cipher_3DES::set_encryption_key( const key_t* key )
{
	make_subkeys_straight( keys[ 0 ], key[ 0 ] );
	make_subkeys_reversed( keys[ 1 ], key[ 1 ] );
	make_subkeys_straight( keys[ 2 ], key[ 2 ] );
}

void cipher_3DES::set_decryption_key( const key_t* key )
{
	make_subkeys_reversed( keys[ 0 ], key[ 2 ] );
	make_subkeys_straight( keys[ 1 ], key[ 1 ] );
	make_subkeys_reversed( keys[ 2 ], key[ 0 ] );
}

void cipher_3DES::crypt_block( uint64_t& text ) const
{
	uint64_t temp;
	
	initial_permutation( temp, text );
	
	crypt_block_core( keys[ 0 ], temp );
	crypt_block_core( keys[ 1 ], temp );
	crypt_block_core( keys[ 2 ], temp );
	
	final_permutation( text, temp );
}

}  // namespace des
