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
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	void ImportLibrary( ConstStr255Param                weakLinkedLibraryName,
	                    NN::Owned< CFragConnectionID >  connIDToImport )
	{
		const CFragSystem7InitBlock* initBlock = SavedCFragInitBlock();
		
		N::ThrowOSStatus( CFMLateImportLibrary( &initBlock->fragLocator.u.onDisk,
		                                        (CFragConnectionID) initBlock->closureID,
		                                        Initialize_SavedCFragInitBlock,
		                                        weakLinkedLibraryName,
		                                        connIDToImport ) );
		
		connIDToImport.Release();
	}
	
}

