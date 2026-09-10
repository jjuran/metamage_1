/*
	des3.hh
	-------
*/

#ifndef DES_DES3_HH
#define DES_DES3_HH

// Standard C
#include <stdint.h>

// des
#include "des/types.hh"


namespace des
{
	
	struct cipher_3DES
	{
		uint64_t keys[ 3 ][ 16 ];
		
		cipher_3DES( Encryption, const key_t* key )
		{
			set_encryption_key( key );
		}
		
		cipher_3DES( Decryption, const key_t* key )
		{
			set_decryption_key( key );
		}
		
		void set_encryption_key( const key_t* key );
		void set_decryption_key( const key_t* key );
		
		void crypt_block( uint64_t& text ) const;
	};
	
}

#endif
