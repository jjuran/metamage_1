/*
	plot_cursor.cc
	--------------
*/

#include "plot_cursor.hh"

// iota
#include "iota/endian.hh"


void plot_cursor( const uint16_t*  crsr_face,
                  const uint16_t*  crsr_mask,
                  uint8_t*         addr,
                  short            shift,
                  short            h_trim,
                  short            v_skip,
                  short            n,
                  short            stride )
{
	const uint16_t* p_face = crsr_face + v_skip;
	const uint16_t* p_mask = crsr_mask + v_skip;
	
	for ( short i = 0;  i < n;  ++i )
	{
		uint16_t face = iota::u16_from_big( *p_face++ );
		uint16_t mask = iota::u16_from_big( *p_mask++ );
		
		uint16_t* q = (uint16_t*) addr;
		
		if ( h_trim >= 0 )
		{
			*q   &= iota::big_u16( ~(mask >> shift) );
			*q++ ^= iota::big_u16(   face >> shift  );
		}
		
		if ( shift  &&  h_trim <= 0 )
		{
			*q &= iota::big_u16( ~(mask << (16 - shift)) );
			*q ^= iota::big_u16(   face << (16 - shift)  );
		}
		
		addr += stride;
	}
}

void plot_cursor_to_2bpp( const uint16_t*  crsr_face,
                          const uint16_t*  crsr_mask,
                          uint8_t*         addr,
                          short            shift,
                          short            h_trim,
                          short            v_skip,
                          short            n,
                          short            stride )
{
	shift *= 2;
	
	const uint8_t conversion[] =
	{
		0x00, 0x03, 0x0C, 0x0F,
		0x30, 0x33, 0x3C, 0x3F,
		0xC0, 0xC3, 0xCC, 0xCF,
		0xF0, 0xF3, 0xFC, 0xFF,
	};
	
	const uint16_t* p_face = crsr_face + v_skip;
	const uint16_t* p_mask = crsr_mask + v_skip;
	
	for ( short i = 0;  i < n;  ++i )
	{
		uint16_t face = iota::u16_from_big( *p_face++ );
		uint16_t mask = iota::u16_from_big( *p_mask++ );
		
		uint32_t face_2bpp = 0;
		uint32_t mask_2bpp = 0;
		
		uint16_t* q = (uint16_t*) addr;
		
		for ( int j = 0;  j < 4;  ++j )
		{
			face = (face << 4) | (face >> 12);
			mask = (mask << 4) | (mask >> 12);
			
			face_2bpp <<= 8;
			mask_2bpp <<= 8;
			
			face_2bpp |= conversion[ face & 0xf ];
			mask_2bpp |= conversion[ mask & 0xf ];
		}
		
		if ( h_trim >= 0 )
		{
			uint32_t face_right = face_2bpp >> shift;
			uint32_t mask_right = mask_2bpp >> shift;
			
			*q   &= ~iota::big_u16( mask_right >> 16 );
			*q++ ^=  iota::big_u16( face_right >> 16 );
			
			*q   &= ~iota::big_u16( mask_right );
			*q++ ^=  iota::big_u16( face_right );
		}
		
		if ( shift  &&  h_trim <= 0 )
		{
			uint32_t face_left = face_2bpp << (32 - shift);
			uint32_t mask_left = mask_2bpp << (32 - shift);
			
			*q   &= ~iota::big_u16( mask_left >> 16 );
			*q++ ^=  iota::big_u16( face_left >> 16 );
			
			*q   &= ~iota::big_u16( mask_left );
			*q++ ^=  iota::big_u16( face_left );
		}
		
		addr += stride;
	}
}

void plot_cursor_to_4bpp( const uint16_t*  crsr_face,
                          const uint16_t*  crsr_mask,
                          uint8_t*         addr,
                          short            shift,
                          short            h_trim,
                          short            v_skip,
                          short            n,
                          short            stride )
{
	shift *= 4;
	
	const uint8_t conversion[] =
	{
		0x00, 0x0F,
		0xF0, 0xFF,
	};
	
	const uint16_t* p_face = crsr_face + v_skip;
	const uint16_t* p_mask = crsr_mask + v_skip;
	
	for ( short i = 0;  i < n;  ++i )
	{
		uint16_t face = iota::u16_from_big( *p_face++ );
		uint16_t mask = iota::u16_from_big( *p_mask++ );
		
		uint64_t face_4bpp = 0;
		uint64_t mask_4bpp = 0;
		
		uint16_t* q = (uint16_t*) addr;
		
		for ( int j = 0;  j < 8;  ++j )
		{
			face = (face << 2) | (face >> 14);
			mask = (mask << 2) | (mask >> 14);
			
			face_4bpp <<= 8;
			mask_4bpp <<= 8;
			
			face_4bpp |= conversion[ face & 0x3 ];
			mask_4bpp |= conversion[ mask & 0x3 ];
		}
		
		if ( h_trim >= 0 )
		{
			uint64_t face_right = face_4bpp >> shift;
			uint64_t mask_right = mask_4bpp >> shift;
			
			*q   &= ~iota::big_u16( mask_right >> 48 );
			*q++ ^=  iota::big_u16( face_right >> 48 );
			
			*q   &= ~iota::big_u16( mask_right >> 32 );
			*q++ ^=  iota::big_u16( face_right >> 32 );
			
			*q   &= ~iota::big_u16( mask_right >> 16 );
			*q++ ^=  iota::big_u16( face_right >> 16 );
			
			*q   &= ~iota::big_u16( mask_right );
			*q++ ^=  iota::big_u16( face_right );
		}
		
		if ( shift  &&  h_trim <= 0 )
		{
			uint64_t face_left = face_4bpp << (64 - shift);
			uint64_t mask_left = mask_4bpp << (64 - shift);
			
			*q   &= ~iota::big_u16( mask_left >> 48 );
			*q++ ^=  iota::big_u16( face_left >> 48 );
			
			*q   &= ~iota::big_u16( mask_left >> 32 );
			*q++ ^=  iota::big_u16( face_left >> 32 );
			
			*q   &= ~iota::big_u16( mask_left >> 16 );
			*q++ ^=  iota::big_u16( face_left >> 16 );
			
			*q   &= ~iota::big_u16( mask_left );
			*q++ ^=  iota::big_u16( face_left );
		}
		
		addr += stride;
	}
}
