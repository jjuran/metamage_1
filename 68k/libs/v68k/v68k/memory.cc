/*
	memory.cc
	---------
*/

#include "v68k/memory.hh"

// v68k
#include "v68k/endian.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k
{
	
	static inline uint8_t read_byte( const uint8_t* addr )
	{
		return *addr;
	}
	
	static inline void write_byte( uint8_t* addr, uint8_t x )
	{
		*addr = x;
	}
	
	
	static inline uint16_t read_word_aligned( const uint8_t* addr )
	{
		return *(const uint16_t*) addr;
	}
	
	static inline void write_word_aligned( uint8_t* addr, uint16_t x )
	{
		*(uint16_t*) addr = x;
	}
	
	static inline uint32_t read_long_aligned( const uint8_t* addr )
	{
		return *(const uint32_t*) addr;
	}
	
	static inline void write_long_aligned( uint8_t* addr, uint32_t x )
	{
		*(uint32_t*) addr = x;
	}
	
	
	static inline uint16_t read_big_word_aligned( const uint8_t* addr )
	{
		return word_from_big( read_word_aligned( addr ) );
	}
	
	static inline void write_big_word_aligned( uint8_t* addr, uint16_t x )
	{
		write_word_aligned( addr, big_word( x ) );
	}
	
	static inline uint32_t read_big_longword_aligned( const uint8_t* addr )
	{
		return longword_from_big( read_long_aligned( addr ) );
	}
	
	static inline void write_big_long_aligned( uint8_t* addr, uint32_t x )
	{
		write_long_aligned( addr, big_longword( x ) );
	}
	
	
	static inline uint16_t read_big_word_unaligned( const uint8_t* addr )
	{
		return addr[0] << 8 | addr[1];
	}
	
	static inline void write_big_word_unaligned( uint8_t* addr, uint16_t x )
	{
		addr[0] = x >> 8;
		addr[1] = x;
	}
	
	static inline uint32_t read_big_long_unaligned( const uint8_t* addr )
	{
		const uint32_t big_endian = addr[0] << 24
		                          | addr[1] << 16
		                          | addr[2] <<  8
		                          | addr[3];
		
		return big_endian;
	}
	
	static inline void write_big_long_unaligned( uint8_t* addr, uint32_t x )
	{
		addr[0] = x >> 24;
		addr[1] = x >> 16;
		addr[2] = x >>  8;
		addr[3] = x;
	}
	
	
	bool memory::get_byte( uint32_t addr, uint8_t& x, function_code_t fc ) const
	{
		if ( const uint8_t* p = translate( addr, sizeof (uint8_t), fc, mem_read ) )
		{
			x = read_byte( p );
			
			return true;
		}
		
		return false;
	}
	
	bool memory::get_word( uint32_t addr, uint16_t& x, function_code_t fc ) const
	{
		if ( const uint8_t* p = translate( addr, sizeof (uint16_t), fc, mem_read ) )
		{
			x = read_big_word_unaligned( p );
			
			return true;
		}
		
		return false;
	}
	
	bool memory::get_long( uint32_t addr, uint32_t& x, function_code_t fc ) const
	{
		if ( const uint8_t* p = translate( addr, sizeof (uint32_t), fc, mem_read ) )
		{
			x = read_big_long_unaligned( p );
			
			return true;
		}
		
		return false;
	}
	
	
	bool memory::put_byte( uint32_t addr, uint8_t x, function_code_t fc ) const
	{
		if ( uint8_t* p = translate( addr, sizeof (uint8_t), fc, mem_write ) )
		{
			write_byte( p, x );
			
			translate( addr, sizeof (uint8_t), fc, mem_update );
			
			return true;
		}
		
		return false;
	}
	
	bool memory::put_word( uint32_t addr, uint16_t x, function_code_t fc ) const
	{
		if ( uint8_t* p = translate( addr, sizeof (uint16_t), fc, mem_write ) )
		{
			write_big_word_unaligned( p, x );
			
			translate( addr, sizeof (uint16_t), fc, mem_update );
			
			return true;
		}
		
		return false;
	}
	
	bool memory::put_long( uint32_t addr, uint32_t x, function_code_t fc ) const
	{
		if ( uint8_t* p = translate( addr, sizeof (uint32_t), fc, mem_write ) )
		{
			write_big_long_unaligned( p, x );
			
			translate( addr, sizeof (uint32_t), fc, mem_update );
			
			return true;
		}
		
		return false;
	}
	
	
	bool memory::get_instruction_word( uint32_t addr, uint16_t& x, function_code_t fc ) const
	{
		if ( const uint8_t* p = translate( addr, sizeof (uint16_t), fc, mem_exec ) )
		{
			x = read_big_word_aligned( p );
			
			return true;
		}
		
		return false;
	}
	
}
