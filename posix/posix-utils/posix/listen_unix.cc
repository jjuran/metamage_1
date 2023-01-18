/*
	listen_unix.cc
	--------------
*/

#include "posix/listen_unix.hh"

// POSIX
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

// Standard C
#include <errno.h>
#include <string.h>


#pragma exceptions off


namespace posix
{

static
int close_without_errno( int fd )
{
	int saved_errno = errno;
	
	close( fd );
	
	errno = saved_errno;
	
	return -1;
}

int listen_unix( const char* path, int backlog )
{
	const size_t path_len = strlen( path );
	
	if ( path_len >= sizeof sockaddr_un().sun_path )
	{
		errno = ENAMETOOLONG;
		return -1;
	}
	
	int s = socket( PF_UNIX, SOCK_STREAM, 0 );
	
	if ( s < 0 )
	{
		return s;
	}
	
	/*
		We don't fully error-check the lstat() or unlink() calls below.
		The goal is to delete any existing socket file.  If a non-socket
		exists, or if something fails (like path resolution), the bind()
		call will catch it.
	*/
	
	struct stat st;
	int nok = lstat( path, &st );
	int ok = ! nok;
	
	if ( ok  &&  S_ISSOCK( st.st_mode ) )
	{
		unlink( path );
	}
	
	sockaddr_un un = { 0 };
	
	un.sun_family = AF_UNIX;
	
	memcpy( un.sun_path, path, path_len + 1 );
	
	if ( bind( s, (const sockaddr*) &un, sizeof un ) != 0 )
	{
		return close_without_errno( s );
	}
	
	if ( backlog == 0 )
	{
		backlog = 5;
	}
	
	if ( listen( s, backlog ) != 0 )
	{
		return close_without_errno( s );
	}
	
	return s;
}

}
