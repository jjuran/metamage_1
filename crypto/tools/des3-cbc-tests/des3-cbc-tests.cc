/*
	des3-cbc-tests.cc
	-----------------
	
	Examples from https://www.rfc-editor.org/info/rfc3217/#section-3.4
*/

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// gear
#include "gear/hexadecimal.hh"

// des
#include "des/cbc.hh"
#include "des/des3.hh"


#pragma exceptions off


#define LENGTH( array )  (sizeof (array) / sizeof *(array))

#define ARRAY_LEN( array )   array, LENGTH( array )
#define ARRAY_SIZE( array )  array, sizeof array

#define STR_LEN( s )  "" s, (sizeof s - 1)


typedef uint8_t Byte;

static
void decode_hex( Byte* output, int n, const char* hex_input )
{
	using gear::decoded_hex_digit;
	
	for ( int i = 0;  i < n;  ++i )
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
		Synopsis:  des3-cbc-tests
	*/
	
	if ( argc > 1 )
	{
		return 2;
	}
	
	using namespace des;
	
	des::iv_t   iv1 = 0x5dd4cbfc96f5453b;
	des::iv_t   iv2 = 0x4adda22c79e82105;
	
	des::key_t  key[ 3 ];
	
	block_t  ptxt_1[ 4 ];
	block_t  ctxt_1[ 4 ];
	block_t  ptxt_2[ 5 ];
	block_t  ctxt_2[ 5 ];
	block_t  result[ 5 ];
	
	decode_hex( (Byte*) &key,
	            sizeof key,
	            "255e0d1c07b646df" "b3134cc843ba8aa7" "1f025b7c0838251f" );
	
	decode_hex( (Byte*) &ptxt_1,
	            sizeof ptxt_1,
	            "2923bf85e06dd6ae" "529149f1f1bae9ea" "b3a7da3d860d3e98"
	            "181b7e9686e04a4e" );
	
	decode_hex( (Byte*) &ctxt_1,
	            sizeof ctxt_1,
	            "cfc1a789c675dd2a" "b49a3204ef92cc03"
	            "5c1f973b7a7960f6" "a44dcc5f729d8449" );
	
	decode_hex( (Byte*) &ptxt_2,
	            sizeof ptxt_2,
	            "49849d725fcc4da4" "f660797a3b971f5c" "03cc92ef04329ab4"
	            "2add75c689a7c1cf" "3b45f596fccbd45d" );
	
	decode_hex( (Byte*) &ctxt_2,
	            sizeof ctxt_2,
	            "690107618ef092b3" "b48ca1796b234ae9" "fa33ebb415960403"
	            "7db5d6a84eb3aac2" "768c632775a467d4" );
	
	typedef CBC_mode< cipher_3DES, Encryption > CBC_3DES_encryption;
	typedef CBC_mode< cipher_3DES, Decryption > CBC_3DES_decryption;
	
	write( STDOUT_FILENO, STR_LEN( "1..4\n" ) );
	
	const char* p;
	
	p = "not ok 1\n";
	
	CBC_3DES_encryption( key, iv1 ).crypt_blocks( result, ARRAY_LEN( ptxt_1 ) );
	
	if ( memcmp( result, ARRAY_SIZE( ctxt_1 ) ) == 0 )
	{
		p += 4;
	}
	
	write( STDOUT_FILENO, p, strlen( p ) );
	
	p = "not ok 2\n";
	
	CBC_3DES_encryption( key, iv2 ).crypt_blocks( result, ARRAY_LEN( ptxt_2 ) );
	
	if ( memcmp( result, ARRAY_SIZE( ctxt_2 ) ) == 0 )
	{
		p += 4;
	}
	
	write( STDOUT_FILENO, p, strlen( p ) );
	
	p = "not ok 3\n";
	
	CBC_3DES_decryption( key, iv1 ).crypt_blocks( result, ARRAY_LEN( ctxt_1 ) );
	
	if ( memcmp( result, ARRAY_SIZE( ptxt_1 ) ) == 0 )
	{
		p += 4;
	}
	
	write( STDOUT_FILENO, p, strlen( p ) );
	
	p = "not ok 4\n";
	
	CBC_3DES_decryption( key, iv2 ).crypt_blocks( result, ARRAY_LEN( ctxt_2 ) );
	
	if ( memcmp( result, ARRAY_SIZE( ptxt_2 ) ) == 0 )
	{
		p += 4;
	}
	
	write( STDOUT_FILENO, p, strlen( p ) );
	
	return 0;
}
