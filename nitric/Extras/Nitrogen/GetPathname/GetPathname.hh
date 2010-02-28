/*	==============
 *	GetPathname.hh
 *	==============
 */

#ifndef GETPATHNAME_HH
#define GETPATHNAME_HH

// Nitrogen
#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
#endif


std::string GetMacPathname( const Nitrogen::FSDirSpec& dir  );
std::string GetMacPathname( const FSSpec&              file );


std::string GetPOSIXPathname( const Nitrogen::FSDirSpec& dir  );
std::string GetPOSIXPathname( const FSSpec&              file );

#endif

