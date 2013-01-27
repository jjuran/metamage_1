/*
	endian.hh
	---------
*/

#ifndef V68K_ENDIAN_HH
#define V68K_ENDIAN_HH

// C99
#include <stdint.h>

// config
#include "config/endian.h"


namespace v68k
{
	
	inline bool is_little_endian()
	{
	#ifdef CONFIG_LITTLE_ENDIAN
		
		return CONFIG_LITTLE_ENDIAN;
		
	#else
	#warning Endianness unknown at compile time, will check (inefficiently) at runtime.
	#endif
		
		const int x = 1;
		
		return *(const char*) &x;
	}
	
	inline uint16_t swap_2_bytes( uint16_t u16 )
	{
		return u16 << 8 | u16 >> 8;
	}
	
	inline uint32_t swap_4_bytes( uint32_t u32 )
	{
		return    u32 << 24
		       | (u32 <<  8 & 0x00FF0000)
		       | (u32 >>  8 & 0x0000FF00)
		       |  u32 >> 24;
	}
	
	
	inline uint16_t swap_2_bytes_big( uint16_t u16 )
	{
		return is_little_endian() ? swap_2_bytes( u16 )
		                          :               u16;
	}
	
	inline uint32_t swap_4_bytes_big( uint32_t u32 )
	{
		return is_little_endian() ? swap_4_bytes( u32 )
		                          :               u32;
	}
	
	
	inline uint16_t big_word( uint16_t word )
	{
		return swap_2_bytes_big( word );
	}
	
	inline uint16_t word_from_big( uint16_t word )
	{
		return swap_2_bytes_big( word );
	}
	
	inline uint32_t big_longword( uint32_t longword )
	{
		return swap_4_bytes_big( longword );
	}
	
	inline uint32_t longword_from_big( uint32_t longword )
	{
		return swap_4_bytes_big( longword );
	}
	
}

#endif

