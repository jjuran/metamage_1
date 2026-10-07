/*
	bytes.cc
	--------
*/

#include "rle1/bytes.hh"


namespace rle1
{

typedef   signed char  int8_t;
typedef unsigned char uint8_t;

size_t compressed_bytes_size( const byte_t* p, size_t n )
{
	size_t   size = 0;
	uint8_t  rlen = 0;  // current run length
	byte_t   prev = 0;
	
	while ( n-- > 0 )
	{
		byte_t next = *p++;
		
		if ( next == prev  &&  ++rlen < 128  &&  n > 0 )
		{
			continue;
		}
		
		size += rlen > 0;
		
		rlen = next != prev;
		prev = next;
	}
	
	return size;
}

void compress_bytes( codon_t* out, const byte_t* in, size_t n )
{
	uint8_t  rlen = 0;  // current run length
	byte_t   prev = 0;
	
	while ( n-- > 0 )
	{
		byte_t next = *in++;
		
		if ( next == prev  &&  ++rlen < 128  &&  n > 0 )
		{
			continue;
		}
		
		if ( rlen > 0 )
		{
			*out++ = (prev & 0x80) | (rlen - 1);
		}
		
		rlen = next != prev;
		prev = next;
	}
}

size_t uncompressed_bytes_size( const codon_t* p, size_t n )
{
	size_t size = n;
	
	while ( n-- > 0 )
	{
		size += (uint8_t) *p++ & 0x7f;
	}
	
	return size;
}

void decompress_bytes( byte_t* out, const codon_t* in, size_t n )
{
	byte_t byte = 0;
	
	while ( n-- > 0 )
	{
		uint8_t codon = *in++;
		
		if ( (int8_t) (codon ^ byte) < 0 )
		{
			byte = ~byte;
		}
		
		int run_len_1 = codon & 0x7f;  // run length - 1
		
		do
		{
			*out++ = byte;
		}
		while ( --run_len_1 >= 0 );
	}
}

}
