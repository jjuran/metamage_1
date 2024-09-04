/*
	macbinary-crc16.hh
	------------------
*/

#ifndef MACBINARYCRC16_HH
#define MACBINARYCRC16_HH


namespace macbinary
{
	
	typedef unsigned char  byte_t;
	typedef unsigned short hash_t;
	
	/*
		The size is omitted, because it's fixed at 124 bytes.
	*/
	
#ifdef __MC68K__
	#pragma parameter header_checksum( __A0 )
#endif
	
	hash_t header_checksum( const byte_t* p );
	
	inline
	hash_t header_checksum( const void* p )
	{
		return header_checksum( (const byte_t*) p );
	}
	
}

#endif
