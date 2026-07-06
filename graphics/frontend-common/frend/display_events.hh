/*
	display_events.hh
	-----------------
*/

#ifndef FREND_DISPLAYEVENTS_HH
#define FREND_DISPLAYEVENTS_HH


namespace frend
{

typedef void (*finish_proc)();
typedef void (*render_proc)();

typedef void (*cursor_vis_proc)( bool visible );

struct display_event_set
{
	finish_proc  finish;
	render_proc  render;
	
	cursor_vis_proc  cursor_vis;
	
	const void*            addr;
	const unsigned short*  clut_palette;
	unsigned               clut_maximum;
	
	bool screenBits;
	bool cursorBits;
	bool newPalette;
	bool repaintDue;
	bool rasterDone;
};

extern display_event_set display_events;

}

#endif
