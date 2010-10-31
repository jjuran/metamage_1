/*	==================
 *	CFMLateImporter.hh
 *	==================
 */

#ifndef CFMLATEIMPORTER_HH
#define CFMLATEIMPORTER_HH

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_CODEFRAGMENTS_TYPES_CFRAGCONNECTIONID_HH
#include "Mac/CodeFragments/Types/CFragConnectionID.hh"
#endif


namespace CFMLateImporter
{
	
	void ImportLibrary( ConstStr255Param                     weakLinkedLibraryName,
	                    nucleus::owned< CFragConnectionID >  connIDToImport );
	
}

#endif

