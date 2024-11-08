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

void send_mouse_event( int fd, const EventRecord& event );
void send_key_event  ( int fd, const EventRecord& event );

}

#endif
