/*
	update_file.hh
	--------------
*/

#ifndef POSIX_UPDATEFILE_HH
#define POSIX_UPDATEFILE_HH


namespace posix
{
	
	int update_file( int fd, const void* current, unsigned size );
	
	int update_file( const char* path, const void* current, unsigned size );
	
}

#endif
