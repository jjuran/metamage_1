/*
	events.hh
	---------
*/

#ifndef PEARL_EVENTS_HH
#define PEARL_EVENTS_HH

// Standard C
#include <stdint.h>


union SDL_Event;

namespace Pearl
{

class Cursor;

extern int events_fd;

extern uint32_t pearl_event_class;

enum
{
	kEventPearlUpdate = 1,
	kEventPearlScreenBits = 2,
	kEventPearlCursorBits = 3,

	kEventPearlScaleMultiple = 4,
	kEventPearlIntegerScale = 5,
	kEventPearlFullscreen = 6,
	kEventPearlKeyboardGrab = 7,
	kEventPearlMouseGrab = 8,
};

bool handle_sdl_event( SDL_Event& event, Cursor& cursor );

}

#endif
