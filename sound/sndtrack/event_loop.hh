/*
	event_loop.hh
	-------------
*/

#ifndef EVENTLOOP_HH
#define EVENTLOOP_HH

typedef bool (*backend_api)();

void event_loop( int fd, backend_api backend_start, backend_api backend_stop );

void interrupt( int );

#endif
