/*
	freemount.hh
	------------
*/

#ifndef FREEMOUNT_HH
#define FREEMOUNT_HH

// plus
#include "plus/var_string.hh"

int try_to_get( int I, int O, const plus::string& path, plus::var_string& r );

int try_to_get( const char* begin, unsigned len, plus::var_string& data );

#endif
