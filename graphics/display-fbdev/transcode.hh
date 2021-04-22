/*
	transcode.hh
	------------
*/

#ifndef TRANSCODE_HH
#define TRANSCODE_HH

// Standard C
#include <stdint.h>


extern const uint8_t monochrome_1bit[];
extern const uint8_t monochrome_2bit[];
extern const uint8_t monochrome_4bit[];

template < class UInt, int bpp, int X >
void transcode_N_to_direct( const uint8_t* src, uint8_t* dst, int width )
{
	const uint8_t* palette = bpp == 1 ? monochrome_1bit
	                       : bpp == 2 ? monochrome_2bit
	                       : bpp == 4 ? monochrome_4bit
	                       :            0;  // NULL
	
	UInt* p = (UInt*) dst;
	
	while ( width > 0 )
	{
		uint8_t byte = ~*src++;
		
		int n_pixels = 8 / bpp;
		
		for ( int mask = (1 << bpp) - 1 << 8 - bpp;  n_pixels--;  mask >>= bpp )
		{
			uint8_t luma = bpp < 8 ? palette[ (byte & mask) >> n_pixels * bpp ]
			                       : byte;
			
			const UInt pixel = luma * 0x01010101;
			
			for ( int i = 0;  i < X;  ++i )
			{
				*p++ = pixel;
			}
		}
		
		width -= 8;
	}
}

template < class UInt, int bpp, int X = 1 >
struct pixtet
{
	UInt pixels[ 8 / bpp * X ];
};

template < class UInt, int bpp, int X >
const pixtet< UInt, bpp, X >* make_N_to_direct_table()
{
	typedef pixtet< UInt, bpp, X > pixel_unit;
	
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
		
		transcode_N_to_direct< UInt, bpp, X >( &oct, (uint8_t*) p, 8 / bpp );
		
		++p;
	}
	
	return table;
}

template < class UInt, int bpp, int X >
void lookup_N_to_direct( const uint8_t* src, uint8_t* dst, int width )
{
	typedef pixtet< UInt, bpp, X > pixel_unit;
	
	static const pixel_unit* table = make_N_to_direct_table< UInt, bpp, X >();
	
	pixel_unit* p = (pixel_unit*) dst;
	
	while ( width > 0 )
	{
		*p++ = table[ *src++ ];
		
		width -= 8 / bpp;
	}
}

void copy_16   ( const uint8_t* src, uint8_t* dst, int width );
void copy_16_2x( const uint8_t* src, uint8_t* dst, int width );
void copy_32   ( const uint8_t* src, uint8_t* dst, int width );
void copy_32_2x( const uint8_t* src, uint8_t* dst, int width );
void swap_32   ( const uint8_t* src, uint8_t* dst, int width );
void swap_32_2x( const uint8_t* src, uint8_t* dst, int width );

#endif
