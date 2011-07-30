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
	
	class odd_memory_address {};
	
	class unmapped_memory_access {};
	
	
	class memory
	{
		private:
			uint8_t* const  base;
			uint32_t const  size;
		
		public:
			memory( uint8_t* mem_base, uint32_t mem_size );
			
			uint8_t* translate( uint32_t addr ) const;
			
			uint8_t  get_byte( uint32_t addr ) const;
			uint16_t get_word( uint32_t addr ) const;
			uint32_t get_long( uint32_t addr ) const;
			
			void put_byte( uint32_t addr, uint8_t  x ) const;
			void put_word( uint32_t addr, uint16_t x ) const;
			void put_long( uint32_t addr, uint32_t x ) const;
			
			uint16_t get_instruction_word( uint32_t addr ) const;
	};
	
}

#endif

