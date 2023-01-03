/*
	GetMacPathname.hh
	-----------------
*/

#ifndef GETMACPATHNAME_HH
#define GETMACPATHNAME_HH

// mac-types
#include "mac_types/VRefNum_DirID.hh"

// plus
#include "plus/string.hh"

// #include <Files.h>
struct FSSpec;


plus::string GetMacPathname( const mac::types::VRefNum_DirID&  dir  );
plus::string GetMacPathname( const FSSpec&                     file );

#endif
