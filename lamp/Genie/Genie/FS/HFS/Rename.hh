/*
	Genie/FS/HFS/Rename.hh
	----------------------
*/

#ifndef GENIE_FS_HFS_RENAME_HH
#define GENIE_FS_HFS_RENAME_HH

// Genie
#include "Genie/FS/FSTree.hh"

//#include <Files.h>
struct FSSpec;


namespace Genie
{
	
	void Rename_HFS( const FSSpec& srcFileSpec, const FSTreePtr& destFile );
	
}

#endif

