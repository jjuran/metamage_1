/*
	endian.hh
	---------
*/

#ifndef V68K_ENDIAN_HH
#define V68K_ENDIAN_HH

// iota
#include "iota/endian.hh"


namespace v68k
{
	
	using iota::swap_2_bytes_big;
	using iota::swap_4_bytes_big;
	
	
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

