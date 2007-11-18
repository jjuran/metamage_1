/*	==================
 *	CFMLateImporter.cc
 *	==================
 */

#include "CFMLateImporter.hh"

// CFMLateImport
#include "CFMLateImport.h"

// CFMLateImporter
#include "SaveCFragInitBlock.h"


namespace CFMLateImporter
{
	
	OSStatus ImportLibrary( ConstStr255Param   fragToFixExportedRoutineName,
	                        ConstStr255Param   weakLinkedLibraryName,
	                        CFragConnectionID  connIDToImport )
	{
		const CFragSystem7InitBlock* initBlock = SavedCFragInitBlock();
		
		return CFMLateImportLibrary( &initBlock->fragLocator.u.onDisk,
		                             (CFragConnectionID) initBlock->closureID,
		                             fragToFixExportedRoutineName,
		                             weakLinkedLibraryName,
		                             connIDToImport );
	}
	
}

