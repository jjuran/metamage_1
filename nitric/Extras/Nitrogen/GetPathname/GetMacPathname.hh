/*
	GetMacPathname.hh
	-----------------
*/

#ifndef GETMACPATHNAME_HH
#define GETMACPATHNAME_HH

// plus
#include "plus/string.hh"

// #include <Files.h>
struct FSSpec;


plus::string GetMacPathname( const FSSpec& file );

#endif
