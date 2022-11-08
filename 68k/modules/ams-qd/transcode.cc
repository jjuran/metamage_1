/*
	transcode.cc
	------------
*/

#include "transcode.hh"


Byte* downsample_8_to_1( Byte*       dst : __A0,
                         const Byte* src : __A1, UInt32 n_src_bytes )
{
	UInt32 n_dst_bytes = n_src_bytes / 8;
	UInt32 n_more_bits = n_src_bytes % 8;
	
	while ( n_dst_bytes-- > 0 )
	{
		Byte byte = 0;
		
		for ( int i = 0;  i < 8;  ++i )
		{
			byte |= (*src++ & 0x80) >> i;
		}
		
		*dst++ = byte;
	}
	
	if ( n_more_bits )
	{
		Byte  byte  = 0;
		short shift = 0;
		
		do
		{
			byte |= (*src++ & 0x80) >> shift++;
		}
		while ( --n_more_bits > 0 );
		
		*dst++ = byte;
	}
	
	return dst;
}
