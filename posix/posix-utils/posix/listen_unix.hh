/*
	listen_unix.hh
	--------------
*/

#ifndef POSIX_LISTENUNIX_HH
#define POSIX_LISTENUNIX_HH


namespace posix
{
	
	int listen_unix( const char* path, int backlog = 0 );
	
}

#endif
