/*
	icon.hh
	-------
*/

#ifndef ICON_ICON_HH
#define ICON_ICON_HH

// Standard C
#include <stdint.h>


namespace icon
{
	
	union icon32_data
	{
		uint32_t u32[ 32 * 32 / 32 ];  // 32 32-bit ints
		uint16_t u16[ 32 * 32 / 16 ];  // 64 16-bit ints
		uint8_t  u8 [ 32 * 32 /  8 ];  // 128 8-bit ints
	};
	
	struct icon32_x2_data
	{
		icon32_data icon;
		icon32_data mask;
	};
	
	inline
	bool test( const icon32_data& data, int x, int y )
	{
		//return data.u32[ y ] & (1 << (31 - x));  // big-endian only
		
		return data.u8[ (y << 2) | (x >> 3) ] & (1 << (7 - (x & 0x7)));
	}
	
	inline
	void set( icon32_data& data, int x, int y )
	{
		//data.u32[ y ] |= (1 << (31 - x));  // big-endian only
		
		data.u8[ (y << 2) | (x >> 3) ] |= 1 << (7 - (x & 0x7));
	}
	
	inline
	void clear( icon32_data& data, int x, int y )
	{
		//data.u32[ y ] &= ~(1 << (31 - x));  // big-endian only
		
		data.u8[ (y << 2) | (x >> 3) ] &= ~(1 << (7 - (x & 0x7)));
	}
	
}

#endif
