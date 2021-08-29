/*
	splode.hh
	---------
*/

#ifndef AMICUS_SPLODE_HH
#define AMICUS_SPLODE_HH

struct EventRecord;

namespace amicus
{

static inline
bool is_keypad( signed char vcode )
{
	return (vcode & 0x60) == 0x40;
}

void send_mouse_moved_event( int fd, short x, short y );

template < class Coord >
inline
void send_mouse_moved_event( int fd, Coord x, Coord y )
{
	send_mouse_moved_event( fd, (short) x, (short) y );
}

void send_mouse_event( int fd, short action, unsigned flags );

void send_key_event( int fd, char c, short modes, short attrs );

void send_mouse_event( int fd, const EventRecord& event );
void send_key_event  ( int fd, const EventRecord& event );

}

#endif
