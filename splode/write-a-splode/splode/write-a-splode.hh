/*
	write-a-splode.hh
	-----------------
*/

#ifndef SPLODE_WRITEASPLODE_HH
#define SPLODE_WRITEASPLODE_HH

/*
	SPLode implements the Simple Platform-Inclusive Event Lode protocol.
*/

namespace splode
{
	
	typedef unsigned char Byte;
	
	int send_mouse_moved_event( int fd, short x, short y );
	
	template < class Coord >
	inline
	int send_mouse_moved_event( int fd, Coord x, Coord y )
	{
		return send_mouse_moved_event( fd, (short) x, (short) y );
	}
	
	int send_mouse_event( int fd, Byte modes, Byte attrs );
	
	int send_key_event( int fd, Byte c, Byte modes, Byte attrs );
	
}

#endif
