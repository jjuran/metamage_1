/*	==================
 *	CFMLateImporter.hh
 *	==================
 */

#ifndef CFMLATEIMPORTER_HH
#define CFMLATEIMPORTER_HH

#ifndef __CODEFRAGMENTS__
#include <CodeFragments.h>
#endif
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif


namespace CFMLateImporter
{
	
	OSStatus ImportLibrary( ConstStr255Param   weakLinkedLibraryName,
	                        CFragConnectionID  connIDToImport );
	
}

#endif

