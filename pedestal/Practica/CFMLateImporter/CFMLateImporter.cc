/*	==================
 *	CFMLateImporter.cc
 *	==================
 */

#include "CFMLateImporter/CFMLateImporter.hh"

// CFMLateImport
#include "CFMLateImport.h"

// CFMLateImporter
#include "CFMLateImporter/SavedCFragInitBlock.h"


namespace CFMLateImporter
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	void ImportLibrary( ConstStr255Param               weakLinkedLibraryName,
	                    n::owned< CFragConnectionID >  connIDToImport )
	{
		const CFragSystem7InitBlock* initBlock = SavedCFragInitBlock();
		
		N::ThrowOSStatus( CFMLateImportLibrary( &initBlock->fragLocator.u.onDisk,
		                                        (CFragConnectionID) initBlock->closureID,
		                                        Initialize_SavedCFragInitBlock,
		                                        weakLinkedLibraryName,
		                                        connIDToImport ) );
		
		connIDToImport.release();
	}
	
}

