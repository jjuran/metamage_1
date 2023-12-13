/*
	admin.hh
	--------
*/

#ifndef ADMIN_HH
#define ADMIN_HH

/*
	These codes are used only internally, not as part of any protocol.
*/

enum
{
	full_stop_size = -1,
	full_stop_mode = 0x5858,  // 'XX'
	
	shutdown_size = -2,
	shutdown_mode = 0x2d2d,  // '--'
};

#endif
