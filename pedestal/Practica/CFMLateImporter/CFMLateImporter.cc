/*	==================
 *	CFMLateImporter.cc
 *	==================
 */

#include "CFMLateImporter/CFMLateImporter.hh"

// CFMLateImport
#include "CFMLateImport.h"

// Nitrogen
#ifndef MAC_TOOLBOX_UTILITIES_THROWOSSTATUS_HH
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"
#endif

// CFMLateImporter
#include "CFMLateImporter/SavedCFragInitBlock.h"


namespace CFMLateImporter
{
	
	namespace n = nucleus;
	
	
	void ImportLibrary( ConstStr255Param               weakLinkedLibraryName,
	                    n::owned< CFragConnectionID >  connIDToImport )
	{
		const CFragSystem7InitBlock* initBlock = SavedCFragInitBlock();
		
		Mac::ThrowOSStatus( CFMLateImportLibrary( &initBlock->fragLocator.u.onDisk,
		                                          (CFragConnectionID) initBlock->closureID,
		                                          Initialize_SavedCFragInitBlock,
		                                          weakLinkedLibraryName,
		                                          connIDToImport ) );
		
		connIDToImport.release();
	}
	
}

