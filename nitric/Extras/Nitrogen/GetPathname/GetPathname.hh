/*	==============
 *	GetPathname.hh
 *	==============
 */

#ifndef GETPATHNAME_HH
#define GETPATHNAME_HH

// plus
#include "plus/string.hh"

// Nitrogen
#include "Mac/Files/Types/FSDirSpec.hh"


plus::string GetMacPathname( const Mac::FSDirSpec&  dir  );
plus::string GetMacPathname( const FSSpec&          file );


plus::string GetPOSIXPathname( const Mac::FSDirSpec&  dir  );
plus::string GetPOSIXPathname( const FSSpec&          file );

#endif

