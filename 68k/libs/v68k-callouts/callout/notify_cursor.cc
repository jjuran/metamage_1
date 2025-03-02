/*
	notify_cursor.cc
	----------------
*/

#include "callout/notify_cursor.hh"

// Standard C
#include <string.h>

// iota
#include "iota/endian.hh"

// v68k-update
#include "update/notify.hh"

// v68k-cursor
#include "cursor/cursor.hh"

// v68k-callouts
#include "callout/cursor_state.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k    {
namespace callout {

using v68k::cursor::shared_cursor_state;

using v68k::mem_read;


enum
{
	rts = 0x4E75,
};

static
void update_cursor()
{
#ifdef __RELIX__
	
	return;
	
#endif
	
	v68k::update::notify();
}

int32_t notify_cursor_moved_callout( v68k::processor_state& s )
{
	if ( shared_cursor_state* cursor = cursor_state )
	{
		uint32_t loc = s.d(0);
		
		cursor->pointer[ 0 ] = loc >> 16;
		cursor->pointer[ 1 ] = loc;
		
		update_cursor();
	}
	
	return rts;
}

int32_t notify_cursor_vis_callout( v68k::processor_state& s )
{
	if ( shared_cursor_state* cursor = cursor_state )
	{
		cursor->visible = s.d(0);
		
		update_cursor();
	}
	
	return rts;
}

int32_t notify_cursor_set_callout( v68k::processor_state& s )
{
	if ( shared_cursor_state* cursor = cursor_state )
	{
		const int n_image_bytes = 16 * 16 / 8u * 2;
		const int n_bytes       = n_image_bytes + 4;
		
		const uint32_t a = s.a(0);
		
		const v68k::function_code_t data_space = s.data_space();
		
		const uint8_t* src = s.translate( a, n_bytes, data_space, mem_read );
		
		if ( src  &&  ((long) src & 1) == 0 )
		{
			const uint16_t* crsr = (const uint16_t*) src;
			
			if ( ! iota::is_little_endian() )
			{
				memcpy( cursor, crsr, n_bytes );
			}
			else
			{
				memcpy( cursor, crsr, n_image_bytes );
				
				cursor->hotspot[ 0 ] = iota::u16_from_big( crsr[ 32 ] );
				cursor->hotspot[ 1 ] = iota::u16_from_big( crsr[ 33 ] );
			}
			
			++cursor->seed;
			
			update_cursor();
		}
	}
	
	return rts;
}

}  // namespace callout
}  // namespace v68k
