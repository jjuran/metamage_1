/*
	escape.hh
	---------
*/

#ifndef ESCAPE_HH
#define ESCAPE_HH

// plus
#include "plus/string.hh"


plus::string escaped( const plus::string& str );

plus::string escaped_HFS_path( const char* path );

#endif
