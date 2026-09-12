/*
	des-cbc-test-helper.cc
	----------------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <string.h>

// des
#include "des/cbc.hh"
#include "des/des.hh"


#pragma exceptions off


int main( int argc, char** argv )
{
	/*
		Synopsis:  des-cbc-test-helper encrypt|decrypt <path>
	*/
	
	if ( argc != 3 )
	{
		return 2;
	}
	
	const char* cryption = argv[ 1 ];
	const char* pathname = argv[ 2 ];
	
	bool decrypting;
	
	if ( strcmp( cryption, "encrypt" ) == 0 )
	{
		decrypting = false;
	}
	else if ( strcmp( cryption, "decrypt" ) == 0 )
	{
		decrypting = true;
	}
	else
	{
		return 2;
	}
	
	int fd = open( pathname, O_RDONLY );
	
	if ( fd < 0 )
	{
		return 1;
	}
	
	using namespace des;
	
	des::iv_t   iv  = 0xd6b3a81556f3d742;
	des::key_t  key = 0x8316f8b6b3dc34e0;
	
	typedef CBC_mode< cipher_DES, Encryption > CBC_DES_encryption;
	typedef CBC_mode< cipher_DES, Decryption > CBC_DES_decryption;
	
	CBC_DES_encryption encryptor( &key, iv );
	CBC_DES_decryption decryptor( &key, iv );
	
	block_t  block;
	ssize_t  n_read;
	
	while ( (n_read = read( fd, &block, sizeof block )) == sizeof block )
	{
		if ( decrypting )
		{
			decryptor.crypt_block( &block );
		}
		else
		{
			encryptor.crypt_block( &block );
		}
		
		write( STDOUT_FILENO, &block, sizeof block );
	}
	
	if ( ! decrypting  &&  n_read > 0 )
	{
		memset( (char*) &block + n_read, '\0', sizeof block - n_read );
		
		encryptor.crypt_block( &block );
		
		write( STDOUT_FILENO, &block, sizeof block );
	}
	
	return 0;
}
