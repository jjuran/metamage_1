/*	==================
 *	CFMLateImporter.cc
 *	==================
 */

#include "CFMLateImporter.hh"

// CFMLateImport
#include "CFMLateImport.h"

// CFMLateImporter
#include "SavedCFragInitBlock.h"


namespace CFMLateImporter
{
	
	OSStatus ImportLibrary( ConstStr255Param   weakLinkedLibraryName,
	                        CFragConnectionID  connIDToImport )
	{
		const CFragSystem7InitBlock* initBlock = SavedCFragInitBlock();
		
		return CFMLateImportLibrary( &initBlock->fragLocator.u.onDisk,
		                             (CFragConnectionID) initBlock->closureID,
		                             Initialize_SavedCFragInitBlock,
		                             weakLinkedLibraryName,
		                             connIDToImport );
	}
	
}

