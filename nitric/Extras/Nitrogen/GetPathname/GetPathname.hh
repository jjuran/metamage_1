/*	==============
 *	GetPathname.hh
 *	==============
 */

#ifndef GETPATHNAME_HH
#define GETPATHNAME_HH

// plus
#include "plus/string.hh"

// Nitrogen
#ifndef MAC_FILES_TYPES_FSDIRSPEC_HH
#include "Mac/Files/Types/FSDirSpec.hh"
#endif


plus::string GetMacPathname( const Mac::FSDirSpec&  dir  );
plus::string GetMacPathname( const FSSpec&          file );


plus::string GetPOSIXPathname( const Mac::FSDirSpec&  dir  );
plus::string GetPOSIXPathname( const FSSpec&          file );

#endif

