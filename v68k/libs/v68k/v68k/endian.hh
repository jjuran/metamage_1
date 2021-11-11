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
	
	using iota::is_little_endian;
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
	
	inline uint8_t& select_byte( uint16_t& word, int i )
	{
		return ((uint8_t*) &word)[ i == is_little_endian() ];
	}
	
	inline uint16_t& select_word( uint32_t& longword, int i )
	{
		uint8_t* p = (uint8_t*) &longword;
		
		if ( i == is_little_endian() )
		{
			p += 2;
		}
		
		return *(uint16_t*) p;
	}
	
	inline uint32_t& select_long( uint64_t& quadword, int i )
	{
		uint8_t* p = (uint8_t*) &quadword;
		
		if ( i == is_little_endian() )
		{
			p += 4;
		}
		
		return *(uint32_t*) p;
	}
	
	inline uint8_t& high_byte( uint16_t& x )  { return select_byte( x, 1 ); }
	inline uint8_t& low_byte ( uint16_t& x )  { return select_byte( x, 0 ); }
	
	inline uint16_t& high_word( uint32_t& x )  { return select_word( x, 1 ); }
	inline uint16_t& low_word ( uint32_t& x )  { return select_word( x, 0 ); }
	
	inline uint32_t& high_long( uint64_t& x )  { return select_long( x, 1 ); }
	inline uint32_t& low_long ( uint64_t& x )  { return select_long( x, 0 ); }
	
}

#endif
