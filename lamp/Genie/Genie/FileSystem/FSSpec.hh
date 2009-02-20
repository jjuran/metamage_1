/*	=========
 *	FSSpec.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_FSSPEC_HH
#define GENIE_FILESYSTEM_FSSPEC_HH

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/FileSystem/FSTree.hh"

// Files.h
//struct FSSpec;


namespace Genie
{
	
	//FSTreePtr FSTreeFromFSSpec( const FSSpec& item );
	
	FSTreePtr FSTreeFromFSDirSpec( const Nitrogen::FSDirSpec& dir );
	
}

#endif

