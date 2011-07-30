/*
	endian.hh
	---------
*/

#ifndef V68K_ENDIAN_HH
#define V68K_ENDIAN_HH

// C99
#include <stdint.h>


namespace v68k
{
	
	inline uint16_t swap_2_bytes( uint16_t word )
	{
		return word << 8 | word >> 8;
	}
	
	inline uint32_t swap_4_bytes( uint32_t longword )
	{
		return   longword << 24
		       | longword <<  8 & 0x00FF0000
		       | longword >>  8 & 0x0000FF00
		       | longword >> 24;
	}
	
	
	inline uint16_t toggle_word_byte_order( uint16_t word )
	{
	#ifdef __LITTLE_ENDIAN__
		
		return swap_2_bytes( word );
		
	#else
	
		return word;
	
	#endif
	}
	
	inline uint32_t toggle_longword_byte_order( uint32_t longword )
	{
	#ifdef __LITTLE_ENDIAN__
		
		return swap_4_bytes( longword );
		
	#else
	
		return longword;
	
	#endif
	}
	
	
	inline uint16_t big_word( uint16_t word )
	{
		return toggle_word_byte_order( word );
	}
	
	inline uint16_t word_from_big( uint16_t word )
	{
		return toggle_word_byte_order( word );
	}
	
	inline uint32_t big_longword( uint32_t longword )
	{
		return toggle_longword_byte_order( longword );
	}
	
	inline uint32_t longword_from_big( uint32_t longword )
	{
		return toggle_longword_byte_order( longword );
	}
	
}

#endif

