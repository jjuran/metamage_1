/*	==================
 *	CFMLateImporter.hh
 *	==================
 */

#ifndef CFMLATEIMPORTER_HH
#define CFMLATEIMPORTER_HH

// Nitrogen
#ifndef NITROGEN_CODEFRAGMENTS_HH
#include "Nitrogen/CodeFragments.hh"
#endif


namespace CFMLateImporter
{
	
	void ImportLibrary( ConstStr255Param                     weakLinkedLibraryName,
	                    nucleus::owned< CFragConnectionID >  connIDToImport );
	
}

#endif

