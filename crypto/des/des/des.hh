/*
	des.hh
	------
*/

#ifndef DES_DES_HH
#define DES_DES_HH

// des
#include "des/types.hh"


namespace des
{
	
	struct cipher_DES
	{
		uint64_t keys[ 16 ];
		
		cipher_DES( Encryption, const key_t* key )
		{
			set_encryption_key( key );
		}
		
		cipher_DES( Decryption, const key_t* key )
		{
			set_decryption_key( key );
		}
		
		void set_encryption_key( const key_t* key );
		void set_decryption_key( const key_t* key );
		
		void crypt_block( uint64_t& text ) const;
	};
	
}

#endif
