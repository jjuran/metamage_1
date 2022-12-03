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

extern int events_fd;

extern CGPoint cursor_limit;
extern CGPoint last_cursor_location;

void move_cursor_to( CGPoint location );

long send_key_event( EventRef event, char c, uint8_t more_attrs = 0 );

bool handle_CGEvent( CGEventRef event );

bool handle_EventRecord( const EventRecord& event );

void install_rawKey_handler();

}

#endif
