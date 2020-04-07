/*
	splode.hh
	---------
*/

#ifndef SPLODE_HH
#define SPLODE_HH

struct EventRecord;

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

#endif
