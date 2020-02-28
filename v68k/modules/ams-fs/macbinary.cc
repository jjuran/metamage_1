/*
	macbinary.cc
	------------
*/

#include "macbinary.hh"


static
uint16_t CalcCRC( const uint8_t* dataBuf, uint32_t size )
{
	//#define CCITT_CRC_GEN	0x1021
	
	const unsigned long kMagicNumber = 0x1021;
	
	register const uint8_t* dataEnd = dataBuf + size;
	
	register unsigned long crc = 0;
	
	while ( dataBuf != dataEnd )
	{
		register unsigned long dataByte = *dataBuf++;
		dataByte <<= 8;
		
		register long i = 8;
		
		do
		{	
			register long bit = dataByte;
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
bool crc_check( const header& h )
{
	return CalcCRC( (const uint8_t*) &h, 124 ) == h.crc;
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
	
	if ( h.extensions )
	{
		return 0;  // TODO:  Check for folder archives
	}
	
	if ( ! crc_check( h ) )
	{
		return h.zeroByte82 == 0;  // version 1, or invalid
	}
	
	int8_t vers = 2 + (h.formatSig == 'mBIN');  // version 2 or 3
	
	return vers;
}

}
