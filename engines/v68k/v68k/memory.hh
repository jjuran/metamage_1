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
	
	enum function_code_t
	{
		reserved_0,
		user_data_space,
		user_program_space,
		utility_space,  // reserved for user definition
		reserved_4,
		supervisor_data_space,
		supervisor_program_space,
		cpu_space
	};
	
	enum memory_access_t
	{
		mem_exec  = 0x0,
		mem_read  = 0x1,
		mem_write = 0x2
	};
	
	class memory
	{
		public:
			virtual uint8_t* translate( uint32_t addr, uint32_t length, function_code_t fc, memory_access_t access ) const = 0;
			
			bool get_byte( uint32_t addr, uint8_t & x, function_code_t fc ) const;
			bool get_word( uint32_t addr, uint16_t& x, function_code_t fc ) const;
			bool get_long( uint32_t addr, uint32_t& x, function_code_t fc ) const;
			
			bool put_byte( uint32_t addr, uint8_t  x, function_code_t fc ) const;
			bool put_word( uint32_t addr, uint16_t x, function_code_t fc ) const;
			bool put_long( uint32_t addr, uint32_t x, function_code_t fc ) const;
			
			bool get_instruction_word( uint32_t addr, uint16_t& x, function_code_t fc ) const;
	};
	
	class low_memory_region : public memory
	{
		private:
			uint8_t* const  base;
			uint32_t const  size;
		
		public:
			low_memory_region( uint8_t* mem_base, uint32_t mem_size );
			
			uint8_t* translate( uint32_t addr, uint32_t length, function_code_t fc, memory_access_t access ) const;
	};
	
}

#endif

