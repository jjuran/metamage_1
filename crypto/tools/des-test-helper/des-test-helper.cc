/*
	des-test-helper.cc
	------------------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// gear
#include "gear/hexadecimal.hh"

// des
#include "des/des.hh"


#pragma exceptions off


typedef uint8_t Byte;

static
void decode_hex_x8( Byte* output, const char* hex_input )
{
	using gear::decoded_hex_digit;
	
	for ( int i = 0;  i < 8;  ++i )
	{
		Byte byte;
		
		byte  = decoded_hex_digit( *hex_input++ ) << 4;
		byte |= decoded_hex_digit( *hex_input++ );
		
		*output++ = byte;
	}
}

int main( int argc, char** argv )
{
	/*
		Synopsis:  des-test-helper encrypt|decrypt <key> <input>
	*/
	
	if ( argc != 4 )
	{
		return 2;
	}
	
	const char* operation = argv[ 1 ];
	const char* key_param = argv[ 2 ];
	const char* block_hex = argv[ 3 ];
	
	bool decrypting;
	
	if ( strcmp( operation, "encrypt" ) == 0 )
	{
		decrypting = false;
	}
	else if ( strcmp( operation, "decrypt" ) == 0 )
	{
		decrypting = true;
	}
	else
	{
		return 2;
	}
	
	if ( strlen( key_param ) != 16  ||  strlen( block_hex ) != 16 )
	{
		return 2;
	}
	
	typedef unsigned char Byte;
	
	using namespace des;
	
	des::key_t    key;
	des::block_t  text;
	
	decode_hex_x8( (Byte*) &key,  key_param );
	decode_hex_x8( (Byte*) &text, block_hex );
	
	uint64_t block = text;
	
	if ( decrypting )
	{
		cipher_DES( decryption, &key ).crypt_block( block );
	}
	else
	{
		cipher_DES( encryption, &key ).crypt_block( block );
	}
	
	text = block;
	
	char hex_buffer[ 16 + 1 ];
	
	char* p = gear::hexpcpy_upper( hex_buffer, &text, sizeof text );
	
	*p = '\n';
	
	(void) write( STDOUT_FILENO, hex_buffer, sizeof hex_buffer );
	
	return 0;
}
