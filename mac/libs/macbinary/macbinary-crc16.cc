/*
	macbinary-crc16.cc
	------------------
*/

#include "macbinary-crc16.hh"


namespace macbinary
{

enum
{
	checked_header_size = 124,
	
	magic_number = 0x1021,
};

#ifdef __MC68K__

asm
hash_t header_checksum( const byte_t* p : __A0 )
{
	MOVE.L   D3,-(SP)
	
	MOVEQ    #0,D0
	
	LEA      checked_header_size(A0),A1
	
byte_loop:
	MOVE.B   (A0)+,D1
	LSL.W    #7,D1
	
	MOVEQ    #7,D3
	
bit_loop:
	MOVE.W   D0,D2
	
	ADD.W    D1,D1
	ADD.W    D0,D0
	
	EOR.W    D1,D2
	BGE.S    high_bit_clear
	
	EORI.W   #magic_number,D0
	
high_bit_clear:
	
	DBRA     D3,bit_loop
	
	CMPA.L   A0,A1
	BNE.S    byte_loop
	
	MOVE.L   (SP)+,D3
	RTS
}

#else

hash_t header_checksum( const byte_t* p )
{
	const byte_t* end = p + checked_header_size;
	
	hash_t crc = 0;
	
	while ( p < end )
	{
		hash_t data = *p++ << 7;
		
		short i = 8;
		
		do
		{
			short prev_crc = crc;
			
			data <<= 1;
			crc  <<= 1;
			
			if ( ((short) data ^ prev_crc) < 0 )
			{
				crc ^= magic_number;
			}
		}
		while ( --i );
	}
	
	return crc;
}

#endif

}
