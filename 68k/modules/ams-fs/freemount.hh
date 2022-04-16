/*
	freemount.hh
	------------
*/

#ifndef FREEMOUNT_HH
#define FREEMOUNT_HH

// plus
#include "plus/var_string.hh"

enum
{
	dirent_size = 64,
};

int try_to_get( int I, int O, const plus::string& path, plus::var_string& r );

inline
int try_to_get( int fd, const plus::string& path, plus::var_string& data )
{
	return try_to_get( fd, fd, path, data );
}

int try_to_get( const char* begin, unsigned len, plus::var_string& data );

int try_to_put( int fd, const plus::string& path, const plus::string& data );

int try_to_write( int                  fd,
                  const plus::string&  path,
                  const plus::string&  data,
                  unsigned             offset );

int try_to_list( int I, int O, const plus::string& path, plus::var_string& r );

inline
int try_to_list( int fd, const plus::string& path, plus::var_string& data )
{
	return try_to_list( fd, fd, path, data );
}

inline
int try_to_list( const plus::string& path, plus::var_string& data )
{
	const int in  = 6;
	const int out = 7;
	
	return try_to_list( in, out, path, data );
}

#endif
