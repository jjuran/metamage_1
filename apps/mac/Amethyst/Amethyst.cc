/*
	Amethyst.cc
	-----------
*/

// frontend-common
#include "frend/display_events.hh"

// Amethyst
#include "blit_CG.hh"
#include "blit_QD.hh"
#include "quickdraw.hh"


#define ON_DISPLAY_EVENT  &on_display_event

#if CONFIG_QUICKDRAW
	typedef QD_blitter Blitter;
#else
	typedef CG_blitter Blitter;
#endif

static bool overlay_enabled;

static
void on_display_event( void* info )
{
	using frend::display_event_set;
	
	display_event_set& events = *(display_event_set*) info;
	
	if ( events.newPalette )
	{
		events.newPalette = false;
		
		if ( const unsigned short* palette = events.clut_palette )
		{
			Blitter::set_palette( palette, events.clut_maximum + 1 );
			
			/*
				We need Blitter::blit() to be called, since
				that's where transcoding samples the palette.
			*/
			
			events.screenBits = true;
		}
	}
	
	if ( events.screenBits )
	{
		events.screenBits = false;
		
		Blitter::blit( (char*) events.addr );
	}
	
	events.repaintDue = false;
	
	if ( events.rasterDone  &&  events.finish )
	{
		events.finish();
	}
}

#include "amnesty/skeleton.cc.hh"
