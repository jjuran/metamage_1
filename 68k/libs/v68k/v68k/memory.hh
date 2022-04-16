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
		mem_write = 0x2,
		mem_update = 0x3
	};
	
	typedef uint32_t         addr_t;
	typedef function_code_t  fc_t;
	typedef memory_access_t  mem_t;
	
	typedef uint8_t* (*translate_f)( addr_t a, uint32_t n, fc_t fc, mem_t mem );
	
	class memory
	{
		private:
			translate_f its_translate;
		
		public:
			memory( translate_f f ) : its_translate( f )
			{
			}
			
			uint8_t* translate( addr_t a, uint32_t n, fc_t fc, mem_t mem ) const
			{
				return its_translate( a, n, fc, mem );
			}
			
			bool get_byte( addr_t addr, uint8_t & x, fc_t fc ) const;
			bool get_word( addr_t addr, uint16_t& x, fc_t fc ) const;
			bool get_long( addr_t addr, uint32_t& x, fc_t fc ) const;
			
			bool put_byte( addr_t addr, uint8_t  x, fc_t fc ) const;
			bool put_word( addr_t addr, uint16_t x, fc_t fc ) const;
			bool put_long( addr_t addr, uint32_t x, fc_t fc ) const;
			
			bool get_instruction_word( addr_t addr, uint16_t& x, fc_t fc ) const;
	};
	
}

#endif
