/*
	notify.hh
	---------
*/

#ifndef GENIE_NOTIFY_HH
#define GENIE_NOTIFY_HH

// POSIX
#include <pthread.h>


namespace Genie
{
	
	void install_empty_sighup_handler();
	
	int notify_thread_via_sighup( pthread_t thread );
	
}

#endif
