/*
	macbinary.cc
	------------
*/

#include "macbinary.hh"

// Standard C
#include <string.h>

// iota
#include "iota/endian.hh"


static
uint16_t CalcCRC( const uint8_t* dataBuf, uint32_t size )
{
	//#define CCITT_CRC_GEN	0x1021
	
	const unsigned long kMagicNumber = 0x1021;
	
	const uint8_t* dataEnd = dataBuf + size;
	
	unsigned long crc = 0;
	
	while ( dataBuf != dataEnd )
	{
		unsigned long dataByte = *dataBuf++;
		dataByte <<= 8;
		
		long i = 8;
		
		do
		{	
			long bit = dataByte;
			dataByte += dataByte;
			bit ^= crc;
			crc += crc;
			
			if ( bit &= 0x8000 )
			{
				crc ^= kMagicNumber;
			}
		}
		while ( --i );
	}
	
	return crc;
}

namespace macbinary
{

static inline
bool sig_check( const header& h )
{
#ifdef __MACOS__
	
	return h.formatSig == 'mBIN';
	
#else
	
	return memcmp( h.formatSig, "mBIN", 4 ) == 0;
	
#endif
}

static inline
bool crc_check( const header& h )
{
	return CalcCRC( (const uint8_t*) &h, 124 ) == iota::u16_from_big( h.crc );
}

int8_t version( const header& h )
{
	const uint8_t filename_len = h.filename[ 0 ];
	
	if ( filename_len <= 0  ||  filename_len > 63 )
	{
		return 0;
	}
	
	if ( h.zeroByte74 != 0 )
	{
		return 0;
	}
	
	enum { kMacBinaryPlusMask = 0x1 };
	
	if ( ! crc_check( h ) )
	{
		return (h.extensions | h.zeroByte82) == 0;  // version 1, or invalid
	}
	
	int8_t vers = 2 + sig_check( h );  // version 2 or 3
	
	if ( h.extensions & ~kMacBinaryPlusMask )
	{
		return 0;  // unrecognized extension
	}
	
	if ( h.extensions )
	{
		if ( memcmp( h.fileType, "fold\xFF\xFF\xFF\xFF", 8 ) != 0 )
		{
			return 0;
		}
		
		vers |= 0x80;
	}
	
	return vers;
}

}
