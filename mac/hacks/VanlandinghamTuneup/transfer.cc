/*
	transfer.cc
	-----------
*/

#include "transfer.hh"


short ScreenRow : 0x0106;

Byte* ScrnBase : 0x0824;

short v_offset;
short h_offset;

UInt16 h_offset_bytes;


void set_8px_at_offset( Byte octet : __D0, UInt16 src_byte_index : __D1 )
{
	if ( src_byte_index >= classic_screen_buffer_size )
	{
		return;
	}
	
	UInt16 x = src_byte_index & 0x3f;
	UInt16 y = src_byte_index >> 6;
	
	x += h_offset_bytes;
	y += v_offset;
	
	Byte* row_addr = ScrnBase + ScreenRow * y;
	
	Byte* dst = row_addr + x;
	
	*dst = octet;
}
