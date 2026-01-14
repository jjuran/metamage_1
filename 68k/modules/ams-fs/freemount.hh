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

int try_to_get( int fd, const plus::string& path, plus::var_string& data );

int try_to_get( int fd, const char* path, int len, plus::var_string& data );

int try_to_put( int fd, const plus::string& path, const plus::string& data );

int try_to_write( int                  fd,
                  const plus::string&  path,
                  const plus::string&  data,
                  unsigned             offset );

int try_to_list( int fd, const plus::string& path, plus::var_string& r );

#endif
