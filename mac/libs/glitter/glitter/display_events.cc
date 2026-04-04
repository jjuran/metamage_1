/*
	display_events.cc
	-----------------
*/

#include "glitter/display_events.hh"

// v68k-cursor
#include "cursor/cursor.hh"

// glfb-common
#include "glfb/glfb.hh"

// frontend-common
#include "frend/cursor.hh"
#include "frend/display_events.hh"


namespace glitter
{

void on_display_event( void* info )
{
	using frend::cursor_state;
	using frend::display_event_set;
	using frend::shared_cursor_state;
	
	display_event_set& events = *(display_event_set*) info;
	
	if ( events.cursorBits )
	{
		events.cursorBits = false;
		
		if ( cursor_state )
		{
			glfb::set_cursor_image( cursor_state );
		}
	}
	
	if ( events.newPalette )
	{
		events.newPalette = false;
		
		if ( const unsigned short* palette = events.clut_palette )
		{
			glfb::set_palette( palette, events.clut_maximum + 1 );
			
			/*
				We need set_screen_image() to be called, since
				that's where transcoding samples the palette.
			*/
			
			events.screenBits = true;
		}
	}
	
	if ( events.screenBits )
	{
		events.screenBits = false;
		
		glfb::set_screen_image( (char*) events.addr );
	}
	
	if ( events.repaintDue )
	{
		events.repaintDue = false;
		
		if ( const shared_cursor_state* cursor = cursor_state )
		{
			int y  = cursor->pointer[ 0 ];
			int x  = cursor->pointer[ 1 ];
			int dy = cursor->hotspot[ 0 ];
			int dx = cursor->hotspot[ 1 ];
			
			glfb::set_cursor_hotspot( dx, dy );
			glfb::set_cursor_location( x, y );
			glfb::set_cursor_visibility( cursor->visible );
		}
		
		if ( events.render )
		{
			events.render();
		}
	}
	
	if ( events.rasterDone  &&  events.finish )
	{
		events.finish();
	}
}

}
