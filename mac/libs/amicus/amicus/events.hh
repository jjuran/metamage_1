/*
	events.hh
	---------
*/

#ifndef AMICUS_EVENTS_HH
#define AMICUS_EVENTS_HH

typedef unsigned char uint8_t;

typedef struct __CGEvent* CGEventRef;

typedef struct OpaqueEventRef* EventRef;

struct CGPoint;
struct EventRecord;

namespace amicus
{

/*
	Command mode is a means of distinguishing between user input to be
	forwarded to the back end application and that to be intepreted by
	the front end.  Pressing a combination of modifier keys activates the
	mode; releasing them without having pressed any other keys enters a
	one-shot mode that interpret the next keystroke as a command and then
	switch back to pass-through (i.e. deactivating command mode).  If other
	keys are pressed before releasing all the modifier keys, they are all
	interpreted as commands and releasing the modifier keys will deactivate
	command mode immediately.
*/

enum CommandMode_state
{
	CommandMode_quasimode = -2,
	CommandMode_activated,
	CommandMode_off,
	CommandMode_oneshot,
};

extern CommandMode_state commandmode_state;

extern int events_fd;

extern CGPoint cursor_limit;
extern CGPoint last_cursor_location;

void move_cursor_to( CGPoint location );

long send_key_event( EventRef event, char c, uint8_t more_attrs = 0 );

bool handle_CGEvent( CGEventRef event );

bool handle_EventRecord( const EventRecord& event );

}

#endif
