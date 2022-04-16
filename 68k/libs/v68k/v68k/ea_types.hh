/*
	ea_types.hh
	-----------
*/

#ifndef V68K_EATYPES_HH
#define V68K_EATYPES_HH

// C99
#include <stdint.h>


namespace v68k
{
	
	inline bool ea_is_data_register( uint16_t mode, uint16_t n = 0 )
	{
		return mode == 0;
	}
	
	inline bool ea_is_address_register( uint16_t mode, uint16_t n = 0 )
	{
		return mode == 1;
	}
	
	inline bool ea_is_postincrement( uint16_t mode, uint16_t n = 0 )
	{
		return mode == 3;
	}
	
	inline bool ea_is_predecrement( uint16_t mode, uint16_t n = 0 )
	{
		return mode == 4;
	}
	
	
	inline bool ea_is_valid( uint16_t mode, uint16_t n )
	{
		return mode < 7  ||  n <= 4;
	}
	
	inline bool ea_is_register( uint16_t mode, uint16_t n = 0 )
	{
		return mode <= 1;
	}
	
	inline bool ea_is_data( uint16_t mode, uint16_t n )
	{
		return ea_is_valid( mode, n )  &&  !ea_is_address_register( mode );
	}
	
	inline bool ea_is_memory( uint16_t mode, uint16_t n )
	{
		return ea_is_valid( mode, n )  &&  !ea_is_register( mode );
	}
	
	inline bool ea_is_control( uint16_t mode, uint16_t n )
	{
		return mode == 2  ||  mode - 5u <= 1  ||  (mode == 7  &&  n < 4);
	}
	
	inline bool ea_is_alterable( uint16_t mode, uint16_t n )
	{
		return mode < 7  ||  n <= 1;
	}
	
	inline bool ea_is_data_alterable( uint16_t mode, uint16_t n )
	{
		return ea_is_alterable( mode, n )  &&  !ea_is_address_register( mode );
	}
	
	inline bool ea_is_memory_alterable( uint16_t mode, uint16_t n )
	{
		return mode > 1  &&  ea_is_alterable( mode, n );
	}
	
}

#endif
