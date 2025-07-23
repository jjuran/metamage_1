/*
	transcode.hh
	------------
*/

#ifndef TRANSCODE_HH
#define TRANSCODE_HH

// Standard C
#include <stdint.h>


extern uint8_t monochrome_toggle;

extern uint32_t palette_entries[ 256 ];

extern const uint8_t monochrome_1bit[];
extern const uint8_t monochrome_2bit[];
extern const uint8_t monochrome_4bit[];

inline
uint16_t downsample( uint32_t zrgb )
{
	uint16_t r = (zrgb & 0x00f80000) >> (16 + 3 - 11);
	uint16_t g = (zrgb & 0x0000fc00) >> ( 8 + 2 -  5);
	uint16_t b = (zrgb & 0x000000f8) >> ( 0 + 3 -  0);
	
	return r | g | b;
}

template < class UInt, int bpp, int X >
void transcode_N_to_direct( const uint8_t* src, uint8_t* dst, int width )
{
	const bool gray = palette_entries[ 0 ] == 0;
	
	const uint8_t* ramp = bpp == 1 ? monochrome_1bit
	                    : bpp == 2 ? monochrome_2bit
	                    : bpp == 4 ? monochrome_4bit
	                    :            0;  // NULL
	
	UInt* p = (UInt*) dst;
	
	while ( width > 0 )
	{
		uint8_t byte = *src++ ^ monochrome_toggle;
		
		const uint8_t mask = (1 << bpp) - 1;
		
		for ( int n_bits = 8;  (n_bits -= bpp) >= 0; )
		{
			const uint8_t i = (byte >> n_bits) & mask;
			
			const uint32_t zrgb = gray ? (bpp < 8 ? ramp[ i ] : byte) * 0x010101
			                           : palette_entries[ i ];
			
			const UInt pixel = sizeof (UInt) == 4 ? 0xFF000000 | zrgb
			                                      : downsample( zrgb );
			
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
void rgba_32   ( const uint8_t* src, uint8_t* dst, int width );
void rgba_32_2x( const uint8_t* src, uint8_t* dst, int width );
void both_32   ( const uint8_t* src, uint8_t* dst, int width );
void both_32_2x( const uint8_t* src, uint8_t* dst, int width );

void rgb555_BE_to_565_LE   ( const uint8_t* src, uint8_t* dst, int width );
void rgb555_BE_to_565_LE_2x( const uint8_t* src, uint8_t* dst, int width );

void rgb555_LE_to_565_LE   ( const uint8_t* src, uint8_t* dst, int width );
void rgb555_LE_to_565_LE_2x( const uint8_t* src, uint8_t* dst, int width );

void rgb555_BE_to_32   ( const uint8_t* src, uint8_t* dst, int width );
void rgb555_BE_to_32_2x( const uint8_t* src, uint8_t* dst, int width );

void rgb555_LE_to_32   ( const uint8_t* src, uint8_t* dst, int width );
void rgb555_LE_to_32_2x( const uint8_t* src, uint8_t* dst, int width );

void rgb565_LE_to_32   ( const uint8_t* src, uint8_t* dst, int width );
void rgb565_LE_to_32_2x( const uint8_t* src, uint8_t* dst, int width );

#endif
