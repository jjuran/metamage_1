/*	==================
 *	CFMLateImporter.hh
 *	==================
 */

#ifndef CFMLATEIMPORTER_HH
#define CFMLATEIMPORTER_HH

// Nitrogen
#include "Nitrogen/CodeFragments.hh"


namespace CFMLateImporter
{
	
	void ImportLibrary( ConstStr255Param                     weakLinkedLibraryName,
	                    nucleus::owned< CFragConnectionID >  connIDToImport );
	
}

#endif

