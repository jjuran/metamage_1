/*
	endian.hh
	---------
*/

#ifndef IOTA_ENDIAN_HH
#define IOTA_ENDIAN_HH

// Standard C
#include <stdint.h>

// config
#include "config/endian.h"


namespace iota
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
	
	inline uint64_t swap_8_bytes( uint64_t u64 )
	{
		return    u64 << 56
		       | (u64 << 40 & 0x00FF000000000000ull)
		       | (u64 << 24 & 0x0000FF0000000000ull)
		       | (u64 <<  8 & 0x000000FF00000000ull)
		       | (u64 >>  8 & 0x00000000FF000000ull)
		       | (u64 >> 24 & 0x0000000000FF0000ull)
		       | (u64 >> 40 & 0x000000000000FF00ull)
		       |  u64 >> 56;
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
	
	inline uint64_t swap_8_bytes_big( uint64_t u64 )
	{
		return is_little_endian() ? swap_8_bytes( u64 )
		                          :               u64;
	}
	
	
	inline uint16_t swap_2_bytes_little( uint16_t u16 )
	{
		return is_little_endian() ?               u16
		                          : swap_2_bytes( u16 );
	}
	
	inline uint32_t swap_4_bytes_little( uint32_t u32 )
	{
		return is_little_endian() ?               u32
		                          : swap_4_bytes( u32 );
	}
	
	inline uint64_t swap_8_bytes_little( uint64_t u64 )
	{
		return is_little_endian() ?               u64
		                          : swap_8_bytes( u64 );
	}
	
	
	inline uint16_t big_u16( uint16_t u16 )
	{
		return swap_2_bytes_big( u16 );
	}
	
	inline uint16_t u16_from_big( uint16_t u16 )
	{
		return swap_2_bytes_big( u16 );
	}
	
	inline uint32_t big_u32( uint32_t u32 )
	{
		return swap_4_bytes_big( u32 );
	}
	
	inline uint32_t u32_from_big( uint32_t u32 )
	{
		return swap_4_bytes_big( u32 );
	}
	
	inline uint64_t big_u64( uint64_t u64 )
	{
		return swap_8_bytes_big( u64 );
	}
	
	inline uint64_t u64_from_big( uint64_t u64 )
	{
		return swap_8_bytes_big( u64 );
	}
	
	
	inline uint16_t little_u16( uint16_t u16 )
	{
		return swap_2_bytes_little( u16 );
	}
	
	inline uint16_t u16_from_little( uint16_t u16 )
	{
		return swap_2_bytes_little( u16 );
	}
	
	inline uint32_t little_u32( uint32_t u32 )
	{
		return swap_4_bytes_little( u32 );
	}
	
	inline uint32_t u32_from_little( uint32_t u32 )
	{
		return swap_4_bytes_little( u32 );
	}
	
	inline uint64_t little_u64( uint64_t u64 )
	{
		return swap_8_bytes_little( u64 );
	}
	
	inline uint64_t u64_from_little( uint64_t u64 )
	{
		return swap_8_bytes_little( u64 );
	}
	
	template < class Int >
	class big_endian
	{
		private:
			Int data;
		
		public:
			typedef Int underlying_type;
			
			big_endian() : data()
			{
			}
			
			void assign( Int x );
			
			big_endian( Int x )  { assign( x ); }
			
			big_endian& operator=( Int x )  { assign( x );  return *this; }
			
			operator Int() const;
	};
	
	template < class Int >
	void big_endian_conversion_unsupported( Int x );
	
	template < class Int >
	void big_endian< Int >::assign( Int x )
	{
		switch ( sizeof x )
		{
			default: big_endian_conversion_unsupported( x );
			
			case 2:  data = big_u16( x );  break;
			case 4:  data = big_u32( x );  break;
		}
	}
	
	template < class Int >
	big_endian< Int >::operator Int() const
	{
		switch ( sizeof data )
		{
			default: big_endian_conversion_unsupported( data );
			
			case 2:  return u16_from_big( data );
			case 4:  return u32_from_big( data );
		}
	}
	
	template < class Int >
	struct big_endian_conversion
	{
	#ifdef __MWERKS__
	
		typedef Int type;
		
	#else
		
		typedef big_endian< Int > type;
		
	#endif
	};
	
}

#endif
