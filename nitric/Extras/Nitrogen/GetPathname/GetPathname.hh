/*	==============
 *	GetPathname.hh
 *	==============
 */

#ifndef GETPATHNAME_HH
#define GETPATHNAME_HH

// plus
#include "plus/string.hh"

// Nitrogen
#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
#endif


plus::string GetMacPathname( const Nitrogen::FSDirSpec& dir  );
plus::string GetMacPathname( const FSSpec&              file );


plus::string GetPOSIXPathname( const Nitrogen::FSDirSpec& dir  );
plus::string GetPOSIXPathname( const FSSpec&              file );

#endif

