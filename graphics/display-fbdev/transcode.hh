/*
	transcode.hh
	------------
*/

#ifndef TRANSCODE_HH
#define TRANSCODE_HH

// Standard C
#include <stdint.h>


template < class UInt, int X >
void transcode_1_to_direct( const uint8_t* src, uint8_t* dst, int width )
{
	UInt* p = (UInt*) dst;
	
	while ( width > 0 )
	{
		uint8_t byte = *src++;
		
		for ( int mask = 1 << 7;  mask != 0;  mask >>= 1 )
		{
			const UInt pixel = byte & mask ? 0x00000000 : 0xFFFFFFFF;
			
			for ( int i = 0;  i < X;  ++i )
			{
				*p++ = pixel;
			}
		}
		
		width -= 8;
	}
}

template < class UInt, int X = 1 >
struct pixtet
{
	UInt pixels[ 8 * X ];
};

template < class UInt, int X >
const pixtet< UInt, X >* make_1_to_direct_table()
{
	typedef pixtet< UInt, X > pixel_unit;
	
	enum
	{
		n_octets = 256,
		
		table_size = n_octets * sizeof (pixel_unit),  // 4K, 8K, or 16K
	};
	
	pixel_unit* table = (pixel_unit*) malloc( table_size );
	
	pixel_unit* p = table;
	
	for ( int i = 0; i < 256;  ++i )
	{
		uint8_t oct = i;
		
		transcode_1_to_direct< UInt, X >( &oct, (uint8_t*) p, 8 );
		
		++p;
	}
	
	return table;
}

template < class UInt, int X >
void lookup_1_to_direct( const uint8_t* src, uint8_t* dst, int width )
{
	typedef pixtet< UInt, X > pixel_unit;
	
	static const pixel_unit* table = make_1_to_direct_table< UInt, X >();
	
	pixel_unit* p = (pixel_unit*) dst;
	
	while ( width > 0 )
	{
		*p++ = table[ *src++ ];
		
		width -= 8;
	}
}

void copy_16   ( const uint8_t* src, uint8_t* dst, int width );
void copy_16_2x( const uint8_t* src, uint8_t* dst, int width );
void copy_32   ( const uint8_t* src, uint8_t* dst, int width );
void copy_32_2x( const uint8_t* src, uint8_t* dst, int width );
void swap_32   ( const uint8_t* src, uint8_t* dst, int width );
void swap_32_2x( const uint8_t* src, uint8_t* dst, int width );

#endif
