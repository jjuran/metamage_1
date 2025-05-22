/*
	cursor.cc
	---------
*/

#include "callout/cursor.hh"

// cursorlib
#include "plot_cursor.hh"

// v68k-screen
#include "screen/lock.hh"
#include "screen/surface.hh"
#include "screen/update.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


#ifndef NULL
#define NULL  0L
#endif

#define plot_cursor_2 plot_cursor_to_2bpp
#define plot_cursor_4 plot_cursor_to_4bpp


namespace v68k    {
namespace callout {

using v68k::mem_read;
using v68k::mem_write;
using v68k::mem_update;


enum
{
	rts = 0x4E75,
};

int32_t plot_cursor_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	uint32_t dest_addr = s.a(0);
	uint32_t crsr_addr = s.a(1);
	
	uint16_t shift     = s.d(0);
	int16_t  h_trim    = s.d(0) >> 16;
	uint16_t v_skip    = s.d(1);
	uint16_t n         = s.d(1) >> 16;
	uint16_t stride    = s.d(2);
	uint16_t log_depth = s.d(2) >> 16;
	
	if ( n == 0 )
	{
		return rts;
	}
	
	const uint16_t cursor_rowBytes = 2 << log_depth;
	
	const uint16_t trailing = (shift  &&  ! h_trim) * 2;
	
	const uint32_t crsr_len = 64;  // We don't use the hotspot here
	const uint32_t dest_len = (n - 1) * stride + cursor_rowBytes + trailing;
	
	const uint8_t* p = s.translate( crsr_addr, crsr_len, data_space, mem_read );
	
	if ( p == NULL )
	{
		return v68k::Bus_error;
	}
	
	uint8_t* addr = s.translate( dest_addr, dest_len, data_space, mem_write );
	
	if ( addr == NULL )
	{
		return v68k::Bus_error;
	}
	
	const uint16_t* p_face = (const uint16_t*) p;
	const uint16_t* p_mask = (const uint16_t*) p + 16;
	
	if ( log_depth == 0 )
	{
		plot_cursor( p_face, p_mask, addr, shift, h_trim, v_skip, n, stride );
	}
	else if ( log_depth == 1 )
	{
		plot_cursor_2( p_face, p_mask, addr, shift, h_trim, v_skip, n, stride );
	}
	else if ( log_depth == 2 )
	{
		plot_cursor_4( p_face, p_mask, addr, shift, h_trim, v_skip, n, stride );
	}
	
	s.translate( dest_addr, dest_len, data_space, mem_update );
	
	return rts;
}

}  // namespace callout
}  // namespace v68k
