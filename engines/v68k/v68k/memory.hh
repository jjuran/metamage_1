/*
	memory.hh
	---------
*/

#ifndef V68K_MEMORY_HH
#define V68K_MEMORY_HH

// C99
#include <stdint.h>


namespace v68k
{
	
	class memory
	{
		private:
			uint8_t* const  base;
			uint32_t const  size;
		
		public:
			memory( uint8_t* mem_base, uint32_t mem_size );
			
			uint8_t* translate( uint32_t addr, uint32_t length ) const;
			
			bool get_byte( uint32_t addr, uint8_t & x ) const;
			bool get_word( uint32_t addr, uint16_t& x ) const;
			bool get_long( uint32_t addr, uint32_t& x ) const;
			
			bool put_byte( uint32_t addr, uint8_t  x ) const;
			bool put_word( uint32_t addr, uint16_t x ) const;
			bool put_long( uint32_t addr, uint32_t x ) const;
			
			bool get_instruction_word( uint32_t addr, uint16_t& next ) const;
	};
	
}

#endif

